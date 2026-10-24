#include "../app/sim.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int main() {
  LLVMContext context;
  Module *module = new Module("app.c", context);
  IRBuilder<> builder(context);

  uint32_t rawColors[10] = {0x000004, 0x160B39, 0x420A68, 0x6A176E, 0x932667,
                            0xBA3655, 0xDD513A, 0xF37812, 0xFCA50A, 0xF6E8A0};
  auto *globalArrayInit = ConstantDataArray::get(context, rawColors);
  module->getOrInsertGlobal("colors", globalArrayInit->getType());
  auto *globalVar = module->getGlobalVariable("colors");
  globalVar->setInitializer(globalArrayInit);

  FunctionType *rgb2linearFuncType =
      FunctionType::get(builder.getFloatTy(), {builder.getInt32Ty()}, false);
  Function *rgb2linearFunc = Function::Create(
      rgb2linearFuncType, Function::ExternalLinkage, "rgb2linear", module);
  FunctionType *intrinsic1FuncType = FunctionType::get(
      builder.getVoidTy(), {builder.getInt64Ty(), builder.getPtrTy()}, false);
  Function *intrinsic1Func =
      Function::Create(intrinsic1FuncType, Function::ExternalLinkage,
                       "llvm.lifetime.start.p0", module);
  FunctionType *intrinsic2FuncType =
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false);
  Function *intrinsic2Func = Function::Create(
      intrinsic2FuncType, Function::ExternalLinkage, "llvm.pow.f64", module);
  FunctionType *linear2rgbFuncType =
      FunctionType::get(builder.getInt32Ty(), {builder.getFloatTy()}, false);
  Function *linear2rgbFunc = Function::Create(
      linear2rgbFuncType, Function::ExternalLinkage, "linear2rgb", module);
  FunctionType *intrinsic3FuncType = FunctionType::get(
      builder.getDoubleTy(),
      {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
      false);
  Function *intrinsic3Func =
      Function::Create(intrinsic3FuncType, Function::ExternalLinkage,
                       "llvm.fmuladd.f64", module);
  FunctionType *interpolateFuncType = FunctionType::get(
      builder.getInt32Ty(),
      {builder.getInt32Ty(), builder.getInt32Ty(), builder.getFloatTy()},
      false);
  Function *interpolateFunc = Function::Create(
      interpolateFuncType, Function::ExternalLinkage, "interpolate", module);
  FunctionType *intrinsic4FuncType = FunctionType::get(
      builder.getFloatTy(),
      {builder.getFloatTy(), builder.getFloatTy(), builder.getFloatTy()},
      false);
  Function *intrinsic4Func =
      Function::Create(intrinsic4FuncType, Function::ExternalLinkage,
                       "llvm.fmuladd.f32", module);
  FunctionType *colorByTempFuncType =
      FunctionType::get(builder.getInt32Ty(), {builder.getFloatTy()}, false);
  Function *colorByTempFunc = Function::Create(
      colorByTempFuncType, Function::ExternalLinkage, "colorByTemp", module);
  FunctionType *intrinsic5FuncType =
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false);
  Function *intrinsic5Func = Function::Create(
      intrinsic5FuncType, Function::ExternalLinkage, "llvm.minnum.f64", module);
  FunctionType *intrinsic6FuncType =
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false);
  Function *intrinsic6Func = Function::Create(
      intrinsic6FuncType, Function::ExternalLinkage, "llvm.maxnum.f64", module);
  FunctionType *calcTempFuncType = FunctionType::get(
      builder.getFloatTy(),
      {builder.getInt32Ty(), builder.getInt32Ty(), builder.getPtrTy()}, false);
  Function *calcTempFunc = Function::Create(
      calcTempFuncType, Function::ExternalLinkage, "calcTemp", module);
  FunctionType *recalculateFieldFuncType = FunctionType::get(
      builder.getVoidTy(), {builder.getPtrTy(), builder.getPtrTy()}, false);
  Function *recalculateFieldFunc =
      Function::Create(recalculateFieldFuncType, Function::ExternalLinkage,
                       "recalculateField", module);
  FunctionType *drawFieldFuncType =
      FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);
  Function *drawFieldFunc = Function::Create(
      drawFieldFuncType, Function::ExternalLinkage, "drawField", module);
  FunctionType *simPutPixelFuncType = FunctionType::get(
      builder.getVoidTy(),
      {builder.getInt32Ty(), builder.getInt32Ty(), builder.getInt32Ty()},
      false);
  Function *simPutPixelFunc = Function::Create(
      simPutPixelFuncType, Function::ExternalLinkage, "simPutPixel", module);
  FunctionType *addObjectCircleFuncType = FunctionType::get(
      builder.getVoidTy(), {builder.getInt32Ty(), builder.getPtrTy()}, false);
  Function *addObjectCircleFunc =
      Function::Create(addObjectCircleFuncType, Function::ExternalLinkage,
                       "addObjectCircle", module);
  FunctionType *addObjectRectangleFuncType = FunctionType::get(
      builder.getVoidTy(), {builder.getInt32Ty(), builder.getPtrTy()}, false);
  Function *addObjectRectangleFunc =
      Function::Create(addObjectRectangleFuncType, Function::ExternalLinkage,
                       "addObjectRectangle", module);
  FunctionType *appFuncType = FunctionType::get(builder.getVoidTy(), {}, false);
  Function *appFunc =
      Function::Create(appFuncType, Function::ExternalLinkage, "app", module);
  FunctionType *intrinsic7FuncType =
      FunctionType::get(builder.getVoidTy(),
                        {builder.getPtrTy(), builder.getInt8Ty(),
                         builder.getInt64Ty(), builder.getInt1Ty()},
                        false);
  Function *intrinsic7Func =
      Function::Create(intrinsic7FuncType, Function::ExternalLinkage,
                       "llvm.memset.p0.i64", module);
  FunctionType *simHasClickFuncType =
      FunctionType::get(builder.getInt32Ty(), {}, false);
  Function *simHasClickFunc = Function::Create(
      simHasClickFuncType, Function::ExternalLinkage, "simHasClick", module);
  FunctionType *simGetClickFuncType =
      FunctionType::get(builder.getInt32Ty(), {}, false);
  Function *simGetClickFunc = Function::Create(
      simGetClickFuncType, Function::ExternalLinkage, "simGetClick", module);
  FunctionType *simFlushFuncType =
      FunctionType::get(builder.getVoidTy(), {}, false);
  Function *simFlushFunc = Function::Create(
      simFlushFuncType, Function::ExternalLinkage, "simFlush", module);
  FunctionType *intrinsic8FuncType =
      FunctionType::get(builder.getInt32Ty(),
                        {builder.getInt32Ty(), builder.getInt32Ty()}, false);
  Function *intrinsic8Func = Function::Create(
      intrinsic8FuncType, Function::ExternalLinkage, "llvm.umin.i32", module);
  Value *val0 = rgb2linearFunc->getArg(0);
  BasicBlock *BB0 = BasicBlock::Create(context, "BB0", rgb2linearFunc);
  BasicBlock *BB1 = BasicBlock::Create(context, "BB1", rgb2linearFunc);
  BasicBlock *BB2 = BasicBlock::Create(context, "BB2", rgb2linearFunc);
  BasicBlock *BB3 = BasicBlock::Create(context, "BB3", rgb2linearFunc);
  builder.SetInsertPoint(BB0);

  Value *val1 = builder.CreateUIToFP(val0, builder.getFloatTy());
  Value *val2 = builder.CreateFDiv(
      val1, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val3 = builder.CreateFPExt(val2, builder.getDoubleTy());
  Value *val4 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val3,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val5 = builder.CreateCondBr(val4, BB2, BB1);

  builder.SetInsertPoint(BB1);

  Value *val6 = builder.CreateFDiv(
      val3, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val7 = builder.CreateBr(BB3);

  builder.SetInsertPoint(BB2);

  Value *val8 = builder.CreateFAdd(
      val3, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val9 = builder.CreateFDiv(
      val8, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val10 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val9, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val11 = builder.CreateBr(BB3);

  builder.SetInsertPoint(BB3);

  PHINode *val12 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val13 = builder.CreateFPTrunc(val12, builder.getFloatTy());
  Value *val14 = builder.CreateRet(val13);
  val12->addIncoming(val6, BB1);
  val12->addIncoming(val10, BB2);
  outs() << (!(verifyFunction(*rgb2linearFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: rgb2linear\n\n");

  Value *val15 = linear2rgbFunc->getArg(0);
  BasicBlock *BB4 = BasicBlock::Create(context, "BB4", linear2rgbFunc);
  BasicBlock *BB5 = BasicBlock::Create(context, "BB5", linear2rgbFunc);
  BasicBlock *BB6 = BasicBlock::Create(context, "BB6", linear2rgbFunc);
  BasicBlock *BB7 = BasicBlock::Create(context, "BB7", linear2rgbFunc);
  builder.SetInsertPoint(BB4);

  Value *val16 = builder.CreateFPExt(val15, builder.getDoubleTy());
  Value *val17 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val16,
                         ConstantFP::get(builder.getDoubleTy(), 0.003131));
  Value *val18 = builder.CreateCondBr(val17, BB6, BB5);

  builder.SetInsertPoint(BB5);

  Value *val19 = builder.CreateFMul(
      val16, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val20 = builder.CreateBr(BB7);

  builder.SetInsertPoint(BB6);

  Value *val21 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val16, ConstantFP::get(builder.getDoubleTy(), 0.416667)});

  Value *val22 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val21, ConstantFP::get(builder.getDoubleTy(), 1.055000),
       ConstantFP::get(builder.getDoubleTy(), -0.055000)});

  Value *val23 = builder.CreateBr(BB7);

  builder.SetInsertPoint(BB7);

  PHINode *val24 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val25 = builder.CreateFPTrunc(val24, builder.getFloatTy());
  Value *val26 = builder.CreateFMul(
      val25, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val27 = builder.CreateFPToUI(val26, builder.getInt8Ty());
  Value *val28 = builder.CreateZExt(val27, builder.getInt32Ty());
  Value *val29 = builder.CreateRet(val28);
  val24->addIncoming(val19, BB5);
  val24->addIncoming(val22, BB6);
  outs() << (!(verifyFunction(*linear2rgbFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: linear2rgb\n\n");

  Value *val30 = interpolateFunc->getArg(0);
  Value *val31 = interpolateFunc->getArg(1);
  Value *val32 = interpolateFunc->getArg(2);
  BasicBlock *BB8 = BasicBlock::Create(context, "BB8", interpolateFunc);
  BasicBlock *BB9 = BasicBlock::Create(context, "BB9", interpolateFunc);
  BasicBlock *BB10 = BasicBlock::Create(context, "BB10", interpolateFunc);
  BasicBlock *BB11 = BasicBlock::Create(context, "BB11", interpolateFunc);
  BasicBlock *BB12 = BasicBlock::Create(context, "BB12", interpolateFunc);
  BasicBlock *BB13 = BasicBlock::Create(context, "BB13", interpolateFunc);
  BasicBlock *BB14 = BasicBlock::Create(context, "BB14", interpolateFunc);
  BasicBlock *BB15 = BasicBlock::Create(context, "BB15", interpolateFunc);
  BasicBlock *BB16 = BasicBlock::Create(context, "BB16", interpolateFunc);
  BasicBlock *BB17 = BasicBlock::Create(context, "BB17", interpolateFunc);
  BasicBlock *BB18 = BasicBlock::Create(context, "BB18", interpolateFunc);
  BasicBlock *BB19 = BasicBlock::Create(context, "BB19", interpolateFunc);
  BasicBlock *BB20 = BasicBlock::Create(context, "BB20", interpolateFunc);
  BasicBlock *BB21 = BasicBlock::Create(context, "BB21", interpolateFunc);
  BasicBlock *BB22 = BasicBlock::Create(context, "BB22", interpolateFunc);
  BasicBlock *BB23 = BasicBlock::Create(context, "BB23", interpolateFunc);
  BasicBlock *BB24 = BasicBlock::Create(context, "BB24", interpolateFunc);
  BasicBlock *BB25 = BasicBlock::Create(context, "BB25", interpolateFunc);
  BasicBlock *BB26 = BasicBlock::Create(context, "BB26", interpolateFunc);
  BasicBlock *BB27 = BasicBlock::Create(context, "BB27", interpolateFunc);
  BasicBlock *BB28 = BasicBlock::Create(context, "BB28", interpolateFunc);
  BasicBlock *BB29 = BasicBlock::Create(context, "BB29", interpolateFunc);
  BasicBlock *BB30 = BasicBlock::Create(context, "BB30", interpolateFunc);
  BasicBlock *BB31 = BasicBlock::Create(context, "BB31", interpolateFunc);
  BasicBlock *BB32 = BasicBlock::Create(context, "BB32", interpolateFunc);
  BasicBlock *BB33 = BasicBlock::Create(context, "BB33", interpolateFunc);
  BasicBlock *BB34 = BasicBlock::Create(context, "BB34", interpolateFunc);
  BasicBlock *BB35 = BasicBlock::Create(context, "BB35", interpolateFunc);
  builder.SetInsertPoint(BB8);

  Value *val33 = builder.CreateLShr(val30, builder.getInt32(16));
  Value *val34 = builder.CreateAnd(val33, builder.getInt32(255));
  Value *val35 = builder.CreateUIToFP(val34, builder.getFloatTy());
  Value *val36 = builder.CreateFDiv(
      val35, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val37 = builder.CreateFPExt(val36, builder.getDoubleTy());
  Value *val38 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val37,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val39 = builder.CreateCondBr(val38, BB10, BB9);

  builder.SetInsertPoint(BB9);

  Value *val40 = builder.CreateFDiv(
      val37, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val41 = builder.CreateBr(BB11);

  builder.SetInsertPoint(BB10);

  Value *val42 = builder.CreateFAdd(
      val37, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val43 = builder.CreateFDiv(
      val42, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val44 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val43, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val45 = builder.CreateBr(BB11);

  builder.SetInsertPoint(BB11);

  PHINode *val46 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val47 = builder.CreateFPTrunc(val46, builder.getFloatTy());
  Value *val48 = builder.CreateLShr(val30, builder.getInt32(8));
  Value *val49 = builder.CreateAnd(val48, builder.getInt32(255));
  Value *val50 = builder.CreateUIToFP(val49, builder.getFloatTy());
  Value *val51 = builder.CreateFDiv(
      val50, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val52 = builder.CreateFPExt(val51, builder.getDoubleTy());
  Value *val53 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val52,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val54 = builder.CreateCondBr(val53, BB13, BB12);

  builder.SetInsertPoint(BB12);

  Value *val55 = builder.CreateFDiv(
      val52, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val56 = builder.CreateBr(BB14);

  builder.SetInsertPoint(BB13);

  Value *val57 = builder.CreateFAdd(
      val52, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val58 = builder.CreateFDiv(
      val57, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val59 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val58, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val60 = builder.CreateBr(BB14);

  builder.SetInsertPoint(BB14);

  PHINode *val61 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val62 = builder.CreateFPTrunc(val61, builder.getFloatTy());
  Value *val63 = builder.CreateAnd(val30, builder.getInt32(255));
  Value *val64 = builder.CreateUIToFP(val63, builder.getFloatTy());
  Value *val65 = builder.CreateFDiv(
      val64, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val66 = builder.CreateFPExt(val65, builder.getDoubleTy());
  Value *val67 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val66,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val68 = builder.CreateCondBr(val67, BB16, BB15);

  builder.SetInsertPoint(BB15);

  Value *val69 = builder.CreateFDiv(
      val66, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val70 = builder.CreateBr(BB17);

  builder.SetInsertPoint(BB16);

  Value *val71 = builder.CreateFAdd(
      val66, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val72 = builder.CreateFDiv(
      val71, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val73 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val72, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val74 = builder.CreateBr(BB17);

  builder.SetInsertPoint(BB17);

  PHINode *val75 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val76 = builder.CreateFPTrunc(val75, builder.getFloatTy());
  Value *val77 = builder.CreateLShr(val31, builder.getInt32(16));
  Value *val78 = builder.CreateAnd(val77, builder.getInt32(255));
  Value *val79 = builder.CreateUIToFP(val78, builder.getFloatTy());
  Value *val80 = builder.CreateFDiv(
      val79, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val81 = builder.CreateFPExt(val80, builder.getDoubleTy());
  Value *val82 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val81,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val83 = builder.CreateCondBr(val82, BB19, BB18);

  builder.SetInsertPoint(BB18);

  Value *val84 = builder.CreateFDiv(
      val81, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val85 = builder.CreateBr(BB20);

  builder.SetInsertPoint(BB19);

  Value *val86 = builder.CreateFAdd(
      val81, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val87 = builder.CreateFDiv(
      val86, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val88 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val87, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val89 = builder.CreateBr(BB20);

  builder.SetInsertPoint(BB20);

  PHINode *val90 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val91 = builder.CreateFPTrunc(val90, builder.getFloatTy());
  Value *val92 = builder.CreateLShr(val31, builder.getInt32(8));
  Value *val93 = builder.CreateAnd(val92, builder.getInt32(255));
  Value *val94 = builder.CreateUIToFP(val93, builder.getFloatTy());
  Value *val95 = builder.CreateFDiv(
      val94, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val96 = builder.CreateFPExt(val95, builder.getDoubleTy());
  Value *val97 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val96,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val98 = builder.CreateCondBr(val97, BB22, BB21);

  builder.SetInsertPoint(BB21);

  Value *val99 = builder.CreateFDiv(
      val96, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val100 = builder.CreateBr(BB23);

  builder.SetInsertPoint(BB22);

  Value *val101 = builder.CreateFAdd(
      val96, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val102 = builder.CreateFDiv(
      val101, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val103 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val102, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val104 = builder.CreateBr(BB23);

  builder.SetInsertPoint(BB23);

  PHINode *val105 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val106 = builder.CreateFPTrunc(val105, builder.getFloatTy());
  Value *val107 = builder.CreateAnd(val31, builder.getInt32(255));
  Value *val108 = builder.CreateUIToFP(val107, builder.getFloatTy());
  Value *val109 = builder.CreateFDiv(
      val108, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val110 = builder.CreateFPExt(val109, builder.getDoubleTy());
  Value *val111 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val110,
                         ConstantFP::get(builder.getDoubleTy(), 0.040450));
  Value *val112 = builder.CreateCondBr(val111, BB25, BB24);

  builder.SetInsertPoint(BB24);

  Value *val113 = builder.CreateFDiv(
      val110, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val114 = builder.CreateBr(BB26);

  builder.SetInsertPoint(BB25);

  Value *val115 = builder.CreateFAdd(
      val110, ConstantFP::get(builder.getDoubleTy(), 0.055000));
  Value *val116 = builder.CreateFDiv(
      val115, ConstantFP::get(builder.getDoubleTy(), 1.055000));
  Value *val117 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val116, ConstantFP::get(builder.getDoubleTy(), 2.400000)});

  Value *val118 = builder.CreateBr(BB26);

  builder.SetInsertPoint(BB26);

  PHINode *val119 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val120 = builder.CreateFPTrunc(val119, builder.getFloatTy());
  Value *val121 = builder.CreateFSub(val91, val47);
  Value *val122 = builder.CreateCall(
      FunctionType::get(
          builder.getFloatTy(),
          {builder.getFloatTy(), builder.getFloatTy(), builder.getFloatTy()},
          false),
      module->getFunction("llvm.fmuladd.f32"), {val121, val32, val47});

  Value *val123 = builder.CreateFPExt(val122, builder.getDoubleTy());
  Value *val124 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val123,
                         ConstantFP::get(builder.getDoubleTy(), 0.003131));
  Value *val125 = builder.CreateCondBr(val124, BB28, BB27);

  builder.SetInsertPoint(BB27);

  Value *val126 = builder.CreateFMul(
      val123, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val127 = builder.CreateBr(BB29);

  builder.SetInsertPoint(BB28);

  Value *val128 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val123, ConstantFP::get(builder.getDoubleTy(), 0.416667)});

  Value *val129 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val128, ConstantFP::get(builder.getDoubleTy(), 1.055000),
       ConstantFP::get(builder.getDoubleTy(), -0.055000)});

  Value *val130 = builder.CreateBr(BB29);

  builder.SetInsertPoint(BB29);

  PHINode *val131 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val132 = builder.CreateFSub(val106, val62);
  Value *val133 = builder.CreateCall(
      FunctionType::get(
          builder.getFloatTy(),
          {builder.getFloatTy(), builder.getFloatTy(), builder.getFloatTy()},
          false),
      module->getFunction("llvm.fmuladd.f32"), {val132, val32, val62});

  Value *val134 = builder.CreateFPExt(val133, builder.getDoubleTy());
  Value *val135 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val134,
                         ConstantFP::get(builder.getDoubleTy(), 0.003131));
  Value *val136 = builder.CreateCondBr(val135, BB31, BB30);

  builder.SetInsertPoint(BB30);

  Value *val137 = builder.CreateFMul(
      val134, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val138 = builder.CreateBr(BB32);

  builder.SetInsertPoint(BB31);

  Value *val139 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val134, ConstantFP::get(builder.getDoubleTy(), 0.416667)});

  Value *val140 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val139, ConstantFP::get(builder.getDoubleTy(), 1.055000),
       ConstantFP::get(builder.getDoubleTy(), -0.055000)});

  Value *val141 = builder.CreateBr(BB32);

  builder.SetInsertPoint(BB32);

  PHINode *val142 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val143 = builder.CreateFSub(val120, val76);
  Value *val144 = builder.CreateCall(
      FunctionType::get(
          builder.getFloatTy(),
          {builder.getFloatTy(), builder.getFloatTy(), builder.getFloatTy()},
          false),
      module->getFunction("llvm.fmuladd.f32"), {val143, val32, val76});

  Value *val145 = builder.CreateFPExt(val144, builder.getDoubleTy());
  Value *val146 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_UGT, val145,
                         ConstantFP::get(builder.getDoubleTy(), 0.003131));
  Value *val147 = builder.CreateCondBr(val146, BB34, BB33);

  builder.SetInsertPoint(BB33);

  Value *val148 = builder.CreateFMul(
      val145, ConstantFP::get(builder.getDoubleTy(), 12.920000));
  Value *val149 = builder.CreateBr(BB35);

  builder.SetInsertPoint(BB34);

  Value *val150 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.pow.f64"),
      {val145, ConstantFP::get(builder.getDoubleTy(), 0.416667)});

  Value *val151 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val150, ConstantFP::get(builder.getDoubleTy(), 1.055000),
       ConstantFP::get(builder.getDoubleTy(), -0.055000)});

  Value *val152 = builder.CreateBr(BB35);

  builder.SetInsertPoint(BB35);

  PHINode *val153 = builder.CreatePHI(builder.getDoubleTy(), 2);
  Value *val154 = builder.CreateFPTrunc(val142, builder.getFloatTy());
  Value *val155 = builder.CreateFMul(
      val154, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val156 = builder.CreateFPToUI(val155, builder.getInt8Ty());
  Value *val157 = builder.CreateZExt(val156, builder.getInt32Ty());
  Value *val158 = builder.CreateFPTrunc(val131, builder.getFloatTy());
  Value *val159 = builder.CreateFMul(
      val158, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val160 = builder.CreateFPToUI(val159, builder.getInt8Ty());
  Value *val161 = builder.CreateZExt(val160, builder.getInt32Ty());
  Value *val162 = builder.CreateFPTrunc(val153, builder.getFloatTy());
  Value *val163 = builder.CreateFMul(
      val162, ConstantFP::get(builder.getFloatTy(), 255.000000));
  Value *val164 = builder.CreateFPToUI(val163, builder.getInt8Ty());
  Value *val165 = builder.CreateZExt(val164, builder.getInt32Ty());
  Value *val166 = builder.CreateShl(val161, builder.getInt32(16));
  Value *val167 = builder.CreateShl(val157, builder.getInt32(8));
  Value *val168 = builder.CreateOr(val167, val166);
  Value *val169 = builder.CreateOr(val168, val165);
  Value *val170 = builder.CreateRet(val169);
  val46->addIncoming(val40, BB9);
  val46->addIncoming(val44, BB10);
  val61->addIncoming(val55, BB12);
  val61->addIncoming(val59, BB13);
  val75->addIncoming(val69, BB15);
  val75->addIncoming(val73, BB16);
  val90->addIncoming(val84, BB18);
  val90->addIncoming(val88, BB19);
  val105->addIncoming(val99, BB21);
  val105->addIncoming(val103, BB22);
  val119->addIncoming(val113, BB24);
  val119->addIncoming(val117, BB25);
  val131->addIncoming(val126, BB27);
  val131->addIncoming(val129, BB28);
  val142->addIncoming(val137, BB30);
  val142->addIncoming(val140, BB31);
  val153->addIncoming(val148, BB33);
  val153->addIncoming(val151, BB34);
  outs() << (!(verifyFunction(*interpolateFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: interpolate\n\n");

  Value *val171 = colorByTempFunc->getArg(0);
  BasicBlock *BB36 = BasicBlock::Create(context, "BB36", colorByTempFunc);
  BasicBlock *BB37 = BasicBlock::Create(context, "BB37", colorByTempFunc);
  BasicBlock *BB38 = BasicBlock::Create(context, "BB38", colorByTempFunc);
  builder.SetInsertPoint(BB36);

  Value *val172 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_ULT, val171,
                         ConstantFP::get(builder.getFloatTy(), 100.000000));
  Value *val173 = builder.CreateCondBr(val172, BB37, BB38);

  builder.SetInsertPoint(BB37);

  Value *val174 = builder.CreateFPExt(val171, builder.getDoubleTy());
  Value *val175 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.minnum.f64"),
      {val174, ConstantFP::get(builder.getDoubleTy(), 100.000000)});

  Value *val176 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.maxnum.f64"),
      {val175, ConstantFP::get(builder.getDoubleTy(), 0.000000)});

  Value *val177 = builder.CreateFDiv(
      val176, ConstantFP::get(builder.getDoubleTy(), 10.000000));
  Value *val178 = builder.CreateFPTrunc(val177, builder.getFloatTy());
  Value *val179 = builder.CreateFPToUI(val178, builder.getInt32Ty());
  Value *val180 = builder.CreateZExt(val179, builder.getInt64Ty());
  Value *val181 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val180});

  Value *val182 = builder.CreateLoad(builder.getInt32Ty(), val181);

  Value *val183 = builder.CreateAdd(val179, builder.getInt32(1));
  Value *val184 = builder.CreateZExt(val183, builder.getInt64Ty());
  Value *val185 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val184});

  Value *val186 = builder.CreateLoad(builder.getInt32Ty(), val185);

  Value *val187 = builder.CreateUIToFP(val179, builder.getFloatTy());
  Value *val188 = builder.CreateFSub(val178, val187);
  Value *val189 = builder.CreateCall(
      FunctionType::get(
          builder.getInt32Ty(),
          {builder.getInt32Ty(), builder.getInt32Ty(), builder.getFloatTy()},
          false),
      module->getFunction("interpolate"), {val182, val186, val188});

  Value *val190 = builder.CreateBr(BB38);

  builder.SetInsertPoint(BB38);

  PHINode *val191 = builder.CreatePHI(builder.getInt32Ty(), 2);
  Value *val192 = builder.CreateRet(val191);
  val191->addIncoming(val189, BB37);
  val191->addIncoming(builder.getInt32(15955986), BB36);
  outs() << (!(verifyFunction(*colorByTempFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: colorByTemp\n\n");

  Value *val193 = calcTempFunc->getArg(0);
  Value *val194 = calcTempFunc->getArg(1);
  Value *val195 = calcTempFunc->getArg(2);
  BasicBlock *BB39 = BasicBlock::Create(context, "BB39", calcTempFunc);
  BasicBlock *BB40 = BasicBlock::Create(context, "BB40", calcTempFunc);
  BasicBlock *BB41 = BasicBlock::Create(context, "BB41", calcTempFunc);
  BasicBlock *BB42 = BasicBlock::Create(context, "BB42", calcTempFunc);
  BasicBlock *BB43 = BasicBlock::Create(context, "BB43", calcTempFunc);
  BasicBlock *BB44 = BasicBlock::Create(context, "BB44", calcTempFunc);
  BasicBlock *BB45 = BasicBlock::Create(context, "BB45", calcTempFunc);
  builder.SetInsertPoint(BB39);

  Value *val196 = builder.CreateMul(val194, builder.getInt32(1536));
  Value *val197 = builder.CreateLShr(val196, builder.getInt32(1));
  Value *val198 = builder.CreateAdd(val197, val193);
  Value *val199 = builder.CreateZExt(val198, builder.getInt64Ty());
  Value *val200 = builder.CreateInBoundsGEP(builder.getFloatTy(), val195,
                                            ArrayRef<Value *>{val199});

  Value *val201 = builder.CreateLoad(builder.getFloatTy(), val200);

  Value *val202 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val194,
                                     builder.getInt32(0));
  Value *val203 = builder.CreateCondBr(val202, BB41, BB40);

  builder.SetInsertPoint(BB40);

  Value *val204 = builder.CreateAdd(val196, builder.getInt32(4294965760));
  Value *val205 = builder.CreateLShr(val204, builder.getInt32(1));
  Value *val206 = builder.CreateAdd(val205, val193);
  Value *val207 = builder.CreateZExt(val206, builder.getInt64Ty());
  Value *val208 = builder.CreateInBoundsGEP(builder.getFloatTy(), val195,
                                            ArrayRef<Value *>{val207});

  Value *val209 = builder.CreateLoad(builder.getFloatTy(), val208);

  Value *val210 = builder.CreateICmp(CmpInst::Predicate::ICMP_ULT, val194,
                                     builder.getInt32(383));
  Value *val211 = builder.CreateCondBr(val210, BB41, BB42);

  builder.SetInsertPoint(BB41);

  PHINode *val212 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val213 = builder.CreateAdd(val196, builder.getInt32(1536));
  Value *val214 = builder.CreateLShr(val213, builder.getInt32(1));
  Value *val215 = builder.CreateAdd(val214, val193);
  Value *val216 = builder.CreateZExt(val215, builder.getInt64Ty());
  Value *val217 = builder.CreateInBoundsGEP(builder.getFloatTy(), val195,
                                            ArrayRef<Value *>{val216});

  Value *val218 = builder.CreateLoad(builder.getFloatTy(), val217);

  Value *val219 = builder.CreateBr(BB42);

  builder.SetInsertPoint(BB42);

  PHINode *val220 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val221 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val222 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val193,
                                     builder.getInt32(0));
  Value *val223 = builder.CreateCondBr(val222, BB44, BB43);

  builder.SetInsertPoint(BB43);

  Value *val224 = builder.CreateAdd(val198, builder.getInt32(4294967295));
  Value *val225 = builder.CreateZExt(val224, builder.getInt64Ty());
  Value *val226 = builder.CreateInBoundsGEP(builder.getFloatTy(), val195,
                                            ArrayRef<Value *>{val225});

  Value *val227 = builder.CreateLoad(builder.getFloatTy(), val226);

  Value *val228 = builder.CreateICmp(CmpInst::Predicate::ICMP_ULT, val193,
                                     builder.getInt32(767));
  Value *val229 = builder.CreateCondBr(val228, BB44, BB45);

  builder.SetInsertPoint(BB44);

  PHINode *val230 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val231 = builder.CreateAdd(val198, builder.getInt32(1));
  Value *val232 = builder.CreateZExt(val231, builder.getInt64Ty());
  Value *val233 = builder.CreateInBoundsGEP(builder.getFloatTy(), val195,
                                            ArrayRef<Value *>{val232});

  Value *val234 = builder.CreateLoad(builder.getFloatTy(), val233);

  Value *val235 = builder.CreateBr(BB45);

  builder.SetInsertPoint(BB45);

  PHINode *val236 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val237 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val238 = builder.CreateFPExt(val201, builder.getDoubleTy());
  Value *val239 = builder.CreateFAdd(val220, val221);
  Value *val240 = builder.CreateFAdd(val239, val236);
  Value *val241 = builder.CreateFAdd(val240, val237);
  Value *val242 = builder.CreateFPExt(val241, builder.getDoubleTy());
  Value *val243 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val238, ConstantFP::get(builder.getDoubleTy(), -4.000000), val242});

  Value *val244 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val243, ConstantFP::get(builder.getDoubleTy(), 0.250000), val238});

  Value *val245 = builder.CreateFPTrunc(val244, builder.getFloatTy());
  Value *val246 = builder.CreateRet(val245);
  val212->addIncoming(val209, BB40);
  val212->addIncoming(val201, BB39);
  val220->addIncoming(val212, BB41);
  val220->addIncoming(val209, BB40);
  val221->addIncoming(val218, BB41);
  val221->addIncoming(val201, BB40);
  val230->addIncoming(val227, BB43);
  val230->addIncoming(val201, BB42);
  val236->addIncoming(val230, BB44);
  val236->addIncoming(val227, BB43);
  val237->addIncoming(val234, BB44);
  val237->addIncoming(val201, BB43);
  outs() << (!(verifyFunction(*calcTempFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: calcTemp\n\n");

  Value *val247 = recalculateFieldFunc->getArg(0);
  Value *val248 = recalculateFieldFunc->getArg(1);
  BasicBlock *BB46 = BasicBlock::Create(context, "BB46", recalculateFieldFunc);
  BasicBlock *BB47 = BasicBlock::Create(context, "BB47", recalculateFieldFunc);
  BasicBlock *BB48 = BasicBlock::Create(context, "BB48", recalculateFieldFunc);
  BasicBlock *BB49 = BasicBlock::Create(context, "BB49", recalculateFieldFunc);
  BasicBlock *BB50 = BasicBlock::Create(context, "BB50", recalculateFieldFunc);
  BasicBlock *BB51 = BasicBlock::Create(context, "BB51", recalculateFieldFunc);
  BasicBlock *BB52 = BasicBlock::Create(context, "BB52", recalculateFieldFunc);
  BasicBlock *BB53 = BasicBlock::Create(context, "BB53", recalculateFieldFunc);
  BasicBlock *BB54 = BasicBlock::Create(context, "BB54", recalculateFieldFunc);
  BasicBlock *BB55 = BasicBlock::Create(context, "BB55", recalculateFieldFunc);
  BasicBlock *BB56 = BasicBlock::Create(context, "BB56", recalculateFieldFunc);
  BasicBlock *BB57 = BasicBlock::Create(context, "BB57", recalculateFieldFunc);
  BasicBlock *BB58 = BasicBlock::Create(context, "BB58", recalculateFieldFunc);
  BasicBlock *BB59 = BasicBlock::Create(context, "BB59", recalculateFieldFunc);
  builder.SetInsertPoint(BB46);

  Value *val249 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val247, ArrayRef<Value *>{builder.getInt64(4)});

  Value *val250 = builder.CreateBr(BB47);

  builder.SetInsertPoint(BB47);

  PHINode *val251 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val252 = builder.CreateMul(val251, builder.getInt64(1536));
  Value *val253 = builder.CreateLShr(val252, builder.getInt64(1));
  Value *val254 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val251,
                                     builder.getInt64(0));
  Value *val255 = builder.CreateMul(val251, builder.getInt64(768));
  Value *val256 = builder.CreateAdd(val252, builder.getInt64(4294965760));
  Value *val257 = builder.CreateLShr(val256, builder.getInt64(1));
  Value *val258 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val251,
                                     builder.getInt64(383));
  Value *val259 = builder.CreateAdd(val252, builder.getInt64(1536));
  Value *val260 = builder.CreateLShr(val259, builder.getInt64(1));
  Value *val261 = builder.CreateAnd(val257, builder.getInt64(2147483392));
  Value *val262 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val253});

  Value *val263 = builder.CreateLoad(builder.getFloatTy(), val262);

  Value *val264 = builder.CreateCondBr(val254, BB49, BB48);

  builder.SetInsertPoint(BB48);

  Value *val265 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val261});

  Value *val266 = builder.CreateLoad(builder.getFloatTy(), val265);

  Value *val267 = builder.CreateCondBr(val258, BB50, BB49);

  builder.SetInsertPoint(BB49);

  PHINode *val268 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val269 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val260});

  Value *val270 = builder.CreateLoad(builder.getFloatTy(), val269);

  Value *val271 = builder.CreateBr(BB50);

  builder.SetInsertPoint(BB50);

  PHINode *val272 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val273 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val274 = builder.CreateInBoundsGEP(builder.getFloatTy(), val249,
                                            ArrayRef<Value *>{val253});

  Value *val275 = builder.CreateLoad(builder.getFloatTy(), val274);

  Value *val276 = builder.CreateFPExt(val263, builder.getDoubleTy());
  Value *val277 = builder.CreateFAdd(val272, val273);
  Value *val278 = builder.CreateFAdd(val277, val263);
  Value *val279 = builder.CreateFAdd(val278, val275);
  Value *val280 = builder.CreateFPExt(val279, builder.getDoubleTy());
  Value *val281 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val276, ConstantFP::get(builder.getDoubleTy(), -4.000000), val280});

  Value *val282 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val281, ConstantFP::get(builder.getDoubleTy(), 0.250000), val276});

  Value *val283 = builder.CreateFPTrunc(val282, builder.getFloatTy());
  Value *val284 = builder.CreateInBoundsGEP(builder.getFloatTy(), val248,
                                            ArrayRef<Value *>{val255});

  Value *val285 = builder.CreateStore(val283, val284);

  Value *val286 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val261});

  Value *val287 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val260});

  Value *val288 = builder.CreateInBoundsGEP(builder.getFloatTy(), val248,
                                            ArrayRef<Value *>{val255});

  Value *val289 = builder.CreateBr(BB56);

  builder.SetInsertPoint(BB56);

  PHINode *val290 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val291 = builder.CreateAdd(val290, val253);
  Value *val292 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val291});

  Value *val293 = builder.CreateLoad(builder.getFloatTy(), val292);

  Value *val294 = builder.CreateCondBr(val254, BB58, BB57);

  builder.SetInsertPoint(BB57);

  Value *val295 = builder.CreateInBoundsGEP(builder.getFloatTy(), val286,
                                            ArrayRef<Value *>{val290});

  Value *val296 = builder.CreateLoad(builder.getFloatTy(), val295);

  Value *val297 = builder.CreateCondBr(val258, BB59, BB58);

  builder.SetInsertPoint(BB58);

  PHINode *val298 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val299 = builder.CreateInBoundsGEP(builder.getFloatTy(), val287,
                                            ArrayRef<Value *>{val290});

  Value *val300 = builder.CreateLoad(builder.getFloatTy(), val299);

  Value *val301 = builder.CreateBr(BB59);

  builder.SetInsertPoint(BB59);

  PHINode *val302 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val303 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val304 = builder.CreateAdd(val291, builder.getInt64(4294967295));
  Value *val305 = builder.CreateAnd(val304, builder.getInt64(4294967295));
  Value *val306 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val305});

  Value *val307 = builder.CreateLoad(builder.getFloatTy(), val306);

  Value *val308 = builder.CreateInBoundsGEP(builder.getFloatTy(), val249,
                                            ArrayRef<Value *>{val291});

  Value *val309 = builder.CreateLoad(builder.getFloatTy(), val308);

  Value *val310 = builder.CreateFPExt(val293, builder.getDoubleTy());
  Value *val311 = builder.CreateFAdd(val302, val303);
  Value *val312 = builder.CreateFAdd(val311, val307);
  Value *val313 = builder.CreateFAdd(val312, val309);
  Value *val314 = builder.CreateFPExt(val313, builder.getDoubleTy());
  Value *val315 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val310, ConstantFP::get(builder.getDoubleTy(), -4.000000), val314});

  Value *val316 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val315, ConstantFP::get(builder.getDoubleTy(), 0.250000), val310});

  Value *val317 = builder.CreateFPTrunc(val316, builder.getFloatTy());
  Value *val318 = builder.CreateInBoundsGEP(builder.getFloatTy(), val288,
                                            ArrayRef<Value *>{val290});

  Value *val319 = builder.CreateStore(val317, val318);

  Value *val320 = builder.CreateAdd(val290, builder.getInt64(1));
  Value *val321 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val320,
                                     builder.getInt64(767));
  Value *val322 = builder.CreateCondBr(val321, BB52, BB56);

  builder.SetInsertPoint(BB52);

  Value *val323 = builder.CreateAdd(val320, val253);
  Value *val324 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val323});

  Value *val325 = builder.CreateLoad(builder.getFloatTy(), val324);

  Value *val326 = builder.CreateCondBr(val254, BB54, BB53);

  builder.SetInsertPoint(BB53);

  Value *val327 = builder.CreateInBoundsGEP(builder.getFloatTy(), val286,
                                            ArrayRef<Value *>{val320});

  Value *val328 = builder.CreateLoad(builder.getFloatTy(), val327);

  Value *val329 = builder.CreateCondBr(val258, BB55, BB54);

  builder.SetInsertPoint(BB54);

  PHINode *val330 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val331 = builder.CreateInBoundsGEP(builder.getFloatTy(), val287,
                                            ArrayRef<Value *>{val320});

  Value *val332 = builder.CreateLoad(builder.getFloatTy(), val331);

  Value *val333 = builder.CreateBr(BB55);

  builder.SetInsertPoint(BB55);

  PHINode *val334 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val335 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val336 = builder.CreateAdd(val323, builder.getInt64(4294967295));
  Value *val337 = builder.CreateAnd(val336, builder.getInt64(4294967294));
  Value *val338 = builder.CreateInBoundsGEP(builder.getFloatTy(), val247,
                                            ArrayRef<Value *>{val337});

  Value *val339 = builder.CreateLoad(builder.getFloatTy(), val338);

  Value *val340 = builder.CreateFPExt(val325, builder.getDoubleTy());
  Value *val341 = builder.CreateFAdd(val334, val335);
  Value *val342 = builder.CreateFAdd(val341, val339);
  Value *val343 = builder.CreateFAdd(val342, val325);
  Value *val344 = builder.CreateFPExt(val343, builder.getDoubleTy());
  Value *val345 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val340, ConstantFP::get(builder.getDoubleTy(), -4.000000), val344});

  Value *val346 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val345, ConstantFP::get(builder.getDoubleTy(), 0.250000), val340});

  Value *val347 = builder.CreateFPTrunc(val346, builder.getFloatTy());
  Value *val348 = builder.CreateInBoundsGEP(builder.getFloatTy(), val288,
                                            ArrayRef<Value *>{val320});

  Value *val349 = builder.CreateStore(val347, val348);

  Value *val350 = builder.CreateAdd(val251, builder.getInt64(1));
  Value *val351 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val350,
                                     builder.getInt64(384));
  Value *val352 = builder.CreateCondBr(val351, BB51, BB47);

  builder.SetInsertPoint(BB51);

  Value *val353 = builder.CreateRetVoid();
  val251->addIncoming(builder.getInt64(0), BB46);
  val251->addIncoming(val350, BB55);
  val268->addIncoming(val266, BB48);
  val268->addIncoming(val263, BB47);
  val272->addIncoming(val268, BB49);
  val272->addIncoming(val266, BB48);
  val273->addIncoming(val270, BB49);
  val273->addIncoming(val263, BB48);
  val290->addIncoming(builder.getInt64(1), BB50);
  val290->addIncoming(val320, BB59);
  val298->addIncoming(val296, BB57);
  val298->addIncoming(val293, BB56);
  val302->addIncoming(val298, BB58);
  val302->addIncoming(val296, BB57);
  val303->addIncoming(val300, BB58);
  val303->addIncoming(val293, BB57);
  val330->addIncoming(val328, BB53);
  val330->addIncoming(val325, BB52);
  val334->addIncoming(val330, BB54);
  val334->addIncoming(val328, BB53);
  val335->addIncoming(val332, BB54);
  val335->addIncoming(val325, BB53);
  outs() << (!(verifyFunction(*recalculateFieldFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: recalculateField\n\n");

  Value *val354 = drawFieldFunc->getArg(0);
  BasicBlock *BB60 = BasicBlock::Create(context, "BB60", drawFieldFunc);
  BasicBlock *BB61 = BasicBlock::Create(context, "BB61", drawFieldFunc);
  BasicBlock *BB62 = BasicBlock::Create(context, "BB62", drawFieldFunc);
  BasicBlock *BB63 = BasicBlock::Create(context, "BB63", drawFieldFunc);
  BasicBlock *BB64 = BasicBlock::Create(context, "BB64", drawFieldFunc);
  BasicBlock *BB65 = BasicBlock::Create(context, "BB65", drawFieldFunc);
  BasicBlock *BB66 = BasicBlock::Create(context, "BB66", drawFieldFunc);
  builder.SetInsertPoint(BB60);

  Value *val355 = builder.CreateBr(BB61);

  builder.SetInsertPoint(BB61);

  PHINode *val356 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val357 = builder.CreateMul(val356, builder.getInt64(3072));
  Value *val358 = builder.CreateInBoundsGEP(builder.getInt8Ty(), val354,
                                            ArrayRef<Value *>{val357});

  Value *val359 =
      builder.CreateTrunc(val356, builder.getInt32Ty(), "", true, true);

  Value *val360 = builder.CreateBr(BB64);

  builder.SetInsertPoint(BB64);

  PHINode *val361 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val362 = builder.CreateInBoundsGEP(builder.getFloatTy(), val358,
                                            ArrayRef<Value *>{val361});

  Value *val363 = builder.CreateLoad(builder.getFloatTy(), val362);

  Value *val364 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_ULT, val363,
                         ConstantFP::get(builder.getFloatTy(), 100.000000));
  Value *val365 = builder.CreateCondBr(val364, BB65, BB66);

  builder.SetInsertPoint(BB65);

  Value *val366 = builder.CreateFPExt(val363, builder.getDoubleTy());
  Value *val367 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.minnum.f64"),
      {val366, ConstantFP::get(builder.getDoubleTy(), 100.000000)});

  Value *val368 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.maxnum.f64"),
      {val367, ConstantFP::get(builder.getDoubleTy(), 0.000000)});

  Value *val369 = builder.CreateFDiv(
      val368, ConstantFP::get(builder.getDoubleTy(), 10.000000));
  Value *val370 = builder.CreateFPTrunc(val369, builder.getFloatTy());
  Value *val371 = builder.CreateFPToUI(val370, builder.getInt32Ty());
  Value *val372 = builder.CreateZExt(val371, builder.getInt64Ty());
  Value *val373 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val372});

  Value *val374 = builder.CreateLoad(builder.getInt32Ty(), val373);

  Value *val375 = builder.CreateAdd(val371, builder.getInt32(1));
  Value *val376 = builder.CreateZExt(val375, builder.getInt64Ty());
  Value *val377 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val376});

  Value *val378 = builder.CreateLoad(builder.getInt32Ty(), val377);

  Value *val379 = builder.CreateUIToFP(val371, builder.getFloatTy());
  Value *val380 = builder.CreateFSub(val370, val379);
  Value *val381 = builder.CreateCall(
      FunctionType::get(
          builder.getInt32Ty(),
          {builder.getInt32Ty(), builder.getInt32Ty(), builder.getFloatTy()},
          false),
      module->getFunction("interpolate"), {val374, val378, val380});

  Value *val382 = builder.CreateBr(BB66);

  builder.SetInsertPoint(BB66);

  PHINode *val383 = builder.CreatePHI(builder.getInt32Ty(), 2);
  Value *val384 =
      builder.CreateTrunc(val361, builder.getInt32Ty(), "", true, true);

  Value *val385 = builder.CreateCall(
      FunctionType::get(
          builder.getVoidTy(),
          {builder.getInt32Ty(), builder.getInt32Ty(), builder.getInt32Ty()},
          false),
      module->getFunction("simPutPixel"), {val384, val359, val383});

  Value *val386 = builder.CreateAdd(val361, builder.getInt64(1));
  Value *val387 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val386,
                                     builder.getInt64(768));
  Value *val388 = builder.CreateCondBr(val387, BB63, BB64);

  builder.SetInsertPoint(BB63);

  Value *val389 = builder.CreateAdd(val356, builder.getInt64(1));
  Value *val390 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val389,
                                     builder.getInt64(384));
  Value *val391 = builder.CreateCondBr(val390, BB62, BB61);

  builder.SetInsertPoint(BB62);

  Value *val392 = builder.CreateRetVoid();
  val356->addIncoming(builder.getInt64(0), BB60);
  val356->addIncoming(val389, BB63);
  val361->addIncoming(builder.getInt64(0), BB61);
  val361->addIncoming(val386, BB66);
  val383->addIncoming(val381, BB65);
  val383->addIncoming(builder.getInt32(15955986), BB64);
  outs() << (!(verifyFunction(*drawFieldFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: drawField\n\n");

  Value *val393 = addObjectCircleFunc->getArg(0);
  Value *val394 = addObjectCircleFunc->getArg(1);
  BasicBlock *BB67 = BasicBlock::Create(context, "BB67", addObjectCircleFunc);
  BasicBlock *BB68 = BasicBlock::Create(context, "BB68", addObjectCircleFunc);
  BasicBlock *BB69 = BasicBlock::Create(context, "BB69", addObjectCircleFunc);
  BasicBlock *BB70 = BasicBlock::Create(context, "BB70", addObjectCircleFunc);
  BasicBlock *BB71 = BasicBlock::Create(context, "BB71", addObjectCircleFunc);
  BasicBlock *BB72 = BasicBlock::Create(context, "BB72", addObjectCircleFunc);
  builder.SetInsertPoint(BB67);

  Value *val395 = builder.CreateLShr(val393, builder.getInt32(16));
  Value *val396 = builder.CreateAnd(val393, builder.getInt32(65535));
  Value *val397 = builder.CreateBr(BB68);

  builder.SetInsertPoint(BB68);

  PHINode *val398 = builder.CreatePHI(builder.getInt32Ty(), 2);
  PHINode *val399 = builder.CreatePHI(builder.getInt32Ty(), 2);
  Value *val400 = builder.CreateAdd(val399, val395);
  Value *val401 = builder.CreateSub(val395, val399);
  Value *val402 = builder.CreateAdd(val399, val396);
  Value *val403 = builder.CreateMul(val402, builder.getInt32(1536));
  Value *val404 = builder.CreateLShr(val403, builder.getInt32(1));
  Value *val405 = builder.CreateAdd(val395, val404);
  Value *val406 = builder.CreateSub(val396, val399);
  Value *val407 = builder.CreateMul(val406, builder.getInt32(1536));
  Value *val408 = builder.CreateLShr(val407, builder.getInt32(1));
  Value *val409 = builder.CreateAdd(val395, val408);
  Value *val410 = builder.CreateMul(val399, val399);
  Value *val411 = builder.CreateBr(BB69);

  builder.SetInsertPoint(BB69);

  PHINode *val412 = builder.CreatePHI(builder.getInt32Ty(), 2);
  Value *val413 = builder.CreateICmp(CmpInst::Predicate::ICMP_SGT, val412,
                                     builder.getInt32(4294967295));
  Value *val414 = builder.CreateCondBr(val413, BB70, BB71);

  builder.SetInsertPoint(BB70);

  Value *val415 = builder.CreateAdd(val412, val396);
  Value *val416 = builder.CreateMul(val415, builder.getInt32(1536));
  Value *val417 = builder.CreateLShr(val416, builder.getInt32(1));
  Value *val418 = builder.CreateAdd(val417, val400);
  Value *val419 = builder.CreateZExt(val418, builder.getInt64Ty());
  Value *val420 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val419});

  Value *val421 = builder.CreateLoad(builder.getFloatTy(), val420);

  Value *val422 = builder.CreateFAdd(
      val421, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val423 = builder.CreateStore(val422, val420);

  Value *val424 = builder.CreateSub(val396, val412);
  Value *val425 = builder.CreateMul(val424, builder.getInt32(1536));
  Value *val426 = builder.CreateLShr(val425, builder.getInt32(1));
  Value *val427 = builder.CreateAdd(val426, val400);
  Value *val428 = builder.CreateZExt(val427, builder.getInt64Ty());
  Value *val429 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val428});

  Value *val430 = builder.CreateLoad(builder.getFloatTy(), val429);

  Value *val431 = builder.CreateFAdd(
      val430, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val432 = builder.CreateStore(val431, val429);

  Value *val433 = builder.CreateAdd(val417, val401);
  Value *val434 = builder.CreateZExt(val433, builder.getInt64Ty());
  Value *val435 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val434});

  Value *val436 = builder.CreateLoad(builder.getFloatTy(), val435);

  Value *val437 = builder.CreateFAdd(
      val436, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val438 = builder.CreateStore(val437, val435);

  Value *val439 = builder.CreateAdd(val426, val401);
  Value *val440 = builder.CreateZExt(val439, builder.getInt64Ty());
  Value *val441 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val440});

  Value *val442 = builder.CreateLoad(builder.getFloatTy(), val441);

  Value *val443 = builder.CreateFAdd(
      val442, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val444 = builder.CreateStore(val443, val441);

  Value *val445 = builder.CreateAdd(val412, val405);
  Value *val446 = builder.CreateZExt(val445, builder.getInt64Ty());
  Value *val447 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val446});

  Value *val448 = builder.CreateLoad(builder.getFloatTy(), val447);

  Value *val449 = builder.CreateFAdd(
      val448, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val450 = builder.CreateStore(val449, val447);

  Value *val451 = builder.CreateAdd(val412, val409);
  Value *val452 = builder.CreateZExt(val451, builder.getInt64Ty());
  Value *val453 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val452});

  Value *val454 = builder.CreateLoad(builder.getFloatTy(), val453);

  Value *val455 = builder.CreateFAdd(
      val454, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val456 = builder.CreateStore(val455, val453);

  Value *val457 = builder.CreateSub(val395, val412);
  Value *val458 = builder.CreateAdd(val457, val404);
  Value *val459 = builder.CreateZExt(val458, builder.getInt64Ty());
  Value *val460 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val459});

  Value *val461 = builder.CreateLoad(builder.getFloatTy(), val460);

  Value *val462 = builder.CreateFAdd(
      val461, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val463 = builder.CreateStore(val462, val460);

  Value *val464 = builder.CreateAdd(val457, val408);
  Value *val465 = builder.CreateZExt(val464, builder.getInt64Ty());
  Value *val466 = builder.CreateInBoundsGEP(builder.getFloatTy(), val394,
                                            ArrayRef<Value *>{val465});

  Value *val467 = builder.CreateLoad(builder.getFloatTy(), val466);

  Value *val468 = builder.CreateFAdd(
      val467, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val469 = builder.CreateStore(val468, val466);

  Value *val470 = builder.CreateAdd(val412, builder.getInt32(4294967295));
  Value *val471 = builder.CreateMul(val470, val470);
  Value *val472 = builder.CreateAdd(val471, val410);
  Value *val473 = builder.CreateICmp(CmpInst::Predicate::ICMP_SLT, val472,
                                     builder.getInt32(900));
  Value *val474 = builder.CreateCondBr(val473, BB71, BB69);

  builder.SetInsertPoint(BB71);

  Value *val475 = builder.CreateAdd(val399, builder.getInt32(1));
  Value *val476 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_SLT, val475, val412);
  Value *val477 = builder.CreateCondBr(val476, BB68, BB72);

  builder.SetInsertPoint(BB72);

  Value *val478 = builder.CreateRetVoid();
  val398->addIncoming(builder.getInt32(30), BB67);
  val398->addIncoming(val412, BB71);
  val399->addIncoming(builder.getInt32(0), BB67);
  val399->addIncoming(val475, BB71);
  val412->addIncoming(val470, BB70);
  val412->addIncoming(val398, BB68);
  outs() << (!(verifyFunction(*addObjectCircleFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: addObjectCircle\n\n");

  Value *val479 = addObjectRectangleFunc->getArg(0);
  Value *val480 = addObjectRectangleFunc->getArg(1);
  BasicBlock *BB73 =
      BasicBlock::Create(context, "BB73", addObjectRectangleFunc);
  BasicBlock *BB74 =
      BasicBlock::Create(context, "BB74", addObjectRectangleFunc);
  BasicBlock *BB75 =
      BasicBlock::Create(context, "BB75", addObjectRectangleFunc);
  BasicBlock *BB76 =
      BasicBlock::Create(context, "BB76", addObjectRectangleFunc);
  BasicBlock *BB77 =
      BasicBlock::Create(context, "BB77", addObjectRectangleFunc);
  BasicBlock *BB78 =
      BasicBlock::Create(context, "BB78", addObjectRectangleFunc);
  BasicBlock *BB79 =
      BasicBlock::Create(context, "BB79", addObjectRectangleFunc);
  BasicBlock *BB80 =
      BasicBlock::Create(context, "BB80", addObjectRectangleFunc);
  BasicBlock *BB81 =
      BasicBlock::Create(context, "BB81", addObjectRectangleFunc);
  BasicBlock *BB82 =
      BasicBlock::Create(context, "BB82", addObjectRectangleFunc);
  BasicBlock *BB83 =
      BasicBlock::Create(context, "BB83", addObjectRectangleFunc);
  BasicBlock *BB84 =
      BasicBlock::Create(context, "BB84", addObjectRectangleFunc);
  BasicBlock *BB85 =
      BasicBlock::Create(context, "BB85", addObjectRectangleFunc);
  BasicBlock *BB86 =
      BasicBlock::Create(context, "BB86", addObjectRectangleFunc);
  BasicBlock *BB87 =
      BasicBlock::Create(context, "BB87", addObjectRectangleFunc);
  BasicBlock *BB88 =
      BasicBlock::Create(context, "BB88", addObjectRectangleFunc);
  builder.SetInsertPoint(BB73);

  Value *val481 = builder.CreateAnd(val479, builder.getInt32(65535));
  Value *val482 = builder.CreateAdd(val481, builder.getInt32(4294967266));
  Value *val483 = builder.CreateCall(
      FunctionType::get(builder.getInt32Ty(),
                        {builder.getInt32Ty(), builder.getInt32Ty()}, false),
      module->getFunction("llvm.umin.i32"), {val481, builder.getInt32(353)});

  Value *val484 = builder.CreateAdd(val483, builder.getInt32(30));
  Value *val485 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_UGT, val482, val484);
  Value *val486 = builder.CreateCondBr(val485, BB86, BB74);

  builder.SetInsertPoint(BB74);

  Value *val487 = builder.CreateICmp(CmpInst::Predicate::ICMP_ULT, val479,
                                     builder.getInt32(48365568));
  Value *val488 = builder.CreateLShr(val479, builder.getInt32(16));
  Value *val489 = builder.CreateAdd(val488, builder.getInt32(30));
  Value *val490 = builder.CreateSelect(val487, val489, builder.getInt32(767));

  Value *val491 = builder.CreateAdd(val488, builder.getInt32(4294967266));
  Value *val492 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_UGT, val491, val490);
  Value *val493 = builder.CreateZExt(val491, builder.getInt64Ty());
  Value *val494 = builder.CreateAdd(val490, builder.getInt32(1));
  Value *val495 = builder.CreateZExt(val482, builder.getInt64Ty());
  Value *val496 = builder.CreateAdd(val483, builder.getInt32(31));
  Value *val497 = builder.CreateAdd(val490, builder.getInt32(30));
  Value *val498 = builder.CreateSub(val497, val488);
  Value *val499 = builder.CreateZExt(val498, builder.getInt64Ty());
  Value *val500 = builder.CreateAdd(val499, builder.getInt64(1));
  Value *val501 = builder.CreateICmp(CmpInst::Predicate::ICMP_ULT, val498,
                                     builder.getInt32(3));
  Value *val502 = builder.CreateICmp(CmpInst::Predicate::ICMP_ULT, val498,
                                     builder.getInt32(15));
  Value *val503 = builder.CreateAnd(val500, builder.getInt64(8589934576));
  Value *val504 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val500, val503);
  Value *val505 = builder.CreateAdd(val503, val493);
  Value *val506 = builder.CreateAnd(val500, builder.getInt64(12));
  Value *val507 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val506,
                                     builder.getInt64(0));
  Value *val508 = builder.CreateAnd(val500, builder.getInt64(8589934588));
  Value *val509 = builder.CreateAdd(val508, val493);
  Value *val510 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val500, val508);
  Value *val511 = builder.CreateBr(BB75);

  builder.SetInsertPoint(BB75);

  PHINode *val512 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val513 = builder.CreateCondBr(val492, BB87, BB76);

  builder.SetInsertPoint(BB76);

  Value *val514 = builder.CreateMul(val512, builder.getInt64(3072));
  Value *val515 = builder.CreateInBoundsGEP(builder.getInt8Ty(), val480,
                                            ArrayRef<Value *>{val514});

  Value *val516 = builder.CreateCondBr(val501, BB77, BB78);

  builder.SetInsertPoint(BB78);

  Value *val517 = builder.CreateCondBr(val502, BB83, BB79);

  builder.SetInsertPoint(BB79);

  Value *val518 = builder.CreateGEP(builder.getFloatTy(), val515,
                                    ArrayRef<Value *>{val493});

  Value *val519 = builder.CreateBr(BB80);

  builder.SetInsertPoint(BB80);

  PHINode *val520 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val521 = builder.CreateGEP(builder.getFloatTy(), val518,
                                    ArrayRef<Value *>{val520});

  Value *val522 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val521, ArrayRef<Value *>{builder.getInt64(16)});

  Value *val523 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val521, ArrayRef<Value *>{builder.getInt64(32)});

  Value *val524 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val521, ArrayRef<Value *>{builder.getInt64(48)});

  Value *val525 =
      builder.CreateLoad(FixedVectorType::get(builder.getFloatTy(), 4), val521);

  Value *val526 =
      builder.CreateLoad(FixedVectorType::get(builder.getFloatTy(), 4), val522);

  Value *val527 =
      builder.CreateLoad(FixedVectorType::get(builder.getFloatTy(), 4), val523);

  Value *val528 =
      builder.CreateLoad(FixedVectorType::get(builder.getFloatTy(), 4), val524);

  Value *val529 = builder.CreateFAdd(
      val525, ConstantDataVector::getSplat(
                  4, ConstantFP::get(builder.getFloatTy(), 21990236926976)));
  Value *val530 = builder.CreateFAdd(
      val526, ConstantDataVector::getSplat(
                  4, ConstantFP::get(builder.getFloatTy(), 21990236926976)));
  Value *val531 = builder.CreateFAdd(
      val527, ConstantDataVector::getSplat(
                  4, ConstantFP::get(builder.getFloatTy(), 21990236926976)));
  Value *val532 = builder.CreateFAdd(
      val528, ConstantDataVector::getSplat(
                  4, ConstantFP::get(builder.getFloatTy(), 21990236926976)));
  Value *val533 = builder.CreateStore(val529, val521);

  Value *val534 = builder.CreateStore(val530, val522);

  Value *val535 = builder.CreateStore(val531, val523);

  Value *val536 = builder.CreateStore(val532, val524);

  Value *val537 = builder.CreateAdd(val520, builder.getInt64(16));
  Value *val538 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val537, val503);
  Value *val539 = builder.CreateCondBr(val538, BB81, BB80);

  builder.SetInsertPoint(BB81);

  Value *val540 = builder.CreateCondBr(val504, BB87, BB82);

  builder.SetInsertPoint(BB82);

  Value *val541 = builder.CreateCondBr(val507, BB77, BB83);

  builder.SetInsertPoint(BB83);

  PHINode *val542 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val543 = builder.CreateGEP(builder.getFloatTy(), val515,
                                    ArrayRef<Value *>{val493});

  Value *val544 = builder.CreateBr(BB84);

  builder.SetInsertPoint(BB84);

  PHINode *val545 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val546 = builder.CreateGEP(builder.getFloatTy(), val543,
                                    ArrayRef<Value *>{val545});

  Value *val547 =
      builder.CreateLoad(FixedVectorType::get(builder.getFloatTy(), 4), val546);

  Value *val548 = builder.CreateFAdd(
      val547, ConstantDataVector::getSplat(
                  4, ConstantFP::get(builder.getFloatTy(), 21990236926976)));
  Value *val549 = builder.CreateStore(val548, val546);

  Value *val550 = builder.CreateAdd(val545, builder.getInt64(4));
  Value *val551 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val550, val508);
  Value *val552 = builder.CreateCondBr(val551, BB85, BB84);

  builder.SetInsertPoint(BB85);

  Value *val553 = builder.CreateCondBr(val510, BB87, BB77);

  builder.SetInsertPoint(BB77);

  PHINode *val554 = builder.CreatePHI(builder.getInt64Ty(), 3);
  Value *val555 = builder.CreateBr(BB88);

  builder.SetInsertPoint(BB88);

  PHINode *val556 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val557 = builder.CreateInBoundsGEP(builder.getFloatTy(), val515,
                                            ArrayRef<Value *>{val556});

  Value *val558 = builder.CreateLoad(builder.getFloatTy(), val557);

  Value *val559 = builder.CreateFAdd(
      val558, ConstantFP::get(builder.getFloatTy(), 90.000000));
  Value *val560 = builder.CreateStore(val559, val557);

  Value *val561 = builder.CreateAdd(val556, builder.getInt64(1));
  Value *val562 =
      builder.CreateTrunc(val561, builder.getInt32Ty(), "", false, false);

  Value *val563 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val494, val562);
  Value *val564 = builder.CreateCondBr(val563, BB87, BB88);

  builder.SetInsertPoint(BB87);

  Value *val565 = builder.CreateAdd(val512, builder.getInt64(1));
  Value *val566 =
      builder.CreateTrunc(val565, builder.getInt32Ty(), "", false, false);

  Value *val567 =
      builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val496, val566);
  Value *val568 = builder.CreateCondBr(val567, BB86, BB75);

  builder.SetInsertPoint(BB86);

  Value *val569 = builder.CreateRetVoid();
  val512->addIncoming(val495, BB74);
  val512->addIncoming(val565, BB87);
  val520->addIncoming(builder.getInt64(0), BB79);
  val520->addIncoming(val537, BB80);
  val542->addIncoming(val503, BB82);
  val542->addIncoming(builder.getInt64(0), BB78);
  val545->addIncoming(val542, BB83);
  val545->addIncoming(val550, BB84);
  val554->addIncoming(val493, BB76);
  val554->addIncoming(val505, BB82);
  val554->addIncoming(val509, BB85);
  val556->addIncoming(val561, BB88);
  val556->addIncoming(val554, BB77);
  outs() << (!(verifyFunction(*addObjectRectangleFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: addObjectRectangle\n\n");

  BasicBlock *BB89 = BasicBlock::Create(context, "BB89", appFunc);
  BasicBlock *BB90 = BasicBlock::Create(context, "BB90", appFunc);
  BasicBlock *BB91 = BasicBlock::Create(context, "BB91", appFunc);
  BasicBlock *BB92 = BasicBlock::Create(context, "BB92", appFunc);
  BasicBlock *BB93 = BasicBlock::Create(context, "BB93", appFunc);
  BasicBlock *BB94 = BasicBlock::Create(context, "BB94", appFunc);
  BasicBlock *BB95 = BasicBlock::Create(context, "BB95", appFunc);
  BasicBlock *BB96 = BasicBlock::Create(context, "BB96", appFunc);
  BasicBlock *BB97 = BasicBlock::Create(context, "BB97", appFunc);
  BasicBlock *BB98 = BasicBlock::Create(context, "BB98", appFunc);
  BasicBlock *BB99 = BasicBlock::Create(context, "BB99", appFunc);
  BasicBlock *BB100 = BasicBlock::Create(context, "BB100", appFunc);
  BasicBlock *BB101 = BasicBlock::Create(context, "BB101", appFunc);
  BasicBlock *BB102 = BasicBlock::Create(context, "BB102", appFunc);
  BasicBlock *BB103 = BasicBlock::Create(context, "BB103", appFunc);
  BasicBlock *BB104 = BasicBlock::Create(context, "BB104", appFunc);
  BasicBlock *BB105 = BasicBlock::Create(context, "BB105", appFunc);
  BasicBlock *BB106 = BasicBlock::Create(context, "BB106", appFunc);
  BasicBlock *BB107 = BasicBlock::Create(context, "BB107", appFunc);
  BasicBlock *BB108 = BasicBlock::Create(context, "BB108", appFunc);
  BasicBlock *BB109 = BasicBlock::Create(context, "BB109", appFunc);
  BasicBlock *BB110 = BasicBlock::Create(context, "BB110", appFunc);
  builder.SetInsertPoint(BB89);

  Value *val570 =
      builder.CreateAlloca(ArrayType::get(builder.getFloatTy(), 294912));

  Value *val571 =
      builder.CreateAlloca(ArrayType::get(builder.getFloatTy(), 294912));

  Value *val573 = builder.CreateCall(
      FunctionType::get(builder.getVoidTy(),
                        {builder.getPtrTy(), builder.getInt8Ty(),
                         builder.getInt64Ty(), builder.getInt1Ty()},
                        false),
      module->getFunction("llvm.memset.p0.i64"),
      {val570, builder.getInt8(0), builder.getInt64(1179648),
       builder.getInt1(0)});

  Value *val575 = builder.CreateCall(
      FunctionType::get(builder.getVoidTy(),
                        {builder.getPtrTy(), builder.getInt8Ty(),
                         builder.getInt64Ty(), builder.getInt1Ty()},
                        false),
      module->getFunction("llvm.memset.p0.i64"),
      {val571, builder.getInt8(0), builder.getInt64(1179648),
       builder.getInt1(0)});

  Value *val576 = builder.CreateBr(BB90);

  builder.SetInsertPoint(BB90);

  PHINode *val577 = builder.CreatePHI(builder.getPtrTy(), 2);
  PHINode *val578 = builder.CreatePHI(builder.getPtrTy(), 2);
  Value *val579 =
      builder.CreateCall(FunctionType::get(builder.getInt32Ty(), {}, false),
                         module->getFunction("simHasClick"), {});

  Value *val580 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val579,
                                     builder.getInt32(0));
  Value *val581 = builder.CreateCondBr(val580, BB92, BB91);

  builder.SetInsertPoint(BB91);

  Value *val582 =
      builder.CreateCall(FunctionType::get(builder.getInt32Ty(), {}, false),
                         module->getFunction("simGetClick"), {});

  Value *val583 = builder.CreateCall(
      FunctionType::get(builder.getVoidTy(),
                        {builder.getInt32Ty(), builder.getPtrTy()}, false),
      module->getFunction("addObjectCircle"), {val582, val578});

  Value *val584 =
      builder.CreateCall(FunctionType::get(builder.getInt32Ty(), {}, false),
                         module->getFunction("simHasClick"), {});

  Value *val585 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val584,
                                     builder.getInt32(0));
  Value *val586 = builder.CreateCondBr(val585, BB92, BB91);

  builder.SetInsertPoint(BB92);

  Value *val587 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val578, ArrayRef<Value *>{builder.getInt64(4)});

  Value *val588 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val578, ArrayRef<Value *>{builder.getInt64(3068)});

  Value *val589 = builder.CreateBr(BB93);

  builder.SetInsertPoint(BB93);

  PHINode *val590 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val591 = builder.CreateMul(val590, builder.getInt64(1536));
  Value *val592 = builder.CreateLShr(val591, builder.getInt64(1));
  Value *val593 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val590,
                                     builder.getInt64(0));
  Value *val594 = builder.CreateAdd(val591, builder.getInt64(4294965760));
  Value *val595 = builder.CreateLShr(val594, builder.getInt64(1));
  Value *val596 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val590,
                                     builder.getInt64(383));
  Value *val597 = builder.CreateAdd(val591, builder.getInt64(1536));
  Value *val598 = builder.CreateLShr(val597, builder.getInt64(1));
  Value *val599 = builder.CreateAnd(val595, builder.getInt64(2147483392));
  Value *val600 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val592});

  Value *val601 = builder.CreateLoad(builder.getFloatTy(), val600);

  Value *val602 = builder.CreateCondBr(val593, BB95, BB94);

  builder.SetInsertPoint(BB94);

  Value *val603 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val599});

  Value *val604 = builder.CreateLoad(builder.getFloatTy(), val603);

  Value *val605 = builder.CreateCondBr(val596, BB96, BB95);

  builder.SetInsertPoint(BB95);

  PHINode *val606 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val607 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val598});

  Value *val608 = builder.CreateLoad(builder.getFloatTy(), val607);

  Value *val609 = builder.CreateBr(BB96);

  builder.SetInsertPoint(BB96);

  PHINode *val610 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val611 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val612 = builder.CreateInBoundsGEP(builder.getFloatTy(), val587,
                                            ArrayRef<Value *>{val592});

  Value *val613 = builder.CreateLoad(builder.getFloatTy(), val612);

  Value *val614 = builder.CreateFPExt(val601, builder.getDoubleTy());
  Value *val615 = builder.CreateFAdd(val610, val611);
  Value *val616 = builder.CreateFAdd(val601, val615);
  Value *val617 = builder.CreateFAdd(val613, val616);
  Value *val618 = builder.CreateFPExt(val617, builder.getDoubleTy());
  Value *val619 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val614, ConstantFP::get(builder.getDoubleTy(), -4.000000), val618});

  Value *val620 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val619, ConstantFP::get(builder.getDoubleTy(), 0.250000), val614});

  Value *val621 = builder.CreateFPTrunc(val620, builder.getFloatTy());
  Value *val622 = builder.CreateMul(val590, builder.getInt64(3072));
  Value *val623 = builder.CreateInBoundsGEP(builder.getInt8Ty(), val577,
                                            ArrayRef<Value *>{val622});

  Value *val624 = builder.CreateStore(val621, val623);

  Value *val625 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val599});

  Value *val626 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val598});

  Value *val627 = builder.CreateBr(BB101);

  builder.SetInsertPoint(BB101);

  PHINode *val628 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val629 = builder.CreateAdd(val628, val592);
  Value *val630 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val629});

  Value *val631 = builder.CreateLoad(builder.getFloatTy(), val630);

  Value *val632 = builder.CreateCondBr(val593, BB103, BB102);

  builder.SetInsertPoint(BB102);

  Value *val633 = builder.CreateInBoundsGEP(builder.getFloatTy(), val625,
                                            ArrayRef<Value *>{val628});

  Value *val634 = builder.CreateLoad(builder.getFloatTy(), val633);

  Value *val635 = builder.CreateCondBr(val596, BB104, BB103);

  builder.SetInsertPoint(BB103);

  PHINode *val636 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val637 = builder.CreateInBoundsGEP(builder.getFloatTy(), val626,
                                            ArrayRef<Value *>{val628});

  Value *val638 = builder.CreateLoad(builder.getFloatTy(), val637);

  Value *val639 = builder.CreateBr(BB104);

  builder.SetInsertPoint(BB104);

  PHINode *val640 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val641 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val642 = builder.CreateAdd(val629, builder.getInt64(4294967295));
  Value *val643 = builder.CreateAnd(val642, builder.getInt64(4294967295));
  Value *val644 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val643});

  Value *val645 = builder.CreateLoad(builder.getFloatTy(), val644);

  Value *val646 = builder.CreateInBoundsGEP(builder.getFloatTy(), val587,
                                            ArrayRef<Value *>{val629});

  Value *val647 = builder.CreateLoad(builder.getFloatTy(), val646);

  Value *val648 = builder.CreateFPExt(val631, builder.getDoubleTy());
  Value *val649 = builder.CreateFAdd(val640, val641);
  Value *val650 = builder.CreateFAdd(val649, val645);
  Value *val651 = builder.CreateFAdd(val650, val647);
  Value *val652 = builder.CreateFPExt(val651, builder.getDoubleTy());
  Value *val653 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val648, ConstantFP::get(builder.getDoubleTy(), -4.000000), val652});

  Value *val654 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val653, ConstantFP::get(builder.getDoubleTy(), 0.250000), val648});

  Value *val655 = builder.CreateFPTrunc(val654, builder.getFloatTy());
  Value *val656 = builder.CreateInBoundsGEP(builder.getFloatTy(), val623,
                                            ArrayRef<Value *>{val628});

  Value *val657 = builder.CreateStore(val655, val656);

  Value *val658 = builder.CreateAdd(val628, builder.getInt64(1));
  Value *val659 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val658,
                                     builder.getInt64(767));
  Value *val660 = builder.CreateCondBr(val659, BB97, BB101);

  builder.SetInsertPoint(BB97);

  Value *val661 = builder.CreateInBoundsGEP(builder.getFloatTy(), val588,
                                            ArrayRef<Value *>{val592});

  Value *val662 = builder.CreateLoad(builder.getFloatTy(), val661);

  Value *val663 = builder.CreateCondBr(val593, BB99, BB98);

  builder.SetInsertPoint(BB98);

  Value *val664 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val625, ArrayRef<Value *>{builder.getInt64(3068)});

  Value *val665 = builder.CreateLoad(builder.getFloatTy(), val664);

  Value *val666 = builder.CreateCondBr(val596, BB100, BB99);

  builder.SetInsertPoint(BB99);

  PHINode *val667 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val668 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val626, ArrayRef<Value *>{builder.getInt64(3068)});

  Value *val669 = builder.CreateLoad(builder.getFloatTy(), val668);

  Value *val670 = builder.CreateBr(BB100);

  builder.SetInsertPoint(BB100);

  PHINode *val671 = builder.CreatePHI(builder.getFloatTy(), 2);
  PHINode *val672 = builder.CreatePHI(builder.getFloatTy(), 2);
  Value *val673 = builder.CreateAdd(val592, builder.getInt64(766));
  Value *val674 = builder.CreateAnd(val673, builder.getInt64(4294967294));
  Value *val675 = builder.CreateInBoundsGEP(builder.getFloatTy(), val578,
                                            ArrayRef<Value *>{val674});

  Value *val676 = builder.CreateLoad(builder.getFloatTy(), val675);

  Value *val677 = builder.CreateFPExt(val662, builder.getDoubleTy());
  Value *val678 = builder.CreateFAdd(val671, val672);
  Value *val679 = builder.CreateFAdd(val678, val676);
  Value *val680 = builder.CreateFAdd(val679, val662);
  Value *val681 = builder.CreateFPExt(val680, builder.getDoubleTy());
  Value *val682 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val677, ConstantFP::get(builder.getDoubleTy(), -4.000000), val681});

  Value *val683 = builder.CreateCall(
      FunctionType::get(
          builder.getDoubleTy(),
          {builder.getDoubleTy(), builder.getDoubleTy(), builder.getDoubleTy()},
          false),
      module->getFunction("llvm.fmuladd.f64"),
      {val682, ConstantFP::get(builder.getDoubleTy(), 0.250000), val677});

  Value *val684 = builder.CreateFPTrunc(val683, builder.getFloatTy());
  Value *val685 = builder.CreateInBoundsGEP(
      builder.getInt8Ty(), val623, ArrayRef<Value *>{builder.getInt64(3068)});

  Value *val686 = builder.CreateStore(val684, val685);

  Value *val687 = builder.CreateAdd(val590, builder.getInt64(1));
  Value *val688 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val687,
                                     builder.getInt64(384));
  Value *val689 = builder.CreateCondBr(val688, BB105, BB93);

  builder.SetInsertPoint(BB105);

  PHINode *val690 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val691 = builder.CreateMul(val690, builder.getInt64(3072));
  Value *val692 = builder.CreateInBoundsGEP(builder.getInt8Ty(), val577,
                                            ArrayRef<Value *>{val691});

  Value *val693 =
      builder.CreateTrunc(val690, builder.getInt32Ty(), "", true, true);

  Value *val694 = builder.CreateBr(BB107);

  builder.SetInsertPoint(BB107);

  PHINode *val695 = builder.CreatePHI(builder.getInt64Ty(), 2);
  Value *val696 = builder.CreateInBoundsGEP(builder.getFloatTy(), val692,
                                            ArrayRef<Value *>{val695});

  Value *val697 = builder.CreateLoad(builder.getFloatTy(), val696);

  Value *val698 =
      builder.CreateFCmp(CmpInst::Predicate::FCMP_ULT, val697,
                         ConstantFP::get(builder.getFloatTy(), 100.000000));
  Value *val699 = builder.CreateCondBr(val698, BB108, BB109);

  builder.SetInsertPoint(BB108);

  Value *val700 = builder.CreateFPExt(val697, builder.getDoubleTy());
  Value *val701 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.minnum.f64"),
      {val700, ConstantFP::get(builder.getDoubleTy(), 100.000000)});

  Value *val702 = builder.CreateCall(
      FunctionType::get(builder.getDoubleTy(),
                        {builder.getDoubleTy(), builder.getDoubleTy()}, false),
      module->getFunction("llvm.maxnum.f64"),
      {val701, ConstantFP::get(builder.getDoubleTy(), 0.000000)});

  Value *val703 = builder.CreateFDiv(
      val702, ConstantFP::get(builder.getDoubleTy(), 10.000000));
  Value *val704 = builder.CreateFPTrunc(val703, builder.getFloatTy());
  Value *val705 = builder.CreateFPToUI(val704, builder.getInt32Ty());
  Value *val706 = builder.CreateZExt(val705, builder.getInt64Ty());
  Value *val707 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val706});

  Value *val708 = builder.CreateLoad(builder.getInt32Ty(), val707);

  Value *val709 = builder.CreateAdd(val705, builder.getInt32(1));
  Value *val710 = builder.CreateZExt(val709, builder.getInt64Ty());
  Value *val711 =
      builder.CreateInBoundsGEP(ArrayType::get(builder.getInt32Ty(), 10),
                                module->getGlobalVariable("colors"),
                                ArrayRef<Value *>{builder.getInt64(0), val710});

  Value *val712 = builder.CreateLoad(builder.getInt32Ty(), val711);

  Value *val713 = builder.CreateUIToFP(val705, builder.getFloatTy());
  Value *val714 = builder.CreateFSub(val704, val713);
  Value *val715 = builder.CreateCall(
      FunctionType::get(
          builder.getInt32Ty(),
          {builder.getInt32Ty(), builder.getInt32Ty(), builder.getFloatTy()},
          false),
      module->getFunction("interpolate"), {val708, val712, val714});

  Value *val716 = builder.CreateBr(BB109);

  builder.SetInsertPoint(BB109);

  PHINode *val717 = builder.CreatePHI(builder.getInt32Ty(), 2);
  Value *val718 =
      builder.CreateTrunc(val695, builder.getInt32Ty(), "", true, true);

  Value *val719 = builder.CreateCall(
      FunctionType::get(
          builder.getVoidTy(),
          {builder.getInt32Ty(), builder.getInt32Ty(), builder.getInt32Ty()},
          false),
      module->getFunction("simPutPixel"), {val718, val693, val717});

  Value *val720 = builder.CreateAdd(val695, builder.getInt64(1));
  Value *val721 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val720,
                                     builder.getInt64(768));
  Value *val722 = builder.CreateCondBr(val721, BB106, BB107);

  builder.SetInsertPoint(BB106);

  Value *val723 = builder.CreateAdd(val690, builder.getInt64(1));
  Value *val724 = builder.CreateICmp(CmpInst::Predicate::ICMP_EQ, val723,
                                     builder.getInt64(384));
  Value *val725 = builder.CreateCondBr(val724, BB110, BB105);

  builder.SetInsertPoint(BB110);

  Value *val726 =
      builder.CreateCall(FunctionType::get(builder.getVoidTy(), {}, false),
                         module->getFunction("simFlush"), {});

  Value *val727 = builder.CreateBr(BB90);

  val577->addIncoming(val571, BB89);
  val577->addIncoming(val578, BB110);
  val578->addIncoming(val570, BB89);
  val578->addIncoming(val577, BB110);
  val590->addIncoming(builder.getInt64(0), BB92);
  val590->addIncoming(val687, BB100);
  val606->addIncoming(val604, BB94);
  val606->addIncoming(val601, BB93);
  val610->addIncoming(val606, BB95);
  val610->addIncoming(val604, BB94);
  val611->addIncoming(val608, BB95);
  val611->addIncoming(val601, BB94);
  val628->addIncoming(builder.getInt64(1), BB96);
  val628->addIncoming(val658, BB104);
  val636->addIncoming(val634, BB102);
  val636->addIncoming(val631, BB101);
  val640->addIncoming(val636, BB103);
  val640->addIncoming(val634, BB102);
  val641->addIncoming(val638, BB103);
  val641->addIncoming(val631, BB102);
  val667->addIncoming(val665, BB98);
  val667->addIncoming(val662, BB97);
  val671->addIncoming(val667, BB99);
  val671->addIncoming(val665, BB98);
  val672->addIncoming(val669, BB99);
  val672->addIncoming(val662, BB98);
  val690->addIncoming(val723, BB106);
  val690->addIncoming(builder.getInt64(0), BB100);
  val695->addIncoming(builder.getInt64(0), BB105);
  val695->addIncoming(val720, BB109);
  val717->addIncoming(val715, BB108);
  val717->addIncoming(builder.getInt32(15955986), BB107);
  outs() << (!(verifyFunction(*appFunc, &outs()))
                 ? ""
                 : "VERIFICATION FAIL: app\n\n");

  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
  ee->InstallLazyFunctionCreator([&](const std::string &name) -> void * {
    if (name == "_simHasClick")
      return reinterpret_cast<void *>(simHasClick);
    else if (name == "_simGetClick")
      return reinterpret_cast<void *>(simGetClick);
    if (name == "_simFlush")
      return reinterpret_cast<void *>(simFlush);
    if (name == "_simPutPixel")
      return reinterpret_cast<void *>(simPutPixel);
    llvm::outs() << "ERROR: Unknown symbol: " << name << "\n";
    return nullptr;
  });
  ee->finalizeObject();
  simInit();
  ArrayRef<GenericValue> noargs;
  GenericValue v = ee->runFunction(appFunc, noargs);
  simExit();
  return 0;
}