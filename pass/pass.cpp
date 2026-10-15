
#include <sstream>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

struct SkibPass : public PassInfoMixin<SkibPass> {
  Type* int8PtrType;
  Type* voidType;

  bool isLoggerFunc(StringRef name) { return name == "usesLogger"; }

  bool insertUsesLogger(Module& mod, Function& func, IRBuilder<>& builder) {
    FunctionType* usesLogType =
        FunctionType::get(voidType, {int8PtrType}, false);
    FunctionCallee usesLogFunc =
        mod.getOrInsertFunction("usesLogger", usesLogType);

    std::stringstream buffer;
    bool inserted = false;
    for (auto& bb : func) {
      for (auto& instr : bb) {
        if (auto* ret = dyn_cast<ReturnInst>(&instr)) {
          builder.SetInsertPoint(ret);
          auto* str = builder.CreateGlobalString(buffer.str());
          builder.CreateCall(usesLogFunc, {str});
          inserted = true;
          break;
        }

        if (auto* _ = dyn_cast<PHINode>(&instr)) continue;
        for (auto& op : instr.operands()) {
          if (auto* phi = dyn_cast<PHINode>(op)) {
            for (auto& op : phi->operands())
              if (auto* opInstr = dyn_cast<Instruction>(op))
                buffer << instr.getOpcodeName() << " <3 "
                       << opInstr->getOpcodeName() << "\n";
            continue;
          }

          if (auto* opInstr = dyn_cast<Instruction>(op)) {
            buffer << instr.getOpcodeName() << " <3 "
                   << opInstr->getOpcodeName() << "\n";
          }
        }
      }
    }
    return inserted;
  }

  static constexpr StringRef RED = "\033[31m";
  static constexpr StringRef GREEN = "\033[32m";
  static constexpr StringRef RESET = "\033[0m";

  PreservedAnalyses run(Module& mod, ModuleAnalysisManager& mam) {
    outs() << "Module: " << mod.getName() << "\n";
    LLVMContext& ctx = mod.getContext();
    IRBuilder<> builder(ctx);
    int8PtrType = Type::getInt8Ty(ctx)->getPointerTo();
    voidType = PointerType::getVoidTy(ctx);

    for (auto& func : mod) {
      outs() << "  Function: " << func.getName() << "\n";

      if (func.isDeclaration() || isLoggerFunc(func.getName())) {
        outs() << "\n";
        continue;
      }

      if (insertUsesLogger(mod, func, builder))
        outs() << "    usesLogger inserted\n";

      outs() << "    Verification: ";
      if (verifyFunction(func, &outs()))
        outs() << RED << "FAIL" << RESET << "\n";
      else
        outs() << GREEN << "OK" << RESET << "\n";

      outs() << "\n";
    }
    outs() << "\n";
    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SkibPass", "0.0.87", [](PassBuilder& pb) {
            pb.registerOptimizerLastEPCallback(
                [](ModulePassManager& mpm, auto, auto) {
                  mpm.addPass(SkibPass{});
                  return true;
                });
          }};
}
