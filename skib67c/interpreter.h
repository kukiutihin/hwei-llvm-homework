#pragma once

#include <assert.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <print>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ast.h"
#include "sim.h"

struct SkibValue {};

using std::int64_t;
using std::size_t;
using std::uint64_t;

struct Variable;

using Heap = std::vector<std::vector<std::unique_ptr<Variable>>>;
using HeapIdx = size_t;

struct Variable {
  std::variant<int64_t, uint64_t, double, bool, SkibValue, HeapIdx> value;
};

struct Function {
  const FuncDecl* function;
  int64_t defineInFrame;
};

struct FunctionFrame {
  Function current;
  std::unordered_map<std::string_view, Function> functions;
  std::unordered_map<std::string_view, Variable> variables;
};

class Interpreter {
  FunctionFrame globalScope;
  std::vector<FunctionFrame> frames;
  Heap heap;

  int64_t toInt(const Variable& x) {
    if (const uint64_t* ui = std::get_if<uint64_t>(&x.value))
      return static_cast<std::int64_t>(*ui);
    return std::get<int64_t>(x.value);
  };

  double toDouble(const Variable& x) {
    if (const uint64_t* ui = std::get_if<uint64_t>(&x.value))
      return static_cast<double>(*ui);
    else if (const int64_t* ui = std::get_if<int64_t>(&x.value))
      return static_cast<double>(*ui);
    return std::get<double>(x.value);
  };

 public:
  void interpret(const Program& program) {
    for (const auto& topDecl : program) std::visit(*this, topDecl);
    run(globalScope.functions.at("main"));
  }

  Variable run(Function func, const ArgsList& argList = {}) {
    std::vector<Variable> args;
    args.reserve(argList.size());
    for (const auto& arg : argList) args.push_back((*this)(arg));

    frames.push_back(FunctionFrame{func, {}, {}});

    for (size_t i = 0; i < args.size(); i++)
      frames.back().variables[func.function->paramList[i].ident.ident] =
          args[i];

    auto res = (*this)(*func.function->expr);
    frames.pop_back();
    return res;
  }

  void operator()(const FuncDecl& func) {
    globalScope.functions[func.ident.ident] = Function(&func, -1);
  }

  void operator()(const ValDecl& val) {
    globalScope.variables[val.ident.ident] = std::visit(*this, val.expr.expr);
  }

  Variable operator()(const IfExpr& expr) {
    auto condV = std::get<bool>((*this)(*expr.cond).value);
    if (condV)
      return (*this)(*expr.then);
    else
      return (*this)(*expr.els);
  }

  template <typename Op>
  void applyBinary(Variable& a, const Variable& b, Op op) {
    if (std::holds_alternative<uint64_t>(a.value) &&
        std::holds_alternative<uint64_t>(b.value))
      a.value = op(std::get<uint64_t>(a.value), std::get<uint64_t>(b.value));
    else if (std::holds_alternative<double>(a.value) ||
             std::holds_alternative<double>(b.value))
      a.value = op(toDouble(a), toDouble(b));
    else
      a.value = op(toInt(a), toInt(b));
  }

  template <typename Op>
  void applyCmpBinary(Variable& a, const Variable& b, Op op) {
    if (std::holds_alternative<bool>(a.value) &&
        std::holds_alternative<bool>(b.value))
      a.value = op(std::get<bool>(a.value), std::get<bool>(b.value));
    else
      applyBinary(a, b, op);
  }

  template <typename Op>
  void applyIntegralBinary(Variable& a, const Variable& b, Op op) {
    if (std::holds_alternative<uint64_t>(a.value) &&
        std::holds_alternative<uint64_t>(b.value))
      a.value = op(std::get<uint64_t>(a.value), std::get<uint64_t>(b.value));
    else
      a.value = op(toInt(a), toInt(b));
  }

  Variable operator()(const BitOrExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      applyIntegralBinary(left, (*this)(r),
                          [](auto a, auto b) { return a | b; });
    return left;
  }

  Variable operator()(const BitXorExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      applyIntegralBinary(left, (*this)(r),
                          [](auto a, auto b) { return a ^ b; });
    return left;
  }

  Variable operator()(const BitAndExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      applyIntegralBinary(left, (*this)(r),
                          [](auto a, auto b) { return a & b; });
    return left;
  }

  Variable operator()(const EqExpr& expr) {
    auto left = (*this)(expr.left);
    if (expr.right.empty()) return left;

    for (const auto& r : expr.right)
      applyCmpBinary(left, (*this)(r.rhs), [&r](auto a, auto b) {
        switch (r.op) {
          case EqOp::Eq:
            return a == b;
          case EqOp::Ne:
            return a != b;
        }
      });
    return left;
  }

  Variable operator()(const CmpExpr& expr) {
    auto left = (*this)(expr.left);
    if (expr.right.empty()) return left;

    for (const auto& r : expr.right)
      applyCmpBinary(left, (*this)(r.rhs), [&r](auto a, auto b) {
        switch (r.op) {
          case CmpOp::Lt:
            return a < b;
          case CmpOp::Le:
            return a <= b;
          case CmpOp::Gt:
            return a > b;
          case CmpOp::Ge:
            return a >= b;
        }
      });
    return left;
  }

  Variable operator()(const ShiftExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      applyIntegralBinary(left, (*this)(r.rhs), [&r](auto a, auto b) {
        switch (r.op) {
          case ShiftOp::Shl:
            return a <<= b;
          case ShiftOp::Shr:
            return a >>= b;
        }
      });
    return left;
  }

  Variable operator()(const AddExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      applyBinary(left, (*this)(r.rhs), [&r](auto a, auto b) {
        switch (r.op) {
          case AddOp::Add:
            return a += b;
          case AddOp::Sub:
            return a -= b;
        }
      });
    return left;
  }

  Variable operator()(const MulExpr& expr) {
    auto left = (*this)(expr.left);
    for (const auto& r : expr.right)
      if (r.op == MulOp::Mod)
        applyIntegralBinary(left, (*this)(r.rhs),
                            [&r](auto a, auto b) { return a % b; });
      else
        applyBinary(left, (*this)(r.rhs), [&r](auto a, auto b) {
          switch (r.op) {
            case MulOp::Mul:
              return a *= b;
            case MulOp::Div:
              return a /= b;
            default:
              return a /= b;
          }
        });
    return left;
  }

  Variable operator()(const UnaryExpr& expr) {
    auto left = (*this)(expr.primary);
    for (const auto& op : expr.ops) {
      switch (op) {
        case UnaryOp::Neg:
          if (auto* val = std::get_if<int64_t>(&left.value)) left.value = -*val;

          if (auto* val = std::get_if<uint64_t>(&left.value))
            left.value = -static_cast<int64_t>(*val);

          else if (auto* val = std::get_if<double>(&left.value))
            left.value = -*val;
          break;

        case UnaryOp::Not:
          if (auto* val = std::get_if<bool>(&left.value)) left.value = !*val;
          break;

        case UnaryOp::BitNot:
          if (auto* val = std::get_if<uint64_t>(&left.value))
            left.value = ~*val;
          else if (auto* val = std::get_if<int64_t>(&left.value))
            left.value = ~*val;
          break;
      }
    }
    return left;
  }

  Variable operator()(const PrimaryExpr& expr) {
    return std::visit(*this, expr);
  }

  Variable operator()(const Call& call) {
    if (call.ident.ident == "simInit") {
      simInit();
      return Variable{SkibValue{}};

    } else if (call.ident.ident == "simExit") {
      simExit();
      return Variable{SkibValue{}};

    } else if (call.ident.ident == "simHasClick")
      return Variable{(bool)simHasClick()};

    else if (call.ident.ident == "simGetClick")
      return Variable{(std::uint64_t)simGetClick()};

    else if (call.ident.ident == "simPutPixel") {
      auto x = std::get<uint64_t>((*this)(call.args[0]).value);
      auto y = std::get<uint64_t>((*this)(call.args[1]).value);
      auto argb = std::get<uint64_t>((*this)(call.args[2]).value);
      simPutPixel(x, y, argb);
      return Variable{};
    }

    else if (call.ident.ident == "simFlush") {
      simFlush();
      return Variable{SkibValue{}};
    }

    else if (call.ident.ident == "fmax") {
      auto x = std::get<double>((*this)(call.args[0]).value);
      auto y = std::get<double>((*this)(call.args[1]).value);
      return Variable{x >= y ? x : y};
    }

    else if (call.ident.ident == "fmin") {
      auto x = std::get<double>((*this)(call.args[0]).value);
      auto y = std::get<double>((*this)(call.args[1]).value);
      return Variable{x <= y ? x : y};
    }

    else if (call.ident.ident == "printi") {
      auto val = (*this)(call.args[0]).value;
      int64_t x = std::holds_alternative<uint64_t>(val)
                      ? static_cast<int64_t>(std::get<uint64_t>(val))
                      : std::get<int64_t>(val);
      std::println("{}", x);
      return Variable{SkibValue{}};
    }

    else if (call.ident.ident == "printf") {
      auto val = (*this)(call.args[0]).value;
      double x = std::visit(
          [](const auto& v) -> double {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, double> ||
                          std::is_same_v<T, int64_t> ||
                          std::is_same_v<T, uint64_t>) {
              return static_cast<double>(v);
            }
            throw std::bad_variant_access{};
          },
          val);
      std::println("{}", x);
      return Variable{SkibValue{}};

    } else if (call.ident.ident == "printui") {
      auto x = std::get<uint64_t>((*this)(call.args[0]).value);
      std::println("{}", x);
      return Variable{SkibValue{}};
    }

    else if (call.ident.ident == "fabs") {
      auto x = std::get<double>((*this)(call.args[0]).value);
      return Variable{x >= 0 ? x : -x};
    }

    else if (call.ident.ident == "sqrt") {
      auto x = std::get<double>((*this)(call.args[0]).value);
      return Variable{std::sqrt(x)};
    }

    else if (call.ident.ident == "f2ui") {
      auto x = std::get<double>((*this)(call.args[0]).value);
      return Variable{static_cast<uint64_t>(x)};
    }

    else {
      auto findFunc = [this, &call]() -> Function {
        if (!frames.empty()) {
          if (auto fn = frames.back().functions.find(call.ident.ident);
              fn != frames.back().functions.end())
            return fn->second;

          for (int64_t idx = frames.back().current.defineInFrame; idx >= 0;
               idx = frames[idx].current.defineInFrame) {
            auto& parentFns = frames[idx].functions;
            if (const auto& fn = parentFns.find(call.ident.ident);
                fn != parentFns.end())
              return fn->second;
          }
        }
        return globalScope.functions.at(call.ident.ident);
      };

      return run(findFunc(), call.args);
    }
  }

  std::pair<HeapIdx, size_t> resolveSubscript(const Subscript& subscript) {
    auto heapIdx = std::get<HeapIdx>((*this)(subscript.expr).value);
    auto indexVal = (*this)(*subscript.index).value;
    size_t arrayIdx = std::holds_alternative<uint64_t>(indexVal)
                          ? std::get<uint64_t>(indexVal)
                          : static_cast<size_t>(std::get<int64_t>(indexVal));
    return {heapIdx, arrayIdx};
  }

  Variable operator()(const Subscript& subscript) {
    auto [heapIdx, arrayIdx] = resolveSubscript(subscript);
    return *heap[heapIdx][arrayIdx];
  }

  Variable operator()(const InParens& val) { return (*this)(*val.expr); }

  Variable operator()(const ArrayLit& lit) {
    std::vector<std::unique_ptr<Variable>> vars;
    vars.reserve(lit.elements.size());
    for (const auto& e : lit.elements)
      vars.push_back(std::make_unique<Variable>((*this)(e)));
    heap.push_back(std::move(vars));
    return Variable{heap.size() - 1};
  }

  Variable defaultScalar(const std::optional<ScalarType>& elemType) {
    if (!elemType) return Variable{int64_t{0}};
    return std::visit(
        [](const auto& t) -> Variable {
          using T = std::decay_t<decltype(t)>;
          if constexpr (std::is_same_v<T, Int>) return Variable{int64_t{0}};
          if constexpr (std::is_same_v<T, Uint>) return Variable{uint64_t{0}};
          if constexpr (std::is_same_v<T, Float>) return Variable{0.0};
          if constexpr (std::is_same_v<T, Bool>) return Variable{false};
        },
        *elemType);
  }

  Variable operator()(const ArrayAlloc& alloc) {
    size_t len = std::get<uint64_t>((*this)(*alloc.size).value);
    std::vector<std::unique_ptr<Variable>> vec;
    vec.reserve(len);
    Variable initVal = defaultScalar(alloc.elemType);
    for (size_t i = 0; i < len; i++) {
      vec.push_back(std::make_unique<Variable>(initVal));
    }
    heap.push_back(std::move(vec));
    return Variable{heap.size() - 1};
  }

  Variable operator()(const Skib&) { return Variable{SkibValue{}}; }
  Variable operator()(const HexLit& lit) { return Variable{lit.value}; }
  Variable operator()(const FloatLit& lit) { return Variable{lit.value}; }
  Variable operator()(const IntLit& lit) { return Variable{lit.value}; }
  Variable operator()(const UintLit& lit) { return Variable{lit.value}; }
  Variable operator()(const True&) { return Variable{true}; }
  Variable operator()(const False&) { return Variable{false}; }
  Variable operator()(const Literal& lit) { return std::visit(*this, lit); }

  Variable operator()(const Ident& ident) {
    if (frames.size() > 0) {
      if (const auto& var = frames.back().variables.find(ident.ident);
          var != frames.back().variables.end())
        return var->second;

      for (int64_t i = frames.back().current.defineInFrame; i >= 0;
           i = frames[i].current.defineInFrame) {
        auto& parentFns = frames[i].functions;
        if (const auto& var =
                frames[frames.back().current.defineInFrame].variables.find(
                    ident.ident);
            var != frames[frames.back().current.defineInFrame].variables.end())
          return var->second;
      }
    }

    return globalScope.variables[ident.ident];
  }

  Variable operator()(const Linear2Rgb& val) {
    auto var = (*this)(*val.expr);
    double linear = 0.0;

    if (const auto* d = std::get_if<double>(&var.value))
      linear = *d;
    else if (const auto* i = std::get_if<std::int64_t>(&var.value))
      linear = static_cast<double>(*i);
    else if (const auto* u = std::get_if<std::uint64_t>(&var.value))
      linear = static_cast<double>(*u);

    linear = std::clamp(linear, 0.0, 1.0);
    double s = linear <= 0.0031308
                   ? linear * 12.92
                   : 1.055 * std::pow(linear, 1.0 / 2.4) - 0.055;
    var.value = static_cast<std::uint64_t>(std::round(s * 255.0));
    return var;
  }

  Variable operator()(const Rgb2Linear& val) {
    auto var = (*this)(*val.expr);
    if (const auto* channel = std::get_if<std::uint64_t>(&var.value)) {
      double s = static_cast<double>(*channel) / 255.0;
      var.value = s <= 0.04045 ? s / 12.92 : std::pow((s + 0.055) / 1.055, 2.4);
    }
    return var;
  }

  Variable operator()(const ValueExpr& expr) {
    return std::visit(*this, expr.expr);
  }

  Variable operator()(const MutExpr& mut) {
    auto [heapIdx, arrayIdx] = resolveSubscript(mut.subscript);
    heap[heapIdx][arrayIdx] = std::make_unique<Variable>((*this)(mut.value));
    return Variable{SkibValue{}};
  }

  Variable operator()(const Assignment& as) {
    auto value = (*this)(as.expr);
    frames.back().variables[as.ident.ident] = value;
    return as.next ? (*this)(*as.next) : Variable{SkibValue{}};
  }

  Variable operator()(const FuncDeclExpr& funcDecl) {
    frames.back().functions[funcDecl.decl.ident.ident] =
        Function(&funcDecl.decl, frames.size() - 1);
    return (*this)(*funcDecl.next);
  }

  Variable operator()(const Expr& expr) {
    for (size_t i = 0; i < expr.binds.size() - 1; i++) {
      std::visit(*this, expr.binds[i]);
    }
    return std::visit(*this, expr.binds.back());
  }
};
