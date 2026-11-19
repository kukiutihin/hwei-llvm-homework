#pragma once

#include <memory>
#include <optional>
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
  std::optional<std::unique_ptr<Expr>> next;
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
  std::int64_t value;
};

struct IntLit {
  std::int64_t value;
};

struct FloatLit {
  double value;
};

using Literal = std::variant<HexLit, FloatLit, IntLit, True, False>;

struct InParens {
  std::unique_ptr<Expr> expr;
};

using PrimaryExpr = std::variant<Call, Subscript, InParens, Literal, QualIdent>;

struct UnaryExpr {
  std::unique_ptr<UnaryExpr> left;
  PrimaryExpr right;
};

struct MulExpr {
  UnaryExpr left;
  std::vector<UnaryExpr> right;
};

struct AddExpr {
  MulExpr left;
  std::vector<MulExpr> right;
};

struct ShiftExpr {
  AddExpr left;
  std::vector<AddExpr> right;
};

struct CmpExpr {
  ShiftExpr left;
  std::vector<ShiftExpr> right;
};

struct EqExpr {
  CmpExpr left;
  std::vector<CmpExpr> right;
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

struct BinExpr {
  BitOrExpr left;
  std::optional<BitOrExpr> right;
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
