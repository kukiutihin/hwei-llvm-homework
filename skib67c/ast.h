#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

using Ident = std::string;
using ModIdent = std::string;

struct QualIdent {
  std::optional<ModIdent> module;
  Ident ident;
};

struct Skib {};
struct Int {};
struct Uint {};
struct Float {};
struct Bool {};

using ScalarType = std::variant<Int, Uint, Float, Bool>;

struct ArrayType {
  ScalarType generic;
};

using Type = std::variant<Skib, ScalarType, ArrayType>;

struct Param {
  Ident ident;
  Type type;
};

using ParamList = std::vector<Param>;

struct Expr;

struct FuncDecl {
  Ident ident;
  ParamList paramList;
  Type type;
  std::unique_ptr<Expr> expr;
};

struct FuncDeclExpr {
  FuncDecl decl;
  std::unique_ptr<Expr> next;
};

struct IfExpr {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Expr> then;
  std::unique_ptr<Expr> els;
};

struct Subscript {
  QualIdent expr;
  std::unique_ptr<Expr> index;
};

using ArgsList = std::vector<Expr>;

struct Call {
  QualIdent ident;
  ArgsList args;
};

struct True {};
struct False {};

struct HexLit {
  std::uint64_t value;
};

struct IntLit {
  std::int64_t value;
};

struct FloatLit {
  double value;
};

using Literal = std::variant<Skib, HexLit, FloatLit, IntLit, True, False>;

struct InParens {
  std::unique_ptr<Expr> expr;
};

using PrimaryExpr = std::variant<Call, Subscript, InParens, Literal, QualIdent>;

enum class UnaryOp { Neg, Not, BitNot };

struct UnaryExpr {
  std::vector<UnaryOp> ops;
  PrimaryExpr primary;
};

enum class MulOp { Mul, Div, Mod };

struct MulRhs {
  MulOp op;
  UnaryExpr rhs;
};

struct MulExpr {
  UnaryExpr left;
  std::vector<MulRhs> right;
};

enum class AddOp { Add, Sub };

struct AddRhs {
  AddOp op;
  MulExpr rhs;
};

struct AddExpr {
  MulExpr left;
  std::vector<AddRhs> right;
};

enum class ShiftOp { Shl, Shr };
struct ShiftRhs {
  ShiftOp op;
  AddExpr rhs;
};
struct ShiftExpr {
  AddExpr left;
  std::vector<ShiftRhs> right;
};

enum class CmpOp { Lt, Le, Gt, Ge };

struct CmpRhs {
  CmpOp op;
  ShiftExpr rhs;
};
struct CmpExpr {
  ShiftExpr left;
  std::vector<CmpRhs> right;
};

enum class EqOp { Eq, Ne };
struct EqRhs {
  EqOp op;
  CmpExpr rhs;
};

struct EqExpr {
  CmpExpr left;
  std::vector<EqRhs> right;
};

struct BitAndExpr {
  EqExpr left;
  std::vector<EqExpr> right;
};

struct BitXorExpr {
  BitAndExpr left;
  std::vector<BitAndExpr> right;
};

struct BitOrExpr {
  BitXorExpr left;
  std::vector<BitXorExpr> right;
};

enum class RangeOp { Exclusive, Inclusive };

struct RangeRhs {
  RangeOp op;
  BitOrExpr rhs;
};

struct BinExpr {
  BitOrExpr left;
  std::optional<RangeRhs> right;
};

using ValueExpr = std::variant<IfExpr, BinExpr>;

struct Assignment {
  Ident ident;
  std::optional<Type> type;
  ValueExpr expr;
  std::unique_ptr<Expr> next;
};

struct MutExpr {
  Subscript subscript;
  ValueExpr value;
};

struct SkibExpr;

using BindExpr =
    std::variant<ValueExpr, MutExpr, Assignment, FuncDeclExpr>;

struct Expr {
  std::vector<BindExpr> binds;
};

struct ValDecl {
  Ident ident;
  std::optional<Type> type;
  ValueExpr expr;
};

using TopDecl = std::variant<FuncDecl, ValDecl>;

using Program = std::vector<TopDecl>;
