#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/dsl/lookahead.hpp>
#include <lexy/dsl/option.hpp>
#include <lexy/dsl/symbol.hpp>
#include <lexy/input/buffer.hpp>
#include <lexy/input/file.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ast.h"
#include "error.h"

namespace grammar {
namespace dsl = lexy::dsl;

#define let static constexpr auto

let ws = dsl::while_(dsl::ascii::space);
let keyword = dsl::identifier(dsl::ascii::lower, dsl::ascii::alpha);
let typeIdent = dsl::identifier(dsl::ascii::upper, dsl::ascii::alpha);
let identTok = dsl::identifier(dsl::ascii::lower / dsl::lit_c<'_'>,
                               dsl::ascii::alpha_digit_underscore);
let modIdentTok = dsl::identifier(dsl::ascii::upper, dsl::ascii::alpha_digit);

struct PIdent {
  let name = "identifier";
  let rule = identTok;
  let value = lexy::as_string<Ident>;
};

struct PModIdent {
  let name = "module identifier";
  let rule = modIdentTok;
  let value = lexy::as_string<ModIdent>;
};

struct PQualIdent {
  let name = "identifier";
  let rule = dsl::opt(dsl::p<PModIdent> >> dsl::lit_c<'.'>) + dsl::p<PIdent>;
  let value = lexy::construct<QualIdent>;
};

struct PHexLit {
  let name = "hex literal";
  let rule = LEXY_LIT("0x") >>
             dsl::integer<std::uint64_t>(dsl::digits<dsl::hex>);
  let value = lexy::construct<HexLit>;
};

struct PFloatLit {
  let name = "float literal";
  let rule =
      dsl::capture(dsl::token(dsl::digits<> + dsl::lit_c<'.'> + dsl::digits<>));
  let value = lexy::callback<FloatLit>([](auto lexeme) {
    double result = 0.0;
    const auto* begin = reinterpret_cast<const char*>(lexeme.data());
    std::from_chars(begin, begin + lexeme.size(), result);
    return FloatLit{result};
  });
};

struct PIntLit {
  let name = "int literal";
  let rule = dsl::integer<std::int64_t>(dsl::digits<>);
  let value = lexy::construct<IntLit>;
};

struct PTrue {
  let name = "bool literal";
  let rule = LEXY_KEYWORD("True", typeIdent);
  let value = lexy::construct<True>;
};

struct PFalse {
  let name = "bool literal";
  let rule = LEXY_KEYWORD("False", typeIdent);
  let value = lexy::construct<False>;
};

struct PSkib {
  let name = "skib";
  let rule = LEXY_LIT("()");
  let value = lexy::construct<Skib>;
};

struct PLiteral {
  let name = "literal";
  let rule = dsl::p<PSkib> | dsl::p<PHexLit> | dsl::p<PTrue> | dsl::p<PFalse> |
             (dsl::peek(dsl::token(dsl::digits<> + dsl::lit_c<'.'>)) >>
              dsl::p<PFloatLit>) |
             (dsl::peek(dsl::digits<>) >> dsl::p<PIntLit>);
  let value = lexy::construct<Literal>;
};

struct PBool {
  let name = "type";
  let rule = LEXY_KEYWORD("Bool", typeIdent);
  let value = lexy::construct<Bool>;
};

struct PFloat {
  let name = "type";
  let rule = LEXY_KEYWORD("Float", typeIdent);
  let value = lexy::construct<Float>;
};

struct PUint {
  let name = "type";
  let rule = LEXY_KEYWORD("Uint", typeIdent);
  let value = lexy::construct<Uint>;
};

struct PInt {
  let name = "type";
  let rule = LEXY_KEYWORD("Int", typeIdent);
  let value = lexy::construct<Int>;
};

struct PScalar {
  let name = "type";
  let rule = dsl::p<PInt> | dsl::p<PUint> | dsl::p<PFloat> | dsl::p<PBool>;
  let value = lexy::construct<ScalarType>;
};

struct PArrayType {
  let name = "type";
  let rule = LEXY_KEYWORD("Array", typeIdent) >>
             dsl::square_bracketed(dsl::p<PScalar>);
  let value = lexy::construct<ArrayType>;
};

struct PType {
  let name = "type";
  let rule = dsl::p<PSkib> | dsl::p<PArrayType> | dsl::p<PScalar>;
  let value = lexy::construct<Type>;
};

struct PExpr;
struct PValueExpr;

struct PParam {
  let name = "function parameter";
  let rule = dsl::p<PIdent> + dsl::lit_c<':'> + dsl::p<PType>;
  let value = lexy::construct<Param>;
};

struct PParamList {
  let name = "function parameters";
  let rule =
      dsl::round_bracketed.opt_list(dsl::p<PParam>, dsl::sep(dsl::comma));
  let value = lexy::as_list<ParamList>;
};

struct PArgsList {
  let name = "function arguments";
  let rule =
      dsl::round_bracketed.opt_list(dsl::recurse<PExpr>, dsl::sep(dsl::comma));
  let value = lexy::as_list<ArgsList>;
};

struct PIndexExpr {
  let name = "array index";
  let rule = dsl::square_bracketed(dsl::recurse<PExpr>);
  let value = lexy::callback<std::unique_ptr<Expr>>(
      [](Expr e) { return std::make_unique<Expr>(std::move(e)); });
};

struct PInParens {
  let name = "parenthesized expression";
  let rule = dsl::round_bracketed(dsl::recurse<PExpr>);
  let value = lexy::callback<InParens>([](Expr expr) {
    return InParens{std::make_unique<Expr>(std::move(expr))};
  });
};

struct PSubscript {
  let name = "array subscript";
  let rule = dsl::p<PQualIdent> + dsl::p<PIndexExpr>;
  let value = lexy::construct<Subscript>;
};

struct PPrimaryExpr {
  let name = "primary expression";
  let rule = [] {
    auto after_qual_ident =
        dsl::p<PArgsList> | dsl::p<PIndexExpr> | (dsl::else_ >> dsl::nullopt);
    auto ident_or_call_or_sub =
        dsl::else_ >> (dsl::p<PQualIdent> + after_qual_ident);
    return dsl::p<PLiteral> | dsl::p<PInParens> | ident_or_call_or_sub;
  }();

  let value = lexy::callback<PrimaryExpr>(
      [](Literal lit) -> PrimaryExpr { return lit; },
      [](InParens parens) -> PrimaryExpr { return std::move(parens); },
      [](QualIdent id, ArgsList args) -> PrimaryExpr {
        return Call{std::move(id), std::move(args)};
      },
      [](QualIdent id, std::unique_ptr<Expr> idx) -> PrimaryExpr {
        return Subscript{std::move(id), std::move(idx)};
      },
      [](QualIdent id, lexy::nullopt) -> PrimaryExpr { return id; });
};

let unary_op_table =
    lexy::symbol_table<UnaryOp>.map<'-'>(UnaryOp::Neg).map<'!'>(UnaryOp::Not).map<'~'>(UnaryOp::BitNot);

struct PUnaryOp {
  let name = "unary operator";
  let rule = dsl::symbol<unary_op_table>;
  let value = lexy::forward<UnaryOp>;
};

struct PUnaryOps {
  let name = "unary operators";
  let rule = dsl::list(dsl::p<PUnaryOp>);
  let value = lexy::as_list<std::vector<UnaryOp>>;
};

struct PUnaryExpr {
  let name = "unary expression";
  let rule = dsl::opt(dsl::p<PUnaryOps>) + dsl::p<PPrimaryExpr>;
  let value = lexy::callback<UnaryExpr>(
      [](lexy::nullopt, PrimaryExpr p) { return UnaryExpr{{}, std::move(p)}; },
      [](std::vector<UnaryOp> ops, PrimaryExpr p) {
        return UnaryExpr{std::move(ops), std::move(p)};
      });
};

let mul_op_table =
    lexy::symbol_table<MulOp>.map<'*'>(MulOp::Mul).map<'/'>(MulOp::Div).map<'%'>(MulOp::Mod);

struct PMulOp {
  let name = "multiplicative operator";
  let rule = dsl::symbol<mul_op_table>;
  let value = lexy::forward<MulOp>;
};
struct PMulRhs {
  let name = "multiplicative expression";
  let rule = dsl::p<PMulOp> >> dsl::p<PUnaryExpr>;
  let value = lexy::construct<MulRhs>;
};
struct PMulRhsList {
  let name = "multiplicative expression";
  let rule = dsl::list(dsl::p<PMulRhs>);
  let value = lexy::as_list<std::vector<MulRhs>>;
};
struct PMulExpr {
  let name = "multiplicative expression";
  let rule = dsl::p<PUnaryExpr> + dsl::opt(dsl::p<PMulRhsList>);
  let value = lexy::callback<MulExpr>(
      [](UnaryExpr lhs, lexy::nullopt) { return MulExpr{std::move(lhs), {}}; },
      [](UnaryExpr lhs, std::vector<MulRhs> rhs) {
        return MulExpr{std::move(lhs), std::move(rhs)};
      });
};

let add_op_table =
    lexy::symbol_table<AddOp>.map<'+'>(AddOp::Add).map<'-'>(AddOp::Sub);

struct PAddOp {
  let name = "additive operator";
  let rule = dsl::symbol<add_op_table>;
  let value = lexy::forward<AddOp>;
};
struct PAddRhs {
  let name = "additive expression";
  let rule = dsl::p<PAddOp> >> dsl::p<PMulExpr>;
  let value = lexy::construct<AddRhs>;
};
struct PAddRhsList {
  let name = "additive expression";
  let rule = dsl::list(dsl::p<PAddRhs>);
  let value = lexy::as_list<std::vector<AddRhs>>;
};
struct PAddExpr {
  let name = "additive expression";
  let rule = dsl::p<PMulExpr> + dsl::opt(dsl::p<PAddRhsList>);
  let value = lexy::callback<AddExpr>(
      [](MulExpr lhs, lexy::nullopt) { return AddExpr{std::move(lhs), {}}; },
      [](MulExpr lhs, std::vector<AddRhs> rhs) {
        return AddExpr{std::move(lhs), std::move(rhs)};
      });
};

let shift_op_table =
    lexy::symbol_table<ShiftOp>.map<LEXY_SYMBOL("<<")>(ShiftOp::Shl).map<LEXY_SYMBOL(">>")>(ShiftOp::Shr);

struct PShiftOp {
  let name = "shift operator";
  let rule = dsl::symbol<shift_op_table>;
  let value = lexy::forward<ShiftOp>;
};
struct PShiftRhs {
  let name = "shift expression";
  let rule = dsl::p<PShiftOp> >> dsl::p<PAddExpr>;
  let value = lexy::construct<ShiftRhs>;
};
struct PShiftRhsList {
  let name = "shift expression";
  let rule = dsl::list(dsl::p<PShiftRhs>);
  let value = lexy::as_list<std::vector<ShiftRhs>>;
};
struct PShiftExpr {
  let name = "shift expression";
  let rule = dsl::p<PAddExpr> + dsl::opt(dsl::p<PShiftRhsList>);
  let value = lexy::callback<ShiftExpr>(
      [](AddExpr lhs, lexy::nullopt) { return ShiftExpr{std::move(lhs), {}}; },
      [](AddExpr lhs, std::vector<ShiftRhs> rhs) {
        return ShiftExpr{std::move(lhs), std::move(rhs)};
      });
};

let cmp_op_table = lexy::symbol_table<CmpOp>
                       .map<LEXY_SYMBOL("<=")>(CmpOp::Le)
                       .map<LEXY_SYMBOL(">=")>(CmpOp::Ge)
                       .map<'<'>(CmpOp::Lt)
                       .map<'>'>(CmpOp::Gt);

struct PCmpOp {
  let name = "comparison operator";
  let rule = dsl::symbol<cmp_op_table>;
  let value = lexy::forward<CmpOp>;
};
struct PCmpRhs {
  let name = "comparison expression";
  let rule = dsl::p<PCmpOp> >> dsl::p<PShiftExpr>;
  let value = lexy::construct<CmpRhs>;
};
struct PCmpRhsList {
  let name = "comparison expression";
  let rule = dsl::list(dsl::p<PCmpRhs>);
  let value = lexy::as_list<std::vector<CmpRhs>>;
};
struct PCmpExpr {
  let name = "comparison expression";
  let rule = dsl::p<PShiftExpr> + dsl::opt(dsl::p<PCmpRhsList>);
  let value = lexy::callback<CmpExpr>(
      [](ShiftExpr lhs, lexy::nullopt) { return CmpExpr{std::move(lhs), {}}; },
      [](ShiftExpr lhs, std::vector<CmpRhs> rhs) {
        return CmpExpr{std::move(lhs), std::move(rhs)};
      });
};

let eq_op_table =
    lexy::symbol_table<EqOp>.map<LEXY_SYMBOL("==")>(EqOp::Eq).map<LEXY_SYMBOL("!=")>(
        EqOp::Ne);

struct PEqOp {
  let name = "equality operator";
  let rule = dsl::symbol<eq_op_table>;
  let value = lexy::forward<EqOp>;
};
struct PEqRhs {
  let name = "equality expression";
  let rule = dsl::p<PEqOp> >> dsl::p<PCmpExpr>;
  let value = lexy::construct<EqRhs>;
};
struct PEqRhsList {
  let name = "equality expression";
  let rule = dsl::list(dsl::p<PEqRhs>);
  let value = lexy::as_list<std::vector<EqRhs>>;
};
struct PEqExpr {
  let name = "equality expression";
  let rule = dsl::p<PCmpExpr> + dsl::opt(dsl::p<PEqRhsList>);
  let value = lexy::callback<EqExpr>(
      [](CmpExpr lhs, lexy::nullopt) { return EqExpr{std::move(lhs), {}}; },
      [](CmpExpr lhs, std::vector<EqRhs> rhs) {
        return EqExpr{std::move(lhs), std::move(rhs)};
      });
};

struct PBitAndRhsList {
  let name = "bitwise AND expression";
  let rule = dsl::list(dsl::lit_c<'&'> >> dsl::p<PEqExpr>);
  let value = lexy::as_list<std::vector<EqExpr>>;
};
struct PBitAndExpr {
  let name = "bitwise AND expression";
  let rule = dsl::p<PEqExpr> + dsl::opt(dsl::p<PBitAndRhsList>);
  let value = lexy::callback<BitAndExpr>(
      [](EqExpr lhs, lexy::nullopt) { return BitAndExpr{std::move(lhs), {}}; },
      [](EqExpr lhs, std::vector<EqExpr> rhs) {
        return BitAndExpr{std::move(lhs), std::move(rhs)};
      });
};

struct PBitXorRhsList {
  let name = "bitwise XOR expression";
  let rule = dsl::list(dsl::lit_c<'^'> >> dsl::p<PBitAndExpr>);
  let value = lexy::as_list<std::vector<BitAndExpr>>;
};
struct PBitXorExpr {
  let name = "bitwise XOR expression";
  let rule = dsl::p<PBitAndExpr> + dsl::opt(dsl::p<PBitXorRhsList>);
  let value = lexy::callback<BitXorExpr>(
      [](BitAndExpr lhs, lexy::nullopt) {
        return BitXorExpr{std::move(lhs), {}};
      },
      [](BitAndExpr lhs, std::vector<BitAndExpr> rhs) {
        return BitXorExpr{std::move(lhs), std::move(rhs)};
      });
};

struct PBitOrRhsList {
  let name = "bitwise OR expression";
  let rule = dsl::list(dsl::lit_c<'|'> >> dsl::p<PBitXorExpr>);
  let value = lexy::as_list<std::vector<BitXorExpr>>;
};
struct PBitOrExpr {
  let name = "bitwise OR expression";
  let rule = dsl::p<PBitXorExpr> + dsl::opt(dsl::p<PBitOrRhsList>);
  let value = lexy::callback<BitOrExpr>(
      [](BitXorExpr lhs, lexy::nullopt) {
        return BitOrExpr{std::move(lhs), {}};
      },
      [](BitXorExpr lhs, std::vector<BitXorExpr> rhs) {
        return BitOrExpr{std::move(lhs), std::move(rhs)};
      });
};

struct PIfExpr {
  let name = "if expression";
  let rule = LEXY_KEYWORD("if", keyword) >>
             dsl::recurse<PExpr> +
                 LEXY_KEYWORD("then", keyword) + dsl::recurse<PExpr> +
                 LEXY_KEYWORD("else", keyword) + dsl::recurse<PExpr>;
  let value = lexy::callback<IfExpr>([](Expr c, Expr t, Expr e) {
    return IfExpr{std::make_unique<Expr>(std::move(c)),
                  std::make_unique<Expr>(std::move(t)),
                  std::make_unique<Expr>(std::move(e))};
  });
};

struct PValueExpr {
  let name = "expression";
  let rule = dsl::p<PIfExpr> | (dsl::else_ >> dsl::p<PBitOrExpr>);
  let value = lexy::construct<ValueExpr>;
};

struct PFuncDecl {
  let name = "function declaration";
  let rule = dsl::p<PIdent> + dsl::opt(dsl::p<PParamList>) + dsl::lit_c<':'> +
             dsl::p<PType> + dsl::lit_c<'='> + dsl::recurse<PExpr>;
  let value = lexy::callback<FuncDecl>(
      [](Ident id, std::optional<ParamList> params, Type ret, Expr body) {
        return FuncDecl{std::move(id), std::move(params).value_or(ParamList{}),
                        std::move(ret),
                        std::make_unique<Expr>(std::move(body))};
      });
};

struct PMutExpr {
  let name = "mutation expression";
  let rule = dsl::p<PSubscript> + LEXY_LIT("*=") + dsl::p<PValueExpr>;
  let value = lexy::construct<MutExpr>;
};

struct POptNextExpr {
  let name = "next expression";
  let rule = (dsl::peek(dsl::lit_c<';'>) >> dsl::nullopt) |
             (dsl::peek(dsl::lit_c<')'>) >> dsl::nullopt) |
             (dsl::peek(dsl::lit_c<']'>) >> dsl::nullopt) |
             (dsl::peek(LEXY_KEYWORD("then", keyword)) >> dsl::nullopt) |
             (dsl::peek(LEXY_KEYWORD("else", keyword)) >> dsl::nullopt) |
             (dsl::peek(dsl::eof) >> dsl::nullopt) |
             (dsl::else_ >> dsl::recurse<PExpr>);
  let value = lexy::callback<std::unique_ptr<Expr>>(
      [](lexy::nullopt) { return nullptr; },
      [](Expr next) { return std::make_unique<Expr>(std::move(next)); });
};

struct PAssignment {
  let name = "assignment";
  let rule = dsl::p<PIdent> + dsl::opt(dsl::lit_c<':'> >> dsl::p<PType>) +
             dsl::lit_c<'='> + dsl::p<PValueExpr> + dsl::p<POptNextExpr>;
  let value = lexy::construct<Assignment>;
};

struct PFuncDeclExpr {
  let name = "function declaration";
  let rule = dsl::p<PFuncDecl> + dsl::recurse<PExpr>;
  let value = lexy::callback<FuncDeclExpr>(
      [](FuncDecl&& decl, Expr&& next) -> FuncDeclExpr {
        return FuncDeclExpr{std::move(decl),
                            std::make_unique<Expr>(std::move(next))};
      });
};

struct PBindExpr {
  let name = "binding expression";
  let rule = [] {
    let not_rparen = dsl::ascii::character - dsl::lit_c<')'>;
    let inner_parens =
        dsl::lit_c<'('> >> dsl::while_(not_rparen) + dsl::lit_c<')'>;
    let paren_span = dsl::lit_c<'('> >>
                     dsl::while_(inner_parens | not_rparen) + dsl::lit_c<')'>;

    let not_rbracket = dsl::ascii::character - dsl::lit_c<']'>;
    let inner_brackets =
        dsl::lit_c<'['> >> dsl::while_(not_rbracket) + dsl::lit_c<']'>;
    let bracket_span =
        dsl::lit_c<'['> >>
        dsl::while_(inner_brackets | not_rbracket) + dsl::lit_c<']'>;

    auto is_func_decl = dsl::peek(
        dsl::token(identTok + ws + paren_span + ws + dsl::lit_c<':'>));
    auto is_typed_assign =
        dsl::peek(dsl::token(identTok + ws + dsl::lit_c<':'>));
    auto is_untyped_assign =
        dsl::peek(dsl::token(identTok + ws + dsl::lit_c<'='>));
    auto is_mut = dsl::peek(
        dsl::token(dsl::opt(modIdentTok >> ws + dsl::lit_c<'.'> + ws) +
                   identTok + ws + bracket_span + ws + LEXY_LIT("*=")));

    return (is_func_decl >> dsl::p<PFuncDeclExpr>) |
           (is_typed_assign >> dsl::p<PAssignment>) |
           (is_untyped_assign >> dsl::p<PAssignment>) |
           (is_mut >> dsl::p<PMutExpr>) | (dsl::else_ >> dsl::p<PValueExpr>);
  }();
  let value = lexy::construct<BindExpr>;
};

struct PBindList {
  let name = "expression list";
  let rule = dsl::list(dsl::p<PBindExpr>, dsl::sep(dsl::lit_c<';'>));
  let value = lexy::as_list<std::vector<BindExpr>>;
};

struct PExpr {
  let name = "expression";
  let rule = dsl::p<PBindList>;
  let value = lexy::construct<Expr>;
};

struct PTopDecl {
  let name = "top-level declaration";
  let rule = [] {
    auto is_func = dsl::peek(dsl::token(identTok + ws + dsl::lit_c<'('>)) |
                   dsl::peek(dsl::token(identTok + ws + dsl::lit_c<':'> + ws +
                                        LEXY_LIT("()")));
    return (is_func >> dsl::p<PFuncDecl>) |
           (dsl::else_ >>
            (dsl::p<PIdent> + dsl::opt(dsl::lit_c<':'> >> dsl::p<PType>) +
             dsl::lit_c<'='> + dsl::p<PValueExpr>));
  }();

  let value = lexy::callback<TopDecl>(
      [](FuncDecl fn) -> TopDecl { return std::move(fn); },
      [](Ident id, std::optional<Type> ty, ValueExpr val) -> TopDecl {
        return ValDecl{std::move(id), std::move(ty), std::move(val)};
      });
};

struct PProgram {
  let name = "program";
  let whitespace = dsl::ascii::space;
  let rule = dsl::terminator(dsl::eof).opt_list(dsl::p<PTopDecl>);
  let value = lexy::as_list<Program>;
};

#undef let
}  // namespace grammar

struct ErrorCollector {
  std::vector<SkibError>* errors;

  struct error_sink {
    std::vector<SkibError>* errors;

    using return_type = std::size_t;

    template <typename Input, typename Reader, typename Tag>
    void operator()(const lexy::error_context<Input>& context,
                    const lexy::error<Reader, Tag>& error) {
      auto loc = lexy::get_input_location(context.input(), error.position());

      std::string msg;
      auto formatter = lexy_ext::report_error.to(std::back_inserter(msg));
      formatter.sink()(context, error);

      if (msg.starts_with("error: ")) {
        msg.erase(0, 7);
      }

      errors->push_back(SkibError{
          .line = loc.line_nr(),
          .column = loc.column_nr(),
          .message = std::move(msg),
      });
    }

    std::size_t finish() && { return errors->size(); }
  };

  error_sink sink() const { return {errors}; }
};

class Parser {
  std::expected<Program, std::vector<SkibError>> parseBuffer(auto buffer) {
    std::vector<SkibError> errors;
    auto result =
        lexy::parse<grammar::PProgram>(buffer, ErrorCollector{&errors});
    if (!result.has_value() || result.error_count() != 0)
      return std::unexpected(std::move(errors));

    return std::move(result).value();
  }

 public:
  std::expected<Program, std::vector<SkibError>> parse(const char* path) {
    auto file = lexy::read_file(path);
    if (!file)
      throw std::runtime_error(std::format("failed to open file: {}", path));
    return parseBuffer(file.buffer());
  }

  std::expected<Program, std::vector<SkibError>> parseString(std::string str) {
    return parseBuffer(lexy::string_input(str));
  }
};
