#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "falls-through-pass.h"
#include "isa.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"
#include "reg-alloc.h"
#include "rgb2lin-pass.h"

using namespace llvm;

constexpr std::size_t GENERALREG_COUNT = 24;
constexpr std::size_t ARGREG_COUNT = 4;

struct SkibPass : public PassInfoMixin<SkibPass> {
  std::unordered_map<BasicBlock*, std::size_t> bbMap;
  std::unordered_map<Value*, std::size_t> registerMap;
  std::unordered_map<Function*, std::size_t> externMap;
  std::unordered_map<Function*, std::size_t> functionMap;

  std::vector<Value*> phis;
  std::unordered_map<BasicBlock*, std::size_t> dummyBeforeJump;

  void fromFunction(Function& func, std::vector<std::unique_ptr<Opcode>>& code,
                    bool isMain = false) {
    if (isMain)
      code.push_back(std::make_unique<Main>());
    else
      code.push_back(std::make_unique<Func>(functionMap.at(&func)));

    bbMap.clear();
    std::size_t idx = 1;
    for (auto& bb : func) bbMap[&bb] = idx++;

    for (auto& bb : func) {
      code.push_back(std::make_unique<Block>(bbMap.at(&bb)));
      for (auto& instr : bb) fromInstr(instr, code, bb);

      code.push_back(std::make_unique<Dummy>());
    }
  }

  std::size_t whichReg(Value* v) const {
    auto it = registerMap.find(v);
    return it != registerMap.end() ? it->second : 0;
  }

  std::size_t whichBlock(BasicBlock* b) const {
    auto it = bbMap.find(b);
    return it != bbMap.end() ? it->second : 0;
  }

  std::size_t whichGlobal(Value*) const { return 1; }

  void assignTo(Value* value, Value* to) { registerMap[value] = whichReg(to); }

  void fromInstr(Instruction& instr, std::vector<std::unique_ptr<Opcode>>& code,
                 BasicBlock& bb) {
    auto dst = whichReg(&instr);
    auto op0 = instr.getNumOperands() > 0 ? instr.getOperand(0) : nullptr;
    auto op1 = instr.getNumOperands() > 1 ? instr.getOperand(1) : nullptr;
    auto lhs = op0 ? whichReg(op0) : 0;
    auto rhs = op1 ? whichReg(op1) : 0;

    switch (instr.getOpcode()) {
      case Instruction::Add:
        if (const auto* c = dyn_cast<ConstantInt>(op1))
          code.push_back(std::make_unique<AddI>(dst, lhs, c->getSExtValue()));
        else
          code.push_back(std::make_unique<Add>(dst, lhs, rhs));
        break;

      case Instruction::Mul:
        if (const auto* c = dyn_cast<ConstantInt>(op1))
          code.push_back(std::make_unique<MulI>(dst, lhs, c->getSExtValue()));
        else
          code.push_back(std::make_unique<Mul>(dst, lhs, rhs));
        break;

      case Instruction::Sub:
        code.push_back(std::make_unique<Sub>(dst, lhs, rhs));
        break;

      case Instruction::FAdd:
        if (const auto* c = dyn_cast<ConstantFP>(op1))
          code.push_back(std::make_unique<FAddI>(
              dst, lhs, c->getValueAPF().convertToDouble()));
        else
          code.push_back(std::make_unique<FAdd>(dst, lhs, rhs));
        break;

      case Instruction::FSub:
        code.push_back(std::make_unique<FSub>(dst, lhs, rhs));
        break;

      case Instruction::FMul: {
        double c = 0.0;
        if (const auto* imm = dyn_cast<ConstantFP>(op1))
          c = imm->getValueAPF().convertToDouble();
        code.push_back(std::make_unique<FMulI>(dst, lhs, c));
        break;
      }

      case Instruction::FDiv: {
        double c = 0.0;
        if (const auto* imm = dyn_cast<ConstantFP>(op1))
          c = imm->getValueAPF().convertToDouble();
        code.push_back(std::make_unique<FDivI>(dst, lhs, c));
        break;
      }

      case Instruction::UIToFP:
        code.push_back(std::make_unique<UiToFp>(dst, lhs));
        break;

      case Instruction::FPToUI:
        code.push_back(std::make_unique<FpToUi>(dst, lhs));
        break;

      case Instruction::And:
        if (const auto* c = dyn_cast<ConstantInt>(op1))
          code.push_back(std::make_unique<AndI>(dst, lhs, c->getZExtValue()));
        else
          code.push_back(std::make_unique<Err>("!and"));
        break;

      case Instruction::Shl:
        if (const auto* c = dyn_cast<ConstantInt>(op1))
          code.push_back(std::make_unique<ShlI>(dst, lhs, c->getZExtValue()));
        else
          code.push_back(std::make_unique<Err>("!shl"));
        break;

      case Instruction::LShr:
        if (const auto* c = dyn_cast<ConstantInt>(op1))
          code.push_back(std::make_unique<LShrI>(dst, lhs, c->getZExtValue()));
        else
          code.push_back(std::make_unique<Err>("!lshr"));
        break;

      case Instruction::Or:
        code.push_back(std::make_unique<Or>(dst, lhs, rhs));
        break;

      case Instruction::Ret:
        if (instr.getNumOperands() == 0)
          code.push_back(std::make_unique<Ret>());
        else
          code.push_back(std::make_unique<RetV>(lhs));
        break;

      case Instruction::ZExt:
      case Instruction::SExt:
      case Instruction::Trunc:
      case Instruction::BitCast:
      case Instruction::Alloca:
        assignTo(&instr, op0);
        break;

      case Instruction::PHI: {
        phis.push_back(&instr);
        break;
      }

      case Instruction::FCmp: {
        const auto* imm = dyn_cast<ConstantFP>(op1);
        if (!imm) {
          code.push_back(std::make_unique<Err>("!fcmp"));
          return;
        }
        double c = imm->getValueAPF().convertToDouble();
        if (const auto* cmp = dyn_cast<FCmpInst>(&instr)) {
          if (cmp->getPredicate() == CmpInst::FCMP_UGT)
            code.push_back(std::make_unique<FGtI>(dst, lhs, c));
          else if (cmp->getPredicate() == CmpInst::FCMP_ULT)
            code.push_back(std::make_unique<FLtI>(dst, lhs, c));
        } else {
          code.push_back(std::make_unique<Err>("!!fcmp"));
        }
        break;
      }

      case Instruction::ICmp: {
        const auto* cmp = dyn_cast<ICmpInst>(&instr);
        if (!cmp) {
          code.push_back(std::make_unique<Err>("!!icmp"));
          break;
        }
        const auto* imm = dyn_cast<ConstantInt>(op1);
        switch (cmp->getPredicate()) {
          case CmpInst::ICMP_EQ:
            if (imm)
              code.push_back(
                  std::make_unique<IEqI>(dst, lhs, imm->getSExtValue()));
            else
              code.push_back(std::make_unique<IEq>(dst, lhs, rhs));
            break;

          case CmpInst::ICMP_ULT:
            if (imm)
              code.push_back(
                  std::make_unique<ILtI>(dst, lhs, imm->getZExtValue()));
            else
              code.push_back(std::make_unique<ILt>(dst, lhs, rhs));
            break;

          case CmpInst::ICMP_SLT:
            if (imm)
              code.push_back(
                  std::make_unique<ISLtI>(dst, lhs, imm->getSExtValue()));
            else
              code.push_back(std::make_unique<ISLt>(dst, lhs, rhs));
            break;

          case CmpInst::ICMP_UGT:
            if (imm)
              code.push_back(std::make_unique<Err>("!icmp_ugt_imm"));
            else
              code.push_back(std::make_unique<IGt>(dst, lhs, rhs));
            break;

          case CmpInst::ICMP_SGT:
            if (imm)
              code.push_back(
                  std::make_unique<ISGtI>(dst, lhs, imm->getSExtValue()));
            else
              code.push_back(std::make_unique<Err>("!icmp_sgt_reg"));
            break;

          default:
            code.push_back(std::make_unique<Err>("!icmp_pred"));
            break;
        }
        break;
      }

      case Instruction::GetElementPtr: {
        auto* gep = dyn_cast<GetElementPtrInst>(&instr);
        if (!gep) {
          code.push_back(std::make_unique<Err>("!gep"));
          break;
        }

        Value* ptrOp = gep->getPointerOperand();
        if (auto* glob = dyn_cast<GlobalVariable>(ptrOp)) {
          auto idx = whichReg(gep->getOperand(gep->getNumIndices()));
          code.push_back(std::make_unique<GepG>(dst, whichGlobal(glob), idx));
          break;
        }

        auto base = whichReg(ptrOp);
        Value* idxVal = gep->getOperand(1);
        bool is8Byte = gep->getSourceElementType()->isDoubleTy();

        if (const auto* cInt = dyn_cast<ConstantInt>(idxVal)) {
          int64_t byteOffset = cInt->getSExtValue() * (is8Byte ? 8 : 1);
          code.push_back(std::make_unique<GepI>(dst, base, byteOffset));
        } else {
          auto idx = whichReg(idxVal);
          if (is8Byte)
            code.push_back(std::make_unique<Gep8>(dst, base, idx));
          else
            code.push_back(std::make_unique<Gep>(dst, base, idx));
        }
        break;
      }

      case Instruction::Call: {
        const auto* cb = dyn_cast<CallBase>(&instr);
        if (!cb) {
          code.push_back(std::make_unique<Err>("!!call"));
          break;
        }

        auto* func =
            dyn_cast<Function>(cb->getCalledOperand()->stripPointerCasts());
        if (!func) {
          code.push_back(std::make_unique<Err>("!call"));
          break;
        }

        if (!externMap.contains(func) && !functionMap.contains(func)) {
          if (cb->arg_size() > 0) assignTo(&instr, cb->getArgOperand(0));
          break;
        }

        if (cb->arg_size() == 2) {
          code.push_back(std::make_unique<Prepare2>(
              whichReg(cb->getArgOperand(0)), whichReg(cb->getArgOperand(1))));
        } else if (cb->arg_size() == 3) {
          code.push_back(std::make_unique<Prepare3>(
              whichReg(cb->getArgOperand(0)), whichReg(cb->getArgOperand(1)),
              whichReg(cb->getArgOperand(2))));
        }

        bool isVoid = cb->getType()->isVoidTy();
        if (func->isIntrinsic() || func->isDeclaration()) {
          auto callee = externMap.at(func);
          if (isVoid)
            code.push_back(std::make_unique<CallE>(callee));
          else
            code.push_back(std::make_unique<CallEV>(dst, callee));
        } else {
          auto callee = functionMap.at(func);
          if (isVoid)
            code.push_back(std::make_unique<Call>(callee));
          else
            code.push_back(std::make_unique<CallV>(dst, callee));
        }
        break;
      }

      case Instruction::Select: {
        if (auto* select = dyn_cast<SelectInst>(&instr)) {
          auto cond = whichReg(select->getCondition());
          auto tru = whichReg(select->getTrueValue());
          auto fls = whichReg(select->getFalseValue());
          code.push_back(std::make_unique<Mov>(dst, fls));
          code.push_back(std::make_unique<CMov>(dst, cond, tru));
        } else {
          code.push_back(std::make_unique<Err>("!select"));
        }
        break;
      }

      case Instruction::Br: {
        if (const auto* br = dyn_cast<BranchInst>(&instr)) {
          // sometimes we want to insert mov here, it needed for phi node
          code.push_back(std::make_unique<Dummy>());
          dummyBeforeJump[&bb] = code.size() - 1;

          if (br->isConditional())
            code.push_back(std::make_unique<CJmp>(
                whichReg(br->getCondition()), whichBlock(br->getSuccessor(0)),
                whichBlock(br->getSuccessor(1))));
          else
            code.push_back(
                std::make_unique<Jmp>(whichBlock(br->getSuccessor(0))));
        } else {
          code.push_back(std::make_unique<Err>("!br"));
        }
        break;
      }

      case Instruction::Store:
        code.push_back(std::make_unique<Store>(lhs, rhs));
        break;

      case Instruction::Load:
        code.push_back(std::make_unique<Load>(dst, lhs));
        break;

      default:
        code.push_back(std::make_unique<Err>("!unknown"));
        break;
    }
  }

  void codeToString(std::stringstream& buffer,
                    const std::vector<std::unique_ptr<Opcode>>& code) {
    for (const auto& opcode : code) {
      if (opcode->type == OpcodeType::Block ||
          opcode->type == OpcodeType::Func || opcode->type == OpcodeType::Main)
        buffer << "\n";
      buffer << opcode->toString();
      if (opcode->type != OpcodeType::Dummy) buffer << "\n";
    }
  }

  void phiPass(std::vector<std::unique_ptr<Opcode>>& code) {
    for (auto phiValue : phis) {
      if (auto* phi = dyn_cast<PHINode>(phiValue)) {
        auto dst = whichReg(phi);
        for (std::size_t i = 0; i < phi->getNumIncomingValues(); i++) {
          auto* inc = phi->getIncomingValue(i);
          auto* block = phi->getIncomingBlock(i);
          auto src = whichReg(inc);
          code[dummyBeforeJump[block]] = std::make_unique<Mov>(dst, src);
        }
      } else
        code.push_back(std::make_unique<Err>("!phi"));
    }
  }

  PreservedAnalyses run(Module& mod, ModuleAnalysisManager&) {
    std::size_t funcCounter = 1;
    std::size_t extCounter = 1;
    std::vector<Function*> exts;

    Function* appFunc = nullptr;
    Function* interpolateFunc = nullptr;

    const std::unordered_set<std::string_view> skipped = {
        "llvm.lifetime.start.p0", "llvm.lifetime.end.p0", "llvm.umin.i64",
        "llvm.memset.p0.i64"};

    registerMap = allocate(mod, GENERALREG_COUNT, skipped);

    for (auto& func : mod.functions()) {
      if (skipped.contains(func.getName())) continue;

      if (func.getName() == "app") {
        appFunc = &func;
      } else if (func.getName() == "interpolate") {
        interpolateFunc = &func;
        functionMap[&func] = funcCounter++;
      } else if (func.isDeclaration() || func.isIntrinsic()) {
        externMap[&func] = extCounter++;
        exts.push_back(&func);
      }
    }

    std::vector<std::unique_ptr<Opcode>> code;
    if (appFunc) fromFunction(*appFunc, code, true);
    if (interpolateFunc) fromFunction(*interpolateFunc, code);

    phiPass(code);
    fallsThroughPass(code);
    lin2RgbPass(code);
    rgb2LinPass(code);

    std::stringstream buffer;
    buffer << "global:\n\tglo1      colors\n\nextern:\n";

    extCounter = 1;
    for (auto* ext : exts) {
      buffer << std::format("\t{: <9} \"{}\"\n",
                            std::format("ext{}", extCounter++),
                            ext->getName().str());
    }

    codeToString(buffer, code);
    outs() << buffer.str();

    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SkibGenPass", "0.0.87",
          [](PassBuilder& pb) {
            pb.registerOptimizerLastEPCallback(
                [](ModulePassManager& mpm, auto, auto) {
                  mpm.addPass(SkibPass{});
                  return true;
                });
          }};
}
