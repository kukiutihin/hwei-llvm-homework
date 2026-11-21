#pragma once

#include <cstddef>
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ast.h"
#include "error.h"

enum class Derived : std::size_t {
  Int,
  Uint,
  Float,
  Bool,
  Skib,
  FloatArray,
  UintArray,
  IntArray,
  BoolArray,
  AnyArray,
  Error,
};

inline std::string_view derivedToString(Derived derived) {
  switch (derived) {
    case Derived::Int:
      return "Int";
    case Derived::Uint:
      return "Uint";
    case Derived::Float:
      return "Float";
    case Derived::Bool:
      return "Bool";
    case Derived::Skib:
      return "()";
    case Derived::FloatArray:
      return "Array[Float]";
    case Derived::UintArray:
      return "Array[Uint]";
    case Derived::IntArray:
      return "Array[Int]";
    case Derived::BoolArray:
      return "Array[Bool]";
    case Derived::AnyArray:
      return "Array[?]";
    case Derived::Error:
      return "<error>";
  }
  return "Unknown";
}

class TypeChecker {
  struct FuncSig {
    std::vector<Derived> params;
    Derived ret;
  };

  std::vector<std::unordered_map<std::string, Derived>> varScopes;
  std::vector<std::unordered_map<std::string, FuncSig>> funcScopes;
  std::vector<SkibError> errors;

  template <typename Variant>
  auto visit(const Variant& v) {
    return std::visit([this](const auto& node) { return (*this)(node); }, v);
  }

  void pushScope() {
    varScopes.emplace_back();
    funcScopes.emplace_back();
  }

  void popScope() {
    varScopes.pop_back();
    funcScopes.pop_back();
  }

  std::optional<Derived> lookupVar(const std::string& name) const {
    for (auto it = varScopes.rbegin(); it != varScopes.rend(); ++it) {
      if (auto found = it->find(name); found != it->end()) {
        return found->second;
      }
    }
    return std::nullopt;
  }

  std::optional<FuncSig> lookupFunc(const std::string& name) const {
    for (auto it = funcScopes.rbegin(); it != funcScopes.rend(); ++it) {
      if (auto found = it->find(name); found != it->end()) {
        return found->second;
      }
    }
    return std::nullopt;
  }

  void addError(Position pos, std::string msg) {
    errors.push_back(SkibError{pos.line, pos.column, std::move(msg)});
  }

  void expectedButGot(Derived expected, Derived got, Position pos) {
    if (expected == Derived::Error || got == Derived::Error) return;
    addError(pos, std::format("Type mismatch: expected {}, but got {}",
                              derivedToString(expected), derivedToString(got)));
  }

  static bool isNumeric(Derived t) {
    return t == Derived::Int || t == Derived::Uint || t == Derived::Float;
  }

  static bool isIntegral(Derived t) {
    return t == Derived::Int || t == Derived::Uint;
  }

  static bool isArray(Derived t) {
    return t == Derived::IntArray || t == Derived::UintArray ||
           t == Derived::FloatArray || t == Derived::BoolArray;
  }

  // Implicit coercion chain: Uint -> Int -> Float, and AnyArray -> Array[T]
  static bool canImplicitCast(Derived from, Derived to) {
    if (from == to || from == Derived::Error || to == Derived::Error) {
      return true;
    }
    if (from == Derived::AnyArray && isArray(to)) {
      return true;
    }
    if (from == Derived::Uint && (to == Derived::Int || to == Derived::Float)) {
      return true;
    }
    if (from == Derived::Int && to == Derived::Float) {
      return true;
    }
    return false;
  }

  static std::optional<Derived> commonType(Derived a, Derived b) {
    if (a == Derived::Error || b == Derived::Error) return Derived::Error;
    if (canImplicitCast(a, b)) return b;
    if (canImplicitCast(b, a)) return a;
    return std::nullopt;
  }

  static std::optional<Derived> arrayElemType(Derived arr) {
    switch (arr) {
      case Derived::IntArray:
        return Derived::Int;
      case Derived::UintArray:
        return Derived::Uint;
      case Derived::FloatArray:
        return Derived::Float;
      case Derived::BoolArray:
        return Derived::Bool;
      default:
        return std::nullopt;
    }
  }

  static std::optional<Derived> scalarToArray(Derived scalar) {
    switch (scalar) {
      case Derived::Int:
        return Derived::IntArray;
      case Derived::Uint:
        return Derived::UintArray;
      case Derived::Float:
        return Derived::FloatArray;
      case Derived::Bool:
        return Derived::BoolArray;
      default:
        return std::nullopt;
    }
  }

  FuncSig buildFuncSig(const FuncDecl& fn) {
    FuncSig sig;
    sig.params.reserve(fn.paramList.size());
    for (const auto& param : fn.paramList) {
      sig.params.push_back(visit(param.type));
    }
    sig.ret = visit(fn.type);
    return sig;
  }

  Derived checkIntegralBinary(Derived lhs, Derived rhs, std::string_view opName,
                              Position pos) {
    if (lhs == Derived::Error || rhs == Derived::Error) return Derived::Error;
    if (!isIntegral(lhs)) {
      addError(pos, std::format("{} requires Int or Uint, got {}", opName,
                                derivedToString(lhs)));
      return Derived::Error;
    }
    if (!isIntegral(rhs)) {
      addError(pos, std::format("{} requires Int or Uint, got {}", opName,
                                derivedToString(rhs)));
      return Derived::Error;
    }
    return *commonType(lhs, rhs);
  }

  void registerBuiltins() {
    auto& globals = funcScopes.back();
    globals["Std.simInit"] = FuncSig{{}, Derived::Skib};
    globals["Std.simExit"] = FuncSig{{}, Derived::Skib};
    globals["Std.simHasClick"] = FuncSig{{}, Derived::Bool};
    globals["Std.simGetClick"] = FuncSig{{}, Derived::Uint};
    globals["Std.simPutPixel"] =
        FuncSig{{Derived::Uint, Derived::Uint, Derived::Uint}, Derived::Skib};
    globals["Std.simFlush"] = FuncSig{{}, Derived::Skib};
    globals["Std.simRand"] = FuncSig{{}, Derived::Uint};
    globals["Std.fmax"] =
        FuncSig{{Derived::Float, Derived::Float}, Derived::Float};
    globals["Std.fmin"] =
        FuncSig{{Derived::Float, Derived::Float}, Derived::Float};
    globals["Std.fabs"] = FuncSig{{Derived::Float}, Derived::Float};
    globals["Std.sqrt"] = FuncSig{{Derived::Float}, Derived::Float};
    globals["Std.rgb2linear"] = FuncSig{{Derived::Uint}, Derived::Float};
    globals["Std.linear2rgb"] = FuncSig{{Derived::Float}, Derived::Uint};
    globals["Std.float2uint"] = FuncSig{{Derived::Float}, Derived::Uint};
  }

 public:
  std::expected<void, std::vector<SkibError>> check(const Program& program) {
    varScopes.clear();
    funcScopes.clear();
    errors.clear();
    pushScope();
    registerBuiltins();

    for (const auto& topDecl : program) {
      if (const auto* fn = std::get_if<FuncDecl>(&topDecl)) {
        funcScopes.back()[fn->ident.ident] = buildFuncSig(*fn);
      }
    }

    for (const auto& topDecl : program) {
      visit(topDecl);
    }

    popScope();
    if (!errors.empty()) return std::unexpected(std::move(errors));
    return {};
  }

  Derived operator()(const Int&) { return Derived::Int; }
  Derived operator()(const Uint&) { return Derived::Uint; }
  Derived operator()(const Float&) { return Derived::Float; }
  Derived operator()(const Bool&) { return Derived::Bool; }
  Derived operator()(const Skib&) { return Derived::Skib; }
  Derived operator()(const ScalarType& st) { return visit(st); }

  Derived operator()(const ArrayType& arr) {
    return scalarToArray(visit(arr.generic)).value_or(Derived::Error);
  }

  void operator()(const ValDecl& valDecl) {
    Derived exprType = (*this)(valDecl.expr);
    if (valDecl.type.has_value()) {
      Derived expected = visit(*valDecl.type);
      if (!canImplicitCast(exprType, expected)) {
        expectedButGot(expected, exprType, valDecl.expr.pos);
      }
      varScopes.back()[valDecl.ident.ident] = expected;
    } else {
      if (exprType == Derived::AnyArray) {
        addError(
            valDecl.expr.pos,
            "Cannot infer array element type; add an explicit type annotation");
        varScopes.back()[valDecl.ident.ident] = Derived::Error;
      } else {
        varScopes.back()[valDecl.ident.ident] = exprType;
      }
    }
  }

  void operator()(const FuncDecl& fn) {
    Derived expectedRet = visit(fn.type);
    pushScope();
    for (const auto& param : fn.paramList) {
      varScopes.back()[param.ident.ident] = visit(param.type);
    }
    Derived bodyType = (*this)(*fn.expr);
    popScope();

    if (!canImplicitCast(bodyType, expectedRet)) {
      expectedButGot(expectedRet, bodyType, fn.pos);
    }
  }

  Derived operator()(const Expr& expr) {
    if (expr.binds.empty()) return Derived::Skib;
    for (std::size_t i = 0; i + 1 < expr.binds.size(); ++i) {
      Derived t = visit(expr.binds[i]);
      if (t != Derived::Skib && t != Derived::Error) {
        addError({0, 0},
                 std::format("Sequenced expression must have type (), got {}",
                             derivedToString(t)));
      }
    }
    return visit(expr.binds.back());
  }

  Derived operator()(const FuncDeclExpr& fnExpr) {
    pushScope();
    funcScopes.back()[fnExpr.decl.ident.ident] = buildFuncSig(fnExpr.decl);
    (*this)(fnExpr.decl);
    Derived res = fnExpr.next ? (*this)(*fnExpr.next) : Derived::Skib;
    popScope();
    return res;
  }

  Derived operator()(const Assignment& assign) {
    Derived exprType = (*this)(assign.expr);
    Derived boundType = exprType;
    if (assign.type.has_value()) {
      Derived expected = visit(*assign.type);
      if (!canImplicitCast(exprType, expected)) {
        expectedButGot(expected, exprType, assign.expr.pos);
      }
      boundType = expected;
    } else if (exprType == Derived::AnyArray) {
      addError(
          assign.expr.pos,
          "Cannot infer array element type; add an explicit type annotation");
      boundType = Derived::Error;
    }

    pushScope();
    varScopes.back()[assign.ident.ident] = boundType;
    Derived res = assign.next ? (*this)(*assign.next) : Derived::Skib;
    popScope();
    return res;
  }

  Derived operator()(const MutExpr& mut) {
    Derived elemType = (*this)(mut.subscript);
    Derived valType = (*this)(mut.value);
    if (!canImplicitCast(valType, elemType)) {
      expectedButGot(elemType, valType, mut.value.pos);
    }
    return Derived::Skib;
  }

  Derived operator()(const ValueExpr& val) {
    return std::visit(
        [this, &val](const auto& inner) { return checkValue(inner, val.pos); },
        val.expr);
  }

  Derived checkValue(const IfExpr& ifExpr, Position pos) {
    Derived condT = (*this)(*ifExpr.cond);
    if (condT != Derived::Bool) {
      expectedButGot(Derived::Bool, condT, pos);
    }
    Derived thenT = (*this)(*ifExpr.then);
    Derived elsT = (*this)(*ifExpr.els);
    auto common = commonType(thenT, elsT);
    if (!common) {
      expectedButGot(thenT, elsT, pos);
      return Derived::Error;
    }
    return *common;
  }

  Derived checkValue(const BitOrExpr& bitOr, Position pos) {
    Derived lhs = checkBitXor(bitOr.left, pos);
    for (const auto& rhsNode : bitOr.right) {
      Derived rhs = checkBitXor(rhsNode, pos);
      lhs = checkIntegralBinary(lhs, rhs, "Bitwise '|'", pos);
      if (lhs == Derived::Error) return Derived::Error;
    }
    return lhs;
  }

  Derived checkBitXor(const BitXorExpr& bitXor, Position pos) {
    Derived lhs = checkBitAnd(bitXor.left, pos);
    for (const auto& rhsNode : bitXor.right) {
      Derived rhs = checkBitAnd(rhsNode, pos);
      lhs = checkIntegralBinary(lhs, rhs, "Bitwise '^'", pos);
      if (lhs == Derived::Error) return Derived::Error;
    }
    return lhs;
  }

  Derived checkBitAnd(const BitAndExpr& bitAnd, Position pos) {
    Derived lhs = checkEq(bitAnd.left, pos);
    for (const auto& rhsNode : bitAnd.right) {
      Derived rhs = checkEq(rhsNode, pos);
      lhs = checkIntegralBinary(lhs, rhs, "Bitwise '&'", pos);
      if (lhs == Derived::Error) return Derived::Error;
    }
    return lhs;
  }

  Derived checkEq(const EqExpr& eq, Position pos) {
    Derived lhs = (*this)(eq.left);
    if (eq.right.empty()) return lhs;
    for (const auto& rhsNode : eq.right) {
      Derived rhs = (*this)(rhsNode.rhs);
      if (!commonType(lhs, rhs)) {
        expectedButGot(lhs, rhs, pos);
        return Derived::Error;
      }
      lhs = Derived::Bool;
    }
    return Derived::Bool;
  }

  Derived operator()(const CmpExpr& cmp) {
    Derived lhs = (*this)(cmp.left);
    if (cmp.right.empty()) return lhs;
    for (const auto& rhsNode : cmp.right) {
      Derived rhs = (*this)(rhsNode.rhs);
      if (lhs == Derived::Error || rhs == Derived::Error) return Derived::Error;
      if (!isNumeric(lhs)) {
        addError(rhsNode.pos,
                 std::format("Comparison requires numeric type, got {}",
                             derivedToString(lhs)));
        return Derived::Error;
      }
      if (!isNumeric(rhs)) {
        addError(rhsNode.pos,
                 std::format("Comparison requires numeric type, got {}",
                             derivedToString(rhs)));
        return Derived::Error;
      }
      lhs = Derived::Bool;
    }
    return Derived::Bool;
  }

  Derived operator()(const ShiftExpr& shift) {
    Derived lhs = (*this)(shift.left);
    for (const auto& rhsNode : shift.right) {
      Derived rhs = (*this)(rhsNode.rhs);
      lhs = checkIntegralBinary(lhs, rhs, "Shift", rhsNode.pos);
      if (lhs == Derived::Error) return Derived::Error;
    }
    return lhs;
  }

  Derived operator()(const AddExpr& add) {
    Derived lhs = (*this)(add.left);
    for (const auto& rhsNode : add.right) {
      Derived rhs = (*this)(rhsNode.rhs);
      if (lhs == Derived::Error || rhs == Derived::Error) return Derived::Error;
      if (!isNumeric(lhs)) {
        addError(rhsNode.pos,
                 std::format("Additive operator requires numeric type, got {}",
                             derivedToString(lhs)));
        return Derived::Error;
      }
      if (!isNumeric(rhs)) {
        addError(rhsNode.pos,
                 std::format("Additive operator requires numeric type, got {}",
                             derivedToString(rhs)));
        return Derived::Error;
      }
      lhs = *commonType(lhs, rhs);
    }
    return lhs;
  }

  Derived operator()(const MulExpr& mul) {
    Derived lhs = (*this)(mul.left);
    for (const auto& rhsNode : mul.right) {
      Derived rhs = (*this)(rhsNode.rhs);
      if (rhsNode.op == MulOp::Mod) {
        lhs = checkIntegralBinary(lhs, rhs, "Modulo '%'", rhsNode.pos);
        if (lhs == Derived::Error) return Derived::Error;
      } else {
        if (lhs == Derived::Error || rhs == Derived::Error)
          return Derived::Error;
        if (!isNumeric(lhs)) {
          addError(rhsNode.pos,
                   std::format(
                       "Multiplicative operator requires numeric type, got {}",
                       derivedToString(lhs)));
          return Derived::Error;
        }
        if (!isNumeric(rhs)) {
          addError(rhsNode.pos,
                   std::format(
                       "Multiplicative operator requires numeric type, got {}",
                       derivedToString(rhs)));
          return Derived::Error;
        }
        lhs = *commonType(lhs, rhs);
      }
    }
    return lhs;
  }

  Derived operator()(const UnaryExpr& unary) {
    Derived t = visit(unary.primary);
    if (t == Derived::Error) return Derived::Error;

    for (auto it = unary.ops.rbegin(); it != unary.ops.rend(); ++it) {
      switch (*it) {
        case UnaryOp::Neg:
          if (t == Derived::Uint) {
            t = Derived::Int;
          } else if (t != Derived::Int && t != Derived::Float) {
            addError(unary.pos,
                     std::format("Unary '-' requires numeric type, got {}",
                                 derivedToString(t)));
            return Derived::Error;
          }
          break;
        case UnaryOp::Not:
          if (t != Derived::Bool) {
            expectedButGot(Derived::Bool, t, unary.pos);
            return Derived::Error;
          }
          break;
        case UnaryOp::BitNot:
          if (!isIntegral(t)) {
            addError(unary.pos,
                     std::format("Unary '~' requires Int or Uint, got {}",
                                 derivedToString(t)));
            return Derived::Error;
          }
          break;
      }
    }
    return t;
  }

  Derived operator()(const InParens& p) { return (*this)(*p.expr); }
  Derived operator()(const Literal& lit) { return visit(lit); }
  Derived operator()(const IntLit&) { return Derived::Int; }
  Derived operator()(const UintLit&) { return Derived::Uint; }
  Derived operator()(const HexLit&) { return Derived::Uint; }
  Derived operator()(const FloatLit&) { return Derived::Float; }
  Derived operator()(const True&) { return Derived::Bool; }
  Derived operator()(const False&) { return Derived::Bool; }

  Derived operator()(const ArrayLit& arr) {
    if (arr.elements.empty()) {
      return Derived::AnyArray;
    }

    Derived elemType = (*this)(arr.elements[0]);
    for (std::size_t i = 1; i < arr.elements.size(); ++i) {
      Derived nextType = (*this)(arr.elements[i]);
      auto common = commonType(elemType, nextType);
      if (!common) {
        expectedButGot(elemType, nextType, arr.pos);
        return Derived::Error;
      }
      elemType = *common;
    }

    if (elemType == Derived::Error) return Derived::Error;
    auto arrType = scalarToArray(elemType);
    if (!arrType) {
      addError(arr.pos,
               std::format("Array elements must be scalar types, got {}",
                           derivedToString(elemType)));
      return Derived::Error;
    }
    return *arrType;
  }

  Derived operator()(const ArrayAlloc& alloc) {
    Derived sizeType = (*this)(*alloc.size);
    if (sizeType != Derived::Uint && sizeType != Derived::Error) {
      expectedButGot(Derived::Uint, sizeType, alloc.pos);
    }
    return Derived::AnyArray;
  }

  Derived operator()(const QualIdent& qid) {
    if (qid.module.has_value()) {
      std::string fullName = qid.module->ident + "." + qid.ident.ident;
      if (auto fnSig = lookupFunc(fullName); fnSig && fnSig->params.empty()) {
        return fnSig->ret;
      }
      addError(qid.module->pos,
               std::format("Unknown module symbol '{}'", fullName));
      return Derived::Error;
    }

    if (auto varType = lookupVar(qid.ident.ident)) {
      return *varType;
    }
    if (auto fnSig = lookupFunc(qid.ident.ident);
        fnSig && fnSig->params.empty()) {
      return fnSig->ret;
    }

    addError(qid.ident.pos,
             std::format("Undefined identifier '{}'", qid.ident.ident));
    return Derived::Error;
  }

  Derived operator()(const Subscript& sub) {
    Derived arrType = (*this)(sub.expr);
    Derived idxType = (*this)(*sub.index);

    if (!isIntegral(idxType) && idxType != Derived::Error) {
      addError(sub.expr.ident.pos,
               std::format("Array index must be Int or Uint, got {}",
                           derivedToString(idxType)));
    }

    if (arrType == Derived::Error) return Derived::Error;
    auto elem = arrayElemType(arrType);
    if (!elem) {
      addError(sub.expr.ident.pos,
               std::format("Cannot subscript non-array type {}",
                           derivedToString(arrType)));
      return Derived::Error;
    }
    return *elem;
  }

  Derived operator()(const Call& call) {
    std::string fullName = call.ident.module ? call.ident.module->ident + "." +
                                                   call.ident.ident.ident
                                             : call.ident.ident.ident;

    auto sig = lookupFunc(fullName);
    if (!sig) {
      addError(call.ident.ident.pos,
               std::format("Undefined function '{}'", fullName));
      return Derived::Error;
    }

    if (call.args.size() != sig->params.size()) {
      addError(call.ident.ident.pos,
               std::format("Function '{}' expects {} arguments, got {}",
                           fullName, sig->params.size(), call.args.size()));
      return sig->ret;
    }

    for (std::size_t i = 0; i < call.args.size(); ++i) {
      Derived argType = (*this)(call.args[i]);
      if (!canImplicitCast(argType, sig->params[i])) {
        expectedButGot(sig->params[i], argType, call.ident.ident.pos);
      }
    }
    return sig->ret;
  }
};
