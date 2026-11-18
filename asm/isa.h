#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

using std::int64_t;
using std::size_t;
using std::string;
using std::string_view;
using std::uint64_t;
using std::uint8_t;

enum class OpcodeType : uint8_t {
  // Interger operations:
  OpAdd,     // add <REG dst> <REG lhs> <REG rhs>
  OpAddI,    // addi <REG dst> <REG lhs> <IMM rhs>
  OpMul,     // mul <REG dst> <REG lhs> <REG rhs>
  OpMulI,    // muli <REG dst> <REG lhs> <IMM rhs>
  OpSub,     // sub <REG dst> <REG lhs> <REG rhs>
  OpUiToFp,  // uitofp <REG dst> <REG src>

  // (lhs == rhs)
  OpIEqI,  // ieqi <REG dst> <REG lhs> <IMM rhs>

  // unsigned integer less-than: dst = (lhs < rhs)
  OpILt,   // ilt <REG dst> <REG lhs> <REG rhs>
  OpILtI,  // ilt <REG dst> <REG lhs> <IMM rhs>

  // logical shift left by immediate: dst = lhs << rhs
  // shli <REG dst> <REG lhs> <IMM rhs>
  OpShlI,

  // logical shift right by immediate: dst = lhs >> rhs
  // lshri <REG dst> <REG lhs> <IMM rhs>
  OpLShrI,

  // bitwise AND with immediate: dst = lhs & rhs
  // andi <REG dst> <REG lhs> <IMM rhs>
  OpAndI,

  // bitwise OR: dst = lhs | rhs
  // or <REG dst> <REG lhs> <REG rhs>
  OpOr,

  // FP operations:
  OpFAdd,   // fadd <REG dst> <REG lhs> <REG rhs>
  OpFAddI,  // faddi <REG dst> <REG lhs> <IMM rhs>
  OpFSub,   // fsub <REG dst> <REG lhs> <REG rhs>
  OpFMul,   // fmul <REG dst> <REG lhs> <REG rhs>
  OpFMulI,  // fmuli <REG dst> <REG lhs> <IMM rhs>
  OpFDivI,  // fdivi <REG dst> <REG lhs> <IMM rhs>

  // converts floating-point value to unsigned integer
  // fptoui <REG dst> <REG src>
  OpFpToUi,

  // (lhs > rhs)
  // fgti <REG dst> <REG lhs> <IMM rhs>
  OpFGtI,

  // (lhs < rhs)
  // flti <REG dst> <REG lhs> <IMM rhs>
  OpFLtI,

  OpJmp,  // jmp <BLOCK>

  // jumps to b1 if condition is true else to b2
  // cjmp <REG cond> <BLOCK b1> <BLOCK b2>
  OpCJmp,

  // call internal function (fn) with return value
  // callv <REG dst> <FN>
  OpCallV,

  // call external function (ext) without return value
  // calle <EXT>
  OpCallE,

  // call external function (ext) with return value
  // callev <REG dst> <EXT>
  OpCallEV,

  // return from function with value
  // retv <REG value>
  OpRetV,

  // allocates size bytes on stack and stores base pointer into dst
  // alloc <REG dst> <IMM size>
  OpAlloc,

  // loads 64-bit value from memory address in ptr into dst
  // load <REG dst> <REG ptr>
  OpLoad,

  // stores 64-bit value from val into memory address in ptr
  // store <REG val> <REG ptr>
  OpStore,

  OpMov,   // mov <REG dst> <REG src>
  OpMovI,  // mov <REG dst> <IMM src>

  // byte-offset pointer arithmetic: dst = base + offset
  // gep <REG dst> <REG base> <REG offset>
  OpGep,

  // 8-byte stride pointer arithmetic: dst = base + index * 8
  // gep8 <REG dst> <REG base> <REG index>
  OpGep8,

  // immediate byte-offset pointer arithmetic: dst = base + offset
  // gepi <REG dst> <REG base> <IMM offset>
  OpGepI,

  // global symbol 8-byte element address: dst = &glo + index * 8
  // gepg <REG dst> <GLO glo> <REG index>
  OpGepG,

  // converts 8-bit sRGB channel [0..255] to linear [0.0..1.0]
  // rgb2lin <REG dst> <REG src>
  OpRgb2Lin,
  // converts linear channel [0.0..1.0] to sRGB [0.0..1.0]
  // lin2rgb <REG dst> <REG src>
  OpLin2Rgb,

  // prepares arguments before function call.
  OpPrepare3,  // prep3 <REG arg1> <REG arg2> <REG arg3>

  Block,  // basic block label
  Func,   // function begin label
  Main,   // program entry function label

  Dummy,  // needed for 'removing' opcodes from code

  Error  // generation error indicator
};

struct Opcode {
  string name;
  OpcodeType type;

  Opcode(OpcodeType type, string name) : name(name), type(type) {}
  virtual ~Opcode() = default;

  virtual string toString() const = 0;

 protected:
  string fmt0() const { return std::format("  {}", name); }

  string fmt1(string_view op1) const {
    return std::format("\t{:<8} {}", name, op1);
  }

  string fmt2(string_view op1, string_view op2) const {
    return std::format("\t{:<8} {:<6} {}", name, op1, op2);
  }

  string fmt3(string_view op1, string_view op2, string_view op3) const {
    return std::format("\t{:<8} {:<6} {:<6} {}", name, op1, op2, op3);
  }
};

struct Add : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  Add(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpAdd, "add"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct AddI : Opcode {
  size_t dst;
  size_t lhs;
  int64_t rhs;

  AddI(size_t dst, size_t lhs, int64_t rhs)
      : Opcode(OpcodeType::OpAddI, "addi"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct Mul : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  Mul(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpMul, "mul"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct MulI : Opcode {
  size_t dst;
  size_t lhs;
  int64_t rhs;

  MulI(size_t dst, size_t lhs, int64_t rhs)
      : Opcode(OpcodeType::OpMulI, "muli"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct Sub : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  Sub(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpSub, "sub"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct UiToFp : Opcode {
  size_t dst;
  size_t src;

  UiToFp(size_t dst, size_t src)
      : Opcode(OpcodeType::OpUiToFp, "uitofp"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", src));
  }
};

struct FpToUi : Opcode {
  size_t dst;
  size_t src;

  FpToUi(size_t dst, size_t src)
      : Opcode(OpcodeType::OpFpToUi, "fptoui"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", src));
  }
};

struct IEqI : Opcode {
  size_t dst;
  size_t lhs;
  int64_t rhs;

  IEqI(size_t dst, size_t lhs, int64_t rhs)
      : Opcode(OpcodeType::OpIEqI, "ieqi"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct ILt : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  ILt(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpILt, "ilt"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct ILtI : Opcode {
  size_t dst;
  size_t lhs;
  uint64_t rhs;

  ILtI(size_t dst, size_t lhs, uint64_t rhs)
      : Opcode(OpcodeType::OpILtI, "ilti"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct ShlI : Opcode {
  size_t dst;
  size_t lhs;
  uint64_t rhs;

  ShlI(size_t dst, size_t lhs, uint64_t rhs)
      : Opcode(OpcodeType::OpShlI, "shli"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct LShrI : Opcode {
  size_t dst;
  size_t lhs;
  uint64_t rhs;

  LShrI(size_t dst, size_t lhs, uint64_t rhs)
      : Opcode(OpcodeType::OpLShrI, "lshri"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct AndI : Opcode {
  size_t dst;
  size_t lhs;
  uint64_t rhs;

  AndI(size_t dst, size_t lhs, uint64_t rhs)
      : Opcode(OpcodeType::OpAndI, "andi"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct Or : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  Or(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpOr, "or"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct FAdd : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  FAdd(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpFAdd, "fadd"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct FAddI : Opcode {
  size_t dst;
  size_t lhs;
  double rhs;

  FAddI(size_t dst, size_t lhs, double rhs)
      : Opcode(OpcodeType::OpFAddI, "faddi"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct FSub : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  FSub(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpFSub, "fsub"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct FMul : Opcode {
  size_t dst;
  size_t lhs;
  size_t rhs;

  FMul(size_t dst, size_t lhs, size_t rhs)
      : Opcode(OpcodeType::OpFMul, "fmul"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("r{}", rhs));
  }
};

struct FMulI : Opcode {
  size_t dst;
  size_t lhs;
  double rhs;

  FMulI(size_t dst, size_t lhs, double rhs)
      : Opcode(OpcodeType::OpFMulI, "fmuli"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct FDivI : Opcode {
  size_t dst;
  size_t lhs;
  double rhs;

  FDivI(size_t dst, size_t lhs, double rhs)
      : Opcode(OpcodeType::OpFDivI, "fdivi"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct FGtI : Opcode {
  size_t dst;
  size_t lhs;
  double rhs;

  FGtI(size_t dst, size_t lhs, double rhs)
      : Opcode(OpcodeType::OpFGtI, "fgti"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct FLtI : Opcode {
  size_t dst;
  size_t lhs;
  double rhs;

  FLtI(size_t dst, size_t lhs, double rhs)
      : Opcode(OpcodeType::OpFLtI, "flti"), dst(dst), lhs(lhs), rhs(rhs) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", lhs),
                std::format("{}", rhs));
  }
};

struct Jmp : Opcode {
  size_t target;

  explicit Jmp(size_t target)
      : Opcode(OpcodeType::OpJmp, "jmp"), target(target) {}

  string toString() const override { return fmt1(std::format("b{}", target)); }
};

struct CJmp : Opcode {
  size_t cond;
  size_t trueTarget;
  size_t falseTarget;

  CJmp(size_t cond, size_t trueTarget, size_t falseTarget)
      : Opcode(OpcodeType::OpCJmp, "cjmp"),
        cond(cond),
        trueTarget(trueTarget),
        falseTarget(falseTarget) {}

  string toString() const override {
    return fmt3(std::format("r{}", cond), std::format("b{}", trueTarget),
                std::format("b{}", falseTarget));
  }
};

struct CallV : Opcode {
  size_t dst;
  size_t callee;

  CallV(size_t dst, size_t callee)
      : Opcode(OpcodeType::OpCallV, "callv"), dst(dst), callee(callee) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("fn{}", callee));
  }
};

struct CallE : Opcode {
  size_t callee;

  CallE(size_t callee) : Opcode(OpcodeType::OpCallE, "calle"), callee(callee) {}

  string toString() const override {
    return fmt1(std::format("ext{}", callee));
  }
};

struct CallEV : Opcode {
  size_t dst;
  size_t callee;

  CallEV(size_t dst, size_t callee)
      : Opcode(OpcodeType::OpCallEV, "callev"), dst(dst), callee(callee) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("ext{}", callee));
  }
};

struct RetV : Opcode {
  size_t val;

  explicit RetV(size_t val) : Opcode(OpcodeType::OpRetV, "retv"), val(val) {}

  string toString() const override { return fmt1(std::format("r{}", val)); }
};

struct Load : Opcode {
  size_t dst;
  size_t ptr;

  Load(size_t dst, size_t ptr)
      : Opcode(OpcodeType::OpLoad, "load"), dst(dst), ptr(ptr) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", ptr));
  }
};

struct Store : Opcode {
  size_t val;
  size_t ptr;

  Store(size_t val, size_t ptr)
      : Opcode(OpcodeType::OpStore, "store"), val(val), ptr(ptr) {}

  string toString() const override {
    return fmt2(std::format("r{}", val), std::format("r{}", ptr));
  }
};

struct Alloc : Opcode {
  size_t dst;
  uint64_t size;

  Alloc(size_t dst, uint64_t size)
      : Opcode(OpcodeType::OpAlloc, "alloc"), dst(dst), size(size) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("{}", size));
  }
};

struct Mov : Opcode {
  size_t dst;
  size_t src;

  Mov(size_t dst, size_t src)
      : Opcode(OpcodeType::OpMov, "mov"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", src));
  }
};

struct MovI : Opcode {
  size_t dst;
  int64_t src;

  MovI(size_t dst, int64_t src)
      : Opcode(OpcodeType::OpMovI, "movi"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("{}", src));
  }
};

struct Block : Opcode {
  size_t id;
  explicit Block(size_t id) : Opcode(OpcodeType::Block, "block"), id(id) {}

  string toString() const override { return std::format("b{}:", id); }
};

struct Func : Opcode {
  size_t id;
  explicit Func(size_t id) : Opcode(OpcodeType::Func, "function"), id(id) {}

  string toString() const override { return std::format("fn{}:", id); }
};

struct Prepare3 : Opcode {
  size_t arg1;
  size_t arg2;
  size_t arg3;

  Prepare3(size_t arg1, size_t arg2, size_t arg3)
      : Opcode(OpcodeType::OpPrepare3, "prep3"),
        arg1(arg1),
        arg2(arg2),
        arg3(arg3) {}

  string toString() const override {
    return fmt3(std::format("r{}", arg1), std::format("r{}", arg2),
                std::format("r{}", arg3));
  }
};

struct Main : Opcode {
  explicit Main() : Opcode(OpcodeType::Main, "main") {}

  string toString() const override { return std::format("main:"); }
};

struct Err : Opcode {
  string msg;
  explicit Err(string msg) : Opcode(OpcodeType::Error, "error"), msg(msg) {}

  string toString() const override { return "  " + msg; }
};

struct Dummy : Opcode {
  explicit Dummy() : Opcode(OpcodeType::Dummy, "dummy") {}
  string toString() const override { return ""; }
};

struct Rgb2Lin : Opcode {
  size_t dst;
  size_t src;

  Rgb2Lin(size_t dst, size_t src)
      : Opcode(OpcodeType::OpRgb2Lin, "rgb2lin"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", src));
  }
};

struct Lin2Rgb : Opcode {
  size_t dst;
  size_t src;

  Lin2Rgb(size_t dst, size_t src)
      : Opcode(OpcodeType::OpLin2Rgb, "lin2rgb"), dst(dst), src(src) {}

  string toString() const override {
    return fmt2(std::format("r{}", dst), std::format("r{}", src));
  }
};

struct Gep : Opcode {
  size_t dst;
  size_t ptr;
  size_t idx;

  Gep(size_t dst, size_t ptr, size_t idx)
      : Opcode(OpcodeType::OpGep, "gep"), dst(dst), ptr(ptr), idx(idx) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", ptr),
                std::format("r{}", idx));
  }
};

struct Gep8 : Opcode {
  size_t dst;
  size_t ptr;
  size_t idx;

  Gep8(size_t dst, size_t ptr, size_t idx)
      : Opcode(OpcodeType::OpGep8, "gep8"), dst(dst), ptr(ptr), idx(idx) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", ptr),
                std::format("r{}", idx));
  }
};

struct GepI : Opcode {
  size_t dst;
  size_t ptr;
  int64_t offset;

  GepI(size_t dst, size_t ptr, int64_t offset)
      : Opcode(OpcodeType::OpGepI, "gepi"),
        dst(dst),
        ptr(ptr),
        offset(offset) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("r{}", ptr),
                std::format("{}", offset));
  }
};

struct GepG : Opcode {
  size_t dst;
  size_t globalId;
  size_t idx;

  GepG(size_t dst, size_t globalId, size_t idx)
      : Opcode(OpcodeType::OpGepG, "gepg"),
        dst(dst),
        globalId(globalId),
        idx(idx) {}

  string toString() const override {
    return fmt3(std::format("r{}", dst), std::format("g{}", globalId),
                std::format("r{}", idx));
  }
};
