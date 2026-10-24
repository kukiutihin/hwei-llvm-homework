#include <format>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CmpPredicate.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/BranchProbability.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

struct SkibPass : public PassInfoMixin<SkibPass> {
  std::size_t blockCounter = 0;
  std::size_t varCounter = 0;
  std::size_t intrinsicCounter = 0;

  using VarTable = std::unordered_map<const Value*, std::string>;

  std::string genTypeGetter(const Type* type) {
    switch (type->getTypeID()) {
      case Type::FloatTyID:
        return "builder.getFloatTy()";

      case Type::VoidTyID:
        return "builder.getVoidTy()";

      case Type::IntegerTyID:
        return std::format("builder.getInt{}Ty()", type->getIntegerBitWidth());

      case Type::PointerTyID:
        return "builder.getPtrTy()";

      case Type::DoubleTyID:
        return "builder.getDoubleTy()";

      case Type::FixedVectorTyID:
        if (auto* vt = dyn_cast<FixedVectorType>(type)) {
          return std::format("FixedVectorType::get({}, {})",
                             genTypeGetter(vt->getElementType()),
                             vt->getNumElements());
        }
      case Type::ArrayTyID:
        if (auto* at = dyn_cast<ArrayType>(type)) {
          return std::format("ArrayType::get({}, {})",
                             genTypeGetter(at->getElementType()),
                             at->getNumElements());
        }
      default:
        return "skibType";
    }
  }

  std::string genConstantGetter(const Type* type, auto value) {
    std::string_view suff = type->isFloatingPointTy() ? "FP" : "Int";
    return std::format("Constant{}::get({}, {})", suff, genTypeGetter(type),
                       value);
  }

  // Operand may be:
  // int const
  // float const
  // global variable
  // vector
  // value received at some point
  std::string resolveOp(const Value* op, const VarTable& table) {
    if (auto* constInt = dyn_cast<ConstantInt>(op)) {
      return std::format("builder.getInt{}({})", constInt->getBitWidth(),
                         std::to_string(constInt->getZExtValue()));
    }

    if (auto* constFP = dyn_cast<ConstantFP>(op))
      return std::format(
          "ConstantFP::get({}, {})", genTypeGetter(constFP->getType()),
          std::to_string(constFP->getValueAPF().convertToFloat()));

    if (auto* global = dyn_cast<GlobalVariable>(op))
      return std::format("module->getGlobalVariable(\"{}\")",
                         global->getName().str());

    if (auto* vector = dyn_cast<ConstantDataVector>(op)) {
      if (vector->isSplat())
        return std::format("ConstantDataVector::getSplat({}, {})",
                           vector->getNumElements(),
                           genConstantGetter(vector->getElementType(),
                                             vector->getElementAsDouble(0)));
      else
        return "skib";
    }

    return table.at(op);
  }

  std::string genFuncType(const Function* func) {
    const auto returnType = genTypeGetter(func->getReturnType());

    std::stringstream argsBuffer;
    argsBuffer << "{";
    if (!func->args().empty()) {
      auto it = func->args().begin();
      argsBuffer << genTypeGetter(it->getType());

      for (it = std::next(it); it != func->args().end(); it = std::next(it))
        argsBuffer << ", " << genTypeGetter(it->getType());
    }

    argsBuffer << "}";

    return std::format("FunctionType::get({}, {}, false)", returnType,
                       argsBuffer.str());
  }

  std::string genFunctionDeclaration(const Function& func) {
    const auto& name = func.getName().str();

    if (func.isIntrinsic()) {
      intrinsicCounter++;
      return std::format(
          "FunctionType* intrinsic{}FuncType = {};\n"
          "Function* intrinsic{}Func = Function::Create(intrinsic{}FuncType,"
          "Function::ExternalLinkage, \"{}\", module);",
          intrinsicCounter, genFuncType(&func), intrinsicCounter,
          intrinsicCounter, name);
    }

    return std::format(
        "FunctionType* {}FuncType = {};\n"
        "Function* {}Func = Function::Create({}FuncType,"
        "Function::ExternalLinkage, \"{}\", module);",
        name, genFuncType(&func), name, name, name);
  }

  std::string genFunctionDefinition(const Function& func) {
    VarTable table;
    std::stringstream buffer;
    const auto& name = func.getName().str();

    // saves func args into values
    for (std::size_t i = 0; i < func.arg_size(); i++) {
      table[func.getArg(i)] = std::format("val{}", varCounter);
      buffer << std::format("Value* val{} = {}Func->getArg({});", varCounter,
                            name, i);
      varCounter++;
    }

    // collect all function basic blocks before main loop
    for (const auto& bb : func) {
      const auto& bbname = "BB" + std::to_string(blockCounter);
      blockCounter++;
      table[&bb] = bbname;

      buffer << std::format(
          "BasicBlock* {} = BasicBlock::Create(context, \"{}\", {}Func);\n",
          bbname, bbname, name);
    }

    // we will define phi-nodes and fill them later
    std::vector<PhiNode> phiNodes;

    ReversePostOrderTraversal<const Function*> traverse(&func);

    for (const auto& bb : traverse) {
      buffer << std::format("builder.SetInsertPoint({});\n\n",
                            resolveOp(bb, table));
      for (const auto& instr : *bb) buffer << genInstr(instr, table, phiNodes);
    }

    // now
    for (const auto& phi : phiNodes) {
      for (const auto [inc, block] : phi.incomings) {
        buffer << std::format("{}->addIncoming({}, {});", phi.name,
                              resolveOp(inc, table), table.at(block));
      }
    }

    buffer << std::format(
        "outs() << (!(verifyFunction(*{}Func, &outs())) ?"
        "\"\" : \"VERIFICATION FAIL: {}\\n\\n\");\n\n",
        name, func.getName().str());

    return buffer.str();
  }

  std::string genBinValue(const Instruction& instr, const VarTable& table,
                          const std::string_view name) {
    return std::format("builder.Create{}({}, {});", name,
                       resolveOp(instr.getOperand(0), table),
                       resolveOp(instr.getOperand(1), table));
  };

  template <typename INST>
  std::string genValAndType(const Instruction& raw, const VarTable& table,
                            const std::string_view name) {
    if (auto* inst = dyn_cast<INST>(&raw))
      return std::format("builder.Create{}({}, {});", name,
                         resolveOp(raw.getOperand(0), table),
                         genTypeGetter(inst->getDestTy()));
    else
      return "";
  }

  std::string genCmp(const Instruction& instr, std::string_view name,
                     const VarTable& table) {
    std::string res = "skibCmp";
    if (auto* cmp = dyn_cast<CmpInst>(&instr)) {
      std::string predicate = genPredicate(cmp->getPredicate());
      res = std::format("builder.Create{}({}, {}, {});", name, predicate,
                        resolveOp(instr.getOperand(0), table),
                        resolveOp(instr.getOperand(1), table));
    }
    return res;
  }

  std::string genPredicate(const CmpPredicate& pred) {
    switch (pred) {
      case CmpInst::ICMP_EQ:
        return "CmpInst::Predicate::ICMP_EQ";
      case CmpInst::ICMP_UGT:
        return "CmpInst::Predicate::ICMP_UGT";
      case CmpInst::ICMP_ULT:
        return "CmpInst::Predicate::ICMP_ULT";
      case CmpInst::ICMP_SGT:
        return "CmpInst::Predicate::ICMP_SGT";
      case CmpInst::ICMP_SLT:
        return "CmpInst::Predicate::ICMP_SLT";
      case CmpInst::FCMP_UGT:
        return "CmpInst::Predicate::FCMP_UGT";
      case CmpInst::FCMP_ULT:
        return "CmpInst::Predicate::FCMP_ULT";
      default:
        return "skibPred";
    }
  }

  std::string handleCallArgs(const CallBase* cb, VarTable& table) {
    if (cb->arg_empty()) return "";
    std::stringstream buffer;
    auto it = cb->args().begin();
    buffer << resolveOp(*it, table);
    for (auto i = std::next(it); i != cb->args().end(); i = std::next(i))
      buffer << ", " << resolveOp(*i, table);
    return buffer.str();
  }

  using Incomings = std::vector<std::pair<const Value*, const BasicBlock*>>;
  struct PhiNode {
    std::string name;
    Incomings incomings;
  };

  std::string genInstr(const Instruction& instr, VarTable& table,
                       std::vector<PhiNode>& phiNodes) {
    std::stringstream buffer;

    auto varName = std::format("val{}", varCounter);
    buffer << "Value* " << varName << " = ";
    const Value* val = &instr;
    table[val] = varName;
    varCounter++;

    const auto binWrapper = [this, &table, &buffer,
                             &instr](const std::string_view name) {
      buffer << genBinValue(instr, table, name);
    };

    switch (instr.getOpcode()) {
      case Instruction::Add:
        binWrapper("Add");
        break;

      case Instruction::Mul:
        binWrapper("Mul");
        break;

      case Instruction::Sub:
        binWrapper("Sub");
        break;

      case Instruction::FAdd:
        binWrapper("FAdd");
        break;

      case Instruction::FSub:
        binWrapper("FSub");
        break;

      case Instruction::FMul:
        binWrapper("FMul");
        break;

      case Instruction::FDiv:
        binWrapper("FDiv");
        break;

      case Instruction::And:
        binWrapper("And");
        break;

      case Instruction::Shl:
        binWrapper("Shl");
        break;

      case Instruction::LShr:
        binWrapper("LShr");
        break;

      case Instruction::Or:
        binWrapper("Or");
        break;

      case Instruction::Ret:
        if (instr.operands().begin() == instr.operands().end())
          buffer << std::format("builder.CreateRetVoid();");
        else
          buffer << std::format("builder.CreateRet({});",
                                resolveOp(instr.getOperand(0), table));
        break;

      case Instruction::UIToFP:
        buffer << genValAndType<UIToFPInst>(instr, table, "UIToFP");
        break;

      case Instruction::FPToUI: {
        buffer << genValAndType<FPToUIInst>(instr, table, "FPToUI");
        break;
      }

      case Instruction::FPTrunc:
        buffer << genValAndType<FPTruncInst>(instr, table, "FPTrunc");
        break;

      case Instruction::FPExt:
        buffer << genValAndType<FPExtInst>(instr, table, "FPExt");
        break;

      case Instruction::ZExt:
        buffer << genValAndType<ZExtInst>(instr, table, "ZExt");
        break;

      case Instruction::FCmp:
        buffer << genCmp(instr, "FCmp", table);
        break;

      case Instruction::ICmp:
        buffer << genCmp(instr, "ICmp", table);
        break;

      case Instruction::GetElementPtr:
        if (auto* gep = dyn_cast<GetElementPtrInst>(&instr)) {
          std::string_view method;
          if (gep->isInBounds())
            method = "CreateInBoundsGEP";
          else
            method = "CreateGEP";

          std::stringstream indexes;
          indexes << "ArrayRef<Value*>{";
          indexes << resolveOp(gep->getOperand(1), table);
          for (std::size_t i = 2; i < gep->getNumOperands(); i++)
            indexes << ", " << resolveOp(gep->getOperand(i), table);

          indexes << "}";

          buffer << std::format("builder.{}({}, {}, {});\n", method,
                                genTypeGetter(gep->getSourceElementType()),
                                resolveOp(gep->getPointerOperand(), table),
                                indexes.str());
        }
        break;

      case Instruction::Call:
        if (auto* cb = dyn_cast<CallBase>(&instr)) {
          Function* func = dyn_cast<Function>(cb->getCalledOperand());
          if (!func) return "";
          if (func->isIntrinsic()) {
            auto iid = func->getIntrinsicID();
            if (iid == Intrinsic::dbg_value || iid == Intrinsic::dbg_declare ||
                iid == Intrinsic::lifetime_start ||
                iid == Intrinsic::lifetime_end) {
              return "";
            }
          }
          buffer << std::format(
              "builder.CreateCall({}, module->getFunction(\"{}\"), {{{}}});\n",
              genFuncType(func), func->getName().str(),
              handleCallArgs(cb, table));
        }
        break;

      case Instruction::Select:
        if (auto* select = dyn_cast<SelectInst>(&instr)) {
          const auto* cond = select->getCondition();
          const auto* then = select->getSuccessor(0);
          const auto* els = select->getSuccessor(1);
          buffer << std::format("builder.CreateSelect({}, {}, {});\n",
                                resolveOp(cond, table), resolveOp(then, table),
                                resolveOp(els, table));
        }
        break;

      case Instruction::Alloca:
        if (auto* alloca = dyn_cast<AllocaInst>(&instr)) {
          buffer << std::format("builder.CreateAlloca({});\n",
                                genTypeGetter(alloca->getAllocatedType()));
        }
        break;

      case Instruction::Br:
        if (auto* br = dyn_cast<BranchInst>(&instr)) {
          if (br->isConditional()) {
            const auto* cond = br->getCondition();
            const auto* bb1 = br->getSuccessor(0);
            const auto* bb2 = br->getSuccessor(1);
            buffer << std::format("builder.CreateCondBr({}, {}, {});\n",
                                  resolveOp(cond, table), resolveOp(bb1, table),
                                  resolveOp(bb2, table));
          } else {
            const auto* bb = br->getSuccessor(0);
            buffer << std::format("builder.CreateBr({});\n",
                                  resolveOp(bb, table));
          }
        }
        break;

      case Instruction::Store:
        if (auto* st = dyn_cast<StoreInst>(&instr)) {
          buffer << std::format("builder.CreateStore({}, {});\n",
                                resolveOp(st->getValueOperand(), table),
                                resolveOp(st->getPointerOperand(), table));
        }
        break;

      case Instruction::Load:
        if (auto* ld = dyn_cast<LoadInst>(&instr)) {
          buffer << std::format("builder.CreateLoad({}, {});\n",
                                genTypeGetter(ld->getType()),
                                resolveOp(ld->getPointerOperand(), table));
        }
        break;

      case Instruction::Trunc:
        if (auto* trunc = dyn_cast<TruncInst>(&instr)) {
          auto nuw = trunc->hasNoUnsignedWrap() ? "true" : "false";
          auto nsw = trunc->hasNoSignedWrap() ? "true" : "false";
          buffer << std::format("builder.CreateTrunc({}, {}, \"\", {}, {});\n",
                                resolveOp(trunc->getOperand(0), table),
                                genTypeGetter(trunc->getDestTy()), nuw, nsw);
        }
        break;

      case Instruction::PHI:
        if (auto* phi = dyn_cast<PHINode>(&instr)) {
          Incomings incomings;
          for (const auto& block : phi->blocks())
            incomings.push_back({phi->getIncomingValueForBlock(block), block});
          buffer = std::stringstream("");
          phiNodes.push_back(PhiNode{varName, incomings});
          buffer << "PHINode* " << varName << " = ";
          buffer << std::format("builder.CreatePHI({}, {});",
                                genTypeGetter(phi->getOperand(0)->getType()),
                                incomings.size());
        }
        break;

      default:
        buffer << instr.getOpcodeName() << ";";
    }
    buffer << "\n";
    return buffer.str();
  }

  PreservedAnalyses run(Module& mod, ModuleAnalysisManager& mam) {
    LLVMContext& ctx = mod.getContext();
    IRBuilder<> builder(ctx);
    std::stringstream buffer;

    buffer << "#include \"../app/sim.h\"\n"
              "#include \"llvm/ExecutionEngine/ExecutionEngine.h\"\n"
              "#include \"llvm/ExecutionEngine/GenericValue.h\"\n"
              "#include \"llvm/IR/IRBuilder.h\"\n"
              "#include \"llvm/IR/LLVMContext.h\"\n"
              "#include \"llvm/IR/DerivedTypes.h\"\n"
              "#include \"llvm/IR/Module.h\"\n"
              "#include \"llvm/IR/Verifier.h\"\n"
              "#include \"llvm/Support/TargetSelect.h\"\n"
              "#include \"llvm/Support/raw_ostream.h\"\n\n"
              "using namespace llvm;\n\n";

    buffer << "int main() {";

    buffer << std::format(
        "LLVMContext context;"
        "Module *module = new Module(\"{}\", context);"
        "IRBuilder<> builder(context);\n\n",
        mod.getSourceFileName());

    buffer << "uint32_t rawColors[10] = {0x000004, 0x160B39, 0x420A68, "
              "0x6A176E, "
              "0x932667,"
              "0xBA3655, 0xDD513A, 0xF37812, 0xFCA50A, 0xF6E8A0};"
              "auto* globalArrayInit = ConstantDataArray::get(context, "
              "rawColors);"

              "module->getOrInsertGlobal(\"colors\", "
              "globalArrayInit->getType());"
              "auto* globalVar = module->getGlobalVariable(\"colors\");"
              "globalVar->setInitializer(globalArrayInit);\n\n";

    // declare all functions before the main loop
    for (const auto& func : mod.functions())
      buffer << genFunctionDeclaration(func) << "\n";

    for (const auto& func : mod.functions())
      if (!func.isDeclaration() && !func.isIntrinsic())
        buffer << genFunctionDefinition(func) << "\n";

    // interpret
    buffer << "InitializeNativeTarget();\n"
              "LLVMInitializeNativeAsmPrinter();\n"
              "ExecutionEngine* ee = "
              "EngineBuilder(std::unique_ptr<Module>(module)).create();\n"

              "ee->InstallLazyFunctionCreator([&](const std::string& name) -> "
              "void* {\n"
              "if (name == \"_simHasClick\")\n"
              "return reinterpret_cast<void*>(simHasClick);\n"
              "else if (name == \"_simGetClick\")\n"
              "return reinterpret_cast<void*>(simGetClick);\n"
              "if (name == \"_simFlush\") return "
              "reinterpret_cast<void*>(simFlush);\n"
              "if (name == \"_simPutPixel\") return "
              "reinterpret_cast<void*>(simPutPixel);\n"
              "llvm::outs() << \"ERROR: Unknown symbol: \" << name << \"\\n\";"
              "return nullptr;\n});\n"
              "ee->finalizeObject();\n"
              "simInit();\n"
              "ArrayRef<GenericValue> noargs;\n"
              "GenericValue v = ee->runFunction(appFunc, noargs);\n"
              "simExit();\n"
              "return 0;\n";

    buffer << "}";
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
