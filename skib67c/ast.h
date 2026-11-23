#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Position {
  std::size_t line;
  std::size_t column;
};

struct Ident {
  std::string ident;
  Position pos;
};

struct Skib {
  Position pos;
};

struct Int {
  Position pos;
};

struct Uint {
  Position pos;
};

struct Float {
  Position pos;
};

struct Bool {
  Position pos;
};

using ScalarType = std::variant<Int, Uint, Float, Bool>;

struct ArrayType {
  Position pos;
  ScalarType generic;
};

using Type = std::variant<Skib, ScalarType, ArrayType>;

struct Param {
  Position pos;
  Ident ident;
  Type type;
};

using ParamList = std::vector<Param>;

struct Expr;

struct FuncDecl {
  Position pos;
  Ident ident;
  ParamList paramList;
  Type type;
  std::unique_ptr<Expr> expr;
};

struct FuncDeclExpr {
  Position pos;
  FuncDecl decl;
  std::unique_ptr<Expr> next;
};

struct IfExpr {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Expr> then;
  std::unique_ptr<Expr> els;
};

struct Subscript {
  Ident expr;
  std::unique_ptr<Expr> index;
};

using ArgsList = std::vector<Expr>;

struct Call {
  Ident ident;
  ArgsList args;
};

struct True {
  Position pos;
};

struct False {
  Position pos;
};

struct HexLit {
  Position pos;
  std::uint64_t value;
};

struct IntLit {
  Position pos;
  std::int64_t value;
};

struct FloatLit {
  Position pos;
  double value;
};

struct UintLit {
  Position pos;
  std::uint64_t value;
};

using Literal =
    std::variant<Skib, HexLit, FloatLit, IntLit, UintLit, True, False>;

struct InParens {
  std::unique_ptr<Expr> expr;
};

struct ArrayLit {
  Position pos;
  std::vector<Expr> elements;
};

struct ArrayAlloc {
  Position pos;
  std::unique_ptr<Expr> size;
  mutable std::optional<ScalarType> elemType;
};

struct Rgb2Linear {
  Position pos;
  std::unique_ptr<Expr> expr;
};

struct Linear2Rgb {
  Position pos;
  std::unique_ptr<Expr> expr;
};

using PrimaryExpr =
    std::variant<Call, Subscript, InParens, ArrayLit, ArrayAlloc, Literal,
                 Ident, Linear2Rgb, Rgb2Linear>;

enum class UnaryOp { Neg, Not, BitNot };

struct UnaryExpr {
  Position pos;
  std::vector<UnaryOp> ops;
  PrimaryExpr primary;
};

enum class MulOp { Mul, Div, Mod };

struct MulRhs {
  Position pos;
  MulOp op;
  UnaryExpr rhs;
};

struct MulExpr {
  UnaryExpr left;
  std::vector<MulRhs> right;
};

enum class AddOp { Add, Sub };

struct AddRhs {
  Position pos;
  AddOp op;
  MulExpr rhs;
};

struct AddExpr {
  Position pos;
  MulExpr left;
  std::vector<AddRhs> right;
};

enum class ShiftOp { Shl, Shr };
struct ShiftRhs {
  Position pos;
  ShiftOp op;
  AddExpr rhs;
};
struct ShiftExpr {
  AddExpr left;
  std::vector<ShiftRhs> right;
};

enum class CmpOp { Lt, Le, Gt, Ge };

struct CmpRhs {
  Position pos;
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

struct ValueExpr {
  Position pos;
  std::variant<IfExpr, BitOrExpr> expr;
};

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

using BindExpr = std::variant<ValueExpr, MutExpr, Assignment, FuncDeclExpr>;

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
