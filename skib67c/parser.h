#pragma once

#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ast.h"
#include "error.h"

namespace parser {

enum class TokenKind {
  Eof,
  Ident,
  ModIdent,
  IntLit,
  UintLit,
  HexLit,
  FloatLit,

  // Keywords
  KwIf,
  KwThen,
  KwElse,
  KwTrue,
  KwFalse,
  KwInt,
  KwUint,
  KwFloat,
  KwBool,
  KwArray,
  KwArrayAlloc,

  // Punctuation & Operators
  LParen,
  RParen,
  LBracket,
  RBracket,
  Colon,
  Assign,     // =
  MutAssign,  // *=
  Semi,
  Comma,
  Dot,

  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Bang,
  Tilde,

  Eq,   // ==
  Ne,   // !=
  Lt,   // <
  Le,   // <=
  Gt,   // >
  Ge,   // >=
  Shl,  // <<
  Shr,  // >>

  Amp,    // &
  Caret,  // ^
  Pipe,   // |
};

struct Token {
  TokenKind kind;
  std::string_view text;
  Position pos;
};

class Lexer {
  std::string_view src;
  std::size_t idx = 0;
  std::size_t line = 1;
  std::size_t col = 1;
  std::vector<SkibError> errors;

  bool eof() const { return idx >= src.size(); }
  char peek(std::size_t offset = 0) const {
    return (idx + offset < src.size()) ? src[idx + offset] : '\0';
  }

  char advance() {
    char c = src[idx++];
    if (c == '\n') {
      line++;
      col = 1;
    } else {
      col++;
    }
    return c;
  }

  Position currentPos() const { return {line, col}; }

  void skipWhitespace() {
    while (!eof() && std::isspace(static_cast<unsigned char>(peek()))) {
      advance();
    }
  }

 public:
  explicit Lexer(std::string_view source) : src(source) {}

  std::expected<std::vector<Token>, std::vector<SkibError>> tokenize() {
    std::vector<Token> tokens;

    while (true) {
      skipWhitespace();
      Position pos = currentPos();
      if (eof()) {
        tokens.push_back({TokenKind::Eof, "", pos});
        break;
      }

      std::size_t start = idx;
      char c = peek();

      if (std::islower(static_cast<unsigned char>(c)) || c == '_') {
        while (!eof() && (std::isalnum(static_cast<unsigned char>(peek())) ||
                          peek() == '_')) {
          advance();
        }
        std::string_view text = src.substr(start, idx - start);
        TokenKind kind = TokenKind::Ident;
        if (text == "if")
          kind = TokenKind::KwIf;
        else if (text == "then")
          kind = TokenKind::KwThen;
        else if (text == "else")
          kind = TokenKind::KwElse;
        else if (text == "array")
          kind = TokenKind::KwArrayAlloc;
        tokens.push_back({kind, text, pos});
        continue;
      }

      if (std::isupper(static_cast<unsigned char>(c))) {
        while (!eof() && std::isalnum(static_cast<unsigned char>(peek()))) {
          advance();
        }
        std::string_view text = src.substr(start, idx - start);
        TokenKind kind = TokenKind::ModIdent;
        if (text == "True")
          kind = TokenKind::KwTrue;
        else if (text == "False")
          kind = TokenKind::KwFalse;
        else if (text == "Int")
          kind = TokenKind::KwInt;
        else if (text == "Uint")
          kind = TokenKind::KwUint;
        else if (text == "Float")
          kind = TokenKind::KwFloat;
        else if (text == "Bool")
          kind = TokenKind::KwBool;
        else if (text == "Array")
          kind = TokenKind::KwArray;
        tokens.push_back({kind, text, pos});
        continue;
      }

      if (std::isdigit(static_cast<unsigned char>(c))) {
        if (c == '0' && peek(1) == 'x') {
          advance();
          advance();
          std::size_t hexStart = idx;
          while (!eof() && std::isxdigit(static_cast<unsigned char>(peek()))) {
            advance();
          }
          if (idx == hexStart) {
            errors.push_back(
                {pos.line, pos.column, "expected hex digits after '0x'"});
          } else {
            tokens.push_back(
                {TokenKind::HexLit, src.substr(start, idx - start), pos});
          }
        } else {
          while (!eof() && std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
          }
          if (peek() == '.' &&
              std::isdigit(static_cast<unsigned char>(peek(1)))) {
            advance();  // '.'
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek()))) {
              advance();
            }
            tokens.push_back(
                {TokenKind::FloatLit, src.substr(start, idx - start), pos});
          } else if (peek() == 'u' || peek() == 'U') {
            advance();
            tokens.push_back(
                {TokenKind::UintLit, src.substr(start, idx - start), pos});
          } else {
            tokens.push_back(
                {TokenKind::IntLit, src.substr(start, idx - start), pos});
          }
        }
        continue;
      }

      auto match2 = [&](char a, char b, TokenKind kind) -> bool {
        if (peek() == a && peek(1) == b) {
          advance();
          advance();
          tokens.push_back({kind, src.substr(start, 2), pos});
          return true;
        }
        return false;
      };

      if (match2('*', '=', TokenKind::MutAssign) ||
          match2('=', '=', TokenKind::Eq) || match2('!', '=', TokenKind::Ne) ||
          match2('<', '=', TokenKind::Le) || match2('>', '=', TokenKind::Ge) ||
          match2('<', '<', TokenKind::Shl) ||
          match2('>', '>', TokenKind::Shr)) {
        continue;
      }

      advance();
      std::string_view text = src.substr(start, 1);
      switch (c) {
        case '(':
          tokens.push_back({TokenKind::LParen, text, pos});
          break;
        case ')':
          tokens.push_back({TokenKind::RParen, text, pos});
          break;
        case '[':
          tokens.push_back({TokenKind::LBracket, text, pos});
          break;
        case ']':
          tokens.push_back({TokenKind::RBracket, text, pos});
          break;
        case ':':
          tokens.push_back({TokenKind::Colon, text, pos});
          break;
        case '=':
          tokens.push_back({TokenKind::Assign, text, pos});
          break;
        case ';':
          tokens.push_back({TokenKind::Semi, text, pos});
          break;
        case ',':
          tokens.push_back({TokenKind::Comma, text, pos});
          break;
        case '.':
          tokens.push_back({TokenKind::Dot, text, pos});
          break;
        case '+':
          tokens.push_back({TokenKind::Plus, text, pos});
          break;
        case '-':
          tokens.push_back({TokenKind::Minus, text, pos});
          break;
        case '*':
          tokens.push_back({TokenKind::Star, text, pos});
          break;
        case '/':
          tokens.push_back({TokenKind::Slash, text, pos});
          break;
        case '%':
          tokens.push_back({TokenKind::Percent, text, pos});
          break;
        case '!':
          tokens.push_back({TokenKind::Bang, text, pos});
          break;
        case '~':
          tokens.push_back({TokenKind::Tilde, text, pos});
          break;
        case '<':
          tokens.push_back({TokenKind::Lt, text, pos});
          break;
        case '>':
          tokens.push_back({TokenKind::Gt, text, pos});
          break;
        case '&':
          tokens.push_back({TokenKind::Amp, text, pos});
          break;
        case '^':
          tokens.push_back({TokenKind::Caret, text, pos});
          break;
        case '|':
          tokens.push_back({TokenKind::Pipe, text, pos});
          break;
        default:
          errors.push_back({pos.line, pos.column,
                            std::format("unexpected character '{}'", c)});
          break;
      }
    }

    if (!errors.empty()) return std::unexpected(std::move(errors));
    return tokens;
  }
};

class RecursiveDescentParser {
  std::vector<Token> tokens;
  std::size_t pos = 0;
  std::vector<SkibError> errors;

  const Token& peek(std::size_t offset = 0) const {
    if (pos + offset >= tokens.size()) return tokens.back();
    return tokens[pos + offset];
  }

  const Token& advance() {
    const Token& tok = peek();
    if (pos + 1 < tokens.size()) pos++;
    return tok;
  }

  bool check(TokenKind kind, std::size_t offset = 0) const {
    return peek(offset).kind == kind;
  }

  bool match(TokenKind kind) {
    if (check(kind)) {
      advance();
      return true;
    }
    return false;
  }

  std::optional<Token> expect(TokenKind kind, std::string_view expectedDesc) {
    if (check(kind)) return advance();
    const auto& tok = peek();
    errors.push_back({
        tok.pos.line,
        tok.pos.column,
        std::format("expected {}, got '{}'", expectedDesc,
                    tok.kind == TokenKind::Eof ? "EOF" : tok.text),
    });
    return std::nullopt;
  }

  std::optional<std::size_t> skipBalanced(std::size_t startIdx, TokenKind open,
                                          TokenKind close) const {
    if (startIdx >= tokens.size() || tokens[startIdx].kind != open)
      return std::nullopt;
    int depth = 0;
    for (std::size_t i = startIdx; i < tokens.size(); ++i) {
      if (tokens[i].kind == open)
        depth++;
      else if (tokens[i].kind == close) {
        depth--;
        if (depth == 0) return i + 1;
      }
    }
    return std::nullopt;
  }

  bool isFuncDeclLookahead() const {
    if (!check(TokenKind::Ident)) return false;
    if (check(TokenKind::LParen, 1)) {
      auto afterParen =
          skipBalanced(pos + 1, TokenKind::LParen, TokenKind::RParen);
      return afterParen && *afterParen < tokens.size() &&
             tokens[*afterParen].kind == TokenKind::Colon;
    }
    if (check(TokenKind::Colon, 1) && check(TokenKind::LParen, 2) &&
        check(TokenKind::RParen, 3) && check(TokenKind::Assign, 4)) {
      return true;
    }
    return false;
  }

  bool isAssignmentLookahead() const {
    if (!check(TokenKind::Ident)) return false;
    return check(TokenKind::Assign, 1) || check(TokenKind::Colon, 1);
  }

  bool isMutExprLookahead() const {
    std::size_t i = pos;
    if (tokens[i].kind == TokenKind::ModIdent && i + 1 < tokens.size() &&
        tokens[i + 1].kind == TokenKind::Dot) {
      i += 2;
    }
    if (i >= tokens.size() || tokens[i].kind != TokenKind::Ident) return false;
    i++;
    auto afterBracket =
        skipBalanced(i, TokenKind::LBracket, TokenKind::RBracket);
    return afterBracket && *afterBracket < tokens.size() &&
           tokens[*afterBracket].kind == TokenKind::MutAssign;
  }

  bool isContinuationStop() const {
    switch (peek().kind) {
      case TokenKind::Semi:
      case TokenKind::RParen:
      case TokenKind::RBracket:
      case TokenKind::Comma:
      case TokenKind::KwThen:
      case TokenKind::KwElse:
      case TokenKind::Eof:
        return true;
      default:
        return false;
    }
  }

  std::optional<Ident> parseIdent() {
    auto tok = expect(TokenKind::Ident, "identifier");
    if (!tok) return std::nullopt;
    return Ident{std::string(tok->text), tok->pos};
  }

  std::optional<QualIdent> parseQualIdent() {
    std::optional<ModIdent> mod;
    if (check(TokenKind::ModIdent) && check(TokenKind::Dot, 1)) {
      Token modTok = advance();
      advance();  // '.'
      mod = ModIdent{std::string(modTok.text), modTok.pos};
    }
    auto id = parseIdent();
    if (!id) return std::nullopt;
    return QualIdent{std::move(mod), std::move(*id)};
  }

  std::optional<ScalarType> parseScalarType() {
    const Token& tok = peek();
    switch (tok.kind) {
      case TokenKind::KwInt:
        advance();
        return Int{tok.pos};
      case TokenKind::KwUint:
        advance();
        return Uint{tok.pos};
      case TokenKind::KwFloat:
        advance();
        return Float{tok.pos};
      case TokenKind::KwBool:
        advance();
        return Bool{tok.pos};
      default:
        errors.push_back(
            {tok.pos.line, tok.pos.column, "expected scalar type"});
        return std::nullopt;
    }
  }

  std::optional<Type> parseType() {
    const Token& tok = peek();
    if (check(TokenKind::LParen) && check(TokenKind::RParen, 1)) {
      advance();
      advance();
      return Skib{tok.pos};
    }
    if (check(TokenKind::KwArray)) {
      Position arrPos = advance().pos;
      if (!expect(TokenKind::LBracket, "'['")) return std::nullopt;
      auto scalar = parseScalarType();
      if (!scalar) return std::nullopt;
      if (!expect(TokenKind::RBracket, "']'")) return std::nullopt;
      return ArrayType{arrPos, std::move(*scalar)};
    }
    auto scalar = parseScalarType();
    if (!scalar) return std::nullopt;
    return Type{std::move(*scalar)};
  }

  std::optional<ParamList> parseParamList() {
    if (!expect(TokenKind::LParen, "'('")) return std::nullopt;
    ParamList params;
    if (!check(TokenKind::RParen)) {
      do {
        Position pPos = peek().pos;
        auto id = parseIdent();
        if (!id) return std::nullopt;
        if (!expect(TokenKind::Colon, "':'")) return std::nullopt;
        auto ty = parseType();
        if (!ty) return std::nullopt;
        params.push_back(Param{pPos, std::move(*id), std::move(*ty)});
      } while (match(TokenKind::Comma));
    }
    if (!expect(TokenKind::RParen, "')'")) return std::nullopt;
    return params;
  }

  std::optional<FuncDecl> parseFuncDecl() {
    Position fnPos = peek().pos;
    auto id = parseIdent();
    if (!id) return std::nullopt;

    ParamList params;
    if (check(TokenKind::LParen)) {
      auto p = parseParamList();
      if (!p) return std::nullopt;
      params = std::move(*p);
    }

    if (!expect(TokenKind::Colon, "':'")) return std::nullopt;
    auto retType = parseType();
    if (!retType) return std::nullopt;
    if (!expect(TokenKind::Assign, "'='")) return std::nullopt;
    auto body = parseExpr();
    if (!body) return std::nullopt;

    return FuncDecl{fnPos, std::move(*id), std::move(params),
                    std::move(*retType),
                    std::make_unique<Expr>(std::move(*body))};
  }

  std::optional<ValDecl> parseValDecl() {
    auto id = parseIdent();
    if (!id) return std::nullopt;

    std::optional<Type> ty;
    if (match(TokenKind::Colon)) {
      ty = parseType();
      if (!ty) return std::nullopt;
    }

    if (!expect(TokenKind::Assign, "'='")) return std::nullopt;
    auto val = parseValueExpr();
    if (!val) return std::nullopt;

    return ValDecl{std::move(*id), std::move(ty), std::move(*val)};
  }

  std::optional<TopDecl> parseTopDecl() {
    if (isFuncDeclLookahead()) {
      auto fn = parseFuncDecl();
      if (!fn) return std::nullopt;
      return TopDecl{std::move(*fn)};
    }
    auto val = parseValDecl();
    if (!val) return std::nullopt;
    return TopDecl{std::move(*val)};
  }

  std::optional<Expr> parseExpr() {
    std::vector<BindExpr> binds;
    auto first = parseBindExpr();
    if (!first) return std::nullopt;
    binds.push_back(std::move(*first));

    while (match(TokenKind::Semi)) {
      auto next = parseBindExpr();
      if (!next) return std::nullopt;
      binds.push_back(std::move(*next));
    }
    return Expr{std::move(binds)};
  }

  std::optional<BindExpr> parseBindExpr() {
    if (isFuncDeclLookahead()) {
      Position fnPos = peek().pos;
      auto decl = parseFuncDecl();
      if (!decl) return std::nullopt;

      auto cont = parseExpr();
      if (!cont) return std::nullopt;
      return BindExpr{FuncDeclExpr{fnPos, std::move(*decl),
                                   std::make_unique<Expr>(std::move(*cont))}};
    }

    if (isAssignmentLookahead()) {
      auto id = parseIdent();
      if (!id) return std::nullopt;

      std::optional<Type> ty;
      if (match(TokenKind::Colon)) {
        ty = parseType();
        if (!ty) return std::nullopt;
      }

      if (!expect(TokenKind::Assign, "'='")) return std::nullopt;
      auto val = parseValueExpr();
      if (!val) return std::nullopt;

      std::unique_ptr<Expr> next;
      if (!isContinuationStop()) {
        auto cont = parseExpr();
        if (!cont) return std::nullopt;
        next = std::make_unique<Expr>(std::move(*cont));
      }
      return BindExpr{Assignment{std::move(*id), std::move(ty), std::move(*val),
                                 std::move(next)}};
    }

    if (isMutExprLookahead()) {
      auto qid = parseQualIdent();
      if (!qid) return std::nullopt;
      if (!expect(TokenKind::LBracket, "'['")) return std::nullopt;
      auto idxExpr = parseExpr();
      if (!idxExpr) return std::nullopt;
      if (!expect(TokenKind::RBracket, "']'")) return std::nullopt;
      if (!expect(TokenKind::MutAssign, "'*='")) return std::nullopt;
      auto val = parseValueExpr();
      if (!val) return std::nullopt;

      Subscript sub{std::move(*qid),
                    std::make_unique<Expr>(std::move(*idxExpr))};
      return BindExpr{MutExpr{std::move(sub), std::move(*val)}};
    }

    auto val = parseValueExpr();
    if (!val) return std::nullopt;
    return BindExpr{std::move(*val)};
  }

  std::optional<ValueExpr> parseValueExpr() {
    Position startPos = peek().pos;
    if (match(TokenKind::KwIf)) {
      auto cond = parseExpr();
      if (!cond) return std::nullopt;
      if (!expect(TokenKind::KwThen, "'then'")) return std::nullopt;
      auto thenBranch = parseExpr();
      if (!thenBranch) return std::nullopt;
      if (!expect(TokenKind::KwElse, "'else'")) return std::nullopt;
      auto elseBranch = parseExpr();
      if (!elseBranch) return std::nullopt;

      return ValueExpr{startPos,
                       IfExpr{
                           std::make_unique<Expr>(std::move(*cond)),
                           std::make_unique<Expr>(std::move(*thenBranch)),
                           std::make_unique<Expr>(std::move(*elseBranch)),
                       }};
    }

    auto bitOr = parseBitOr();
    if (!bitOr) return std::nullopt;
    return ValueExpr{startPos, std::move(*bitOr)};
  }

  std::optional<BitOrExpr> parseBitOr() {
    auto left = parseBitXor();
    if (!left) return std::nullopt;
    std::vector<BitXorExpr> right;
    while (match(TokenKind::Pipe)) {
      auto rhs = parseBitXor();
      if (!rhs) return std::nullopt;
      right.push_back(std::move(*rhs));
    }
    return BitOrExpr{std::move(*left), std::move(right)};
  }

  std::optional<BitXorExpr> parseBitXor() {
    auto left = parseBitAnd();
    if (!left) return std::nullopt;
    std::vector<BitAndExpr> right;
    while (match(TokenKind::Caret)) {
      auto rhs = parseBitAnd();
      if (!rhs) return std::nullopt;
      right.push_back(std::move(*rhs));
    }
    return BitXorExpr{std::move(*left), std::move(right)};
  }

  std::optional<BitAndExpr> parseBitAnd() {
    auto left = parseEq();
    if (!left) return std::nullopt;
    std::vector<EqExpr> right;
    while (match(TokenKind::Amp)) {
      auto rhs = parseEq();
      if (!rhs) return std::nullopt;
      right.push_back(std::move(*rhs));
    }
    return BitAndExpr{std::move(*left), std::move(right)};
  }

  std::optional<EqExpr> parseEq() {
    auto left = parseCmp();
    if (!left) return std::nullopt;
    std::vector<EqRhs> right;
    while (check(TokenKind::Eq) || check(TokenKind::Ne)) {
      EqOp op = (advance().kind == TokenKind::Eq) ? EqOp::Eq : EqOp::Ne;
      auto rhs = parseCmp();
      if (!rhs) return std::nullopt;
      right.push_back(EqRhs{op, std::move(*rhs)});
    }
    return EqExpr{std::move(*left), std::move(right)};
  }

  std::optional<CmpExpr> parseCmp() {
    auto left = parseShift();
    if (!left) return std::nullopt;
    std::vector<CmpRhs> right;
    while (check(TokenKind::Lt) || check(TokenKind::Le) ||
           check(TokenKind::Gt) || check(TokenKind::Ge)) {
      Token opTok = advance();
      CmpOp op = CmpOp::Lt;
      if (opTok.kind == TokenKind::Le)
        op = CmpOp::Le;
      else if (opTok.kind == TokenKind::Gt)
        op = CmpOp::Gt;
      else if (opTok.kind == TokenKind::Ge)
        op = CmpOp::Ge;

      auto rhs = parseShift();
      if (!rhs) return std::nullopt;
      right.push_back(CmpRhs{opTok.pos, op, std::move(*rhs)});
    }
    return CmpExpr{std::move(*left), std::move(right)};
  }

  std::optional<ShiftExpr> parseShift() {
    auto left = parseAdd();
    if (!left) return std::nullopt;
    std::vector<ShiftRhs> right;
    while (check(TokenKind::Shl) || check(TokenKind::Shr)) {
      Token opTok = advance();
      ShiftOp op = (opTok.kind == TokenKind::Shl) ? ShiftOp::Shl : ShiftOp::Shr;
      auto rhs = parseAdd();
      if (!rhs) return std::nullopt;
      right.push_back(ShiftRhs{opTok.pos, op, std::move(*rhs)});
    }
    return ShiftExpr{std::move(*left), std::move(right)};
  }

  std::optional<AddExpr> parseAdd() {
    Position startPos = peek().pos;
    auto left = parseMul();
    if (!left) return std::nullopt;
    std::vector<AddRhs> right;
    while (check(TokenKind::Plus) || check(TokenKind::Minus)) {
      Token opTok = advance();
      AddOp op = (opTok.kind == TokenKind::Plus) ? AddOp::Add : AddOp::Sub;
      auto rhs = parseMul();
      if (!rhs) return std::nullopt;
      right.push_back(AddRhs{opTok.pos, op, std::move(*rhs)});
    }
    return AddExpr{startPos, std::move(*left), std::move(right)};
  }

  std::optional<MulExpr> parseMul() {
    auto left = parseUnary();
    if (!left) return std::nullopt;
    std::vector<MulRhs> right;
    while (check(TokenKind::Star) || check(TokenKind::Slash) ||
           check(TokenKind::Percent)) {
      Token opTok = advance();
      MulOp op = MulOp::Mul;
      if (opTok.kind == TokenKind::Slash)
        op = MulOp::Div;
      else if (opTok.kind == TokenKind::Percent)
        op = MulOp::Mod;

      auto rhs = parseUnary();
      if (!rhs) return std::nullopt;
      right.push_back(MulRhs{opTok.pos, op, std::move(*rhs)});
    }
    return MulExpr{std::move(*left), std::move(right)};
  }

  std::optional<UnaryExpr> parseUnary() {
    Position startPos = peek().pos;
    std::vector<UnaryOp> ops;
    while (check(TokenKind::Minus) || check(TokenKind::Bang) ||
           check(TokenKind::Tilde)) {
      TokenKind k = advance().kind;
      if (k == TokenKind::Minus)
        ops.push_back(UnaryOp::Neg);
      else if (k == TokenKind::Bang)
        ops.push_back(UnaryOp::Not);
      else
        ops.push_back(UnaryOp::BitNot);
    }
    auto prim = parsePrimary();
    if (!prim) return std::nullopt;
    return UnaryExpr{startPos, std::move(ops), std::move(*prim)};
  }

  std::optional<PrimaryExpr> parsePrimary() {
    const Token& tok = peek();

    if (check(TokenKind::LParen)) {
      if (check(TokenKind::RParen, 1)) {
        Position skibPos = advance().pos;
        advance();
        return PrimaryExpr{Literal{Skib{skibPos}}};
      }
      advance();  // '('
      auto inner = parseExpr();
      if (!inner) return std::nullopt;
      if (!expect(TokenKind::RParen, "')'")) return std::nullopt;
      return PrimaryExpr{InParens{std::make_unique<Expr>(std::move(*inner))}};
    }

    // Array initializer: [ expr, expr, ... ]
    if (check(TokenKind::LBracket)) {
      Position arrPos = advance().pos;
      std::vector<Expr> elements;
      if (!check(TokenKind::RBracket)) {
        do {
          auto elem = parseExpr();
          if (!elem) return std::nullopt;
          elements.push_back(std::move(*elem));
        } while (match(TokenKind::Comma));
      }
      if (!expect(TokenKind::RBracket, "']'")) return std::nullopt;
      return PrimaryExpr{ArrayLit{arrPos, std::move(elements)}};
    }

    // Array allocation: array(size)
    if (check(TokenKind::KwArrayAlloc)) {
      Position arrPos = advance().pos;
      if (!expect(TokenKind::LParen, "'('")) return std::nullopt;
      auto sizeExpr = parseExpr();
      if (!sizeExpr) return std::nullopt;
      if (!expect(TokenKind::RParen, "')'")) return std::nullopt;
      return PrimaryExpr{
          ArrayAlloc{arrPos, std::make_unique<Expr>(std::move(*sizeExpr))}};
    }

    if (check(TokenKind::KwTrue)) {
      return PrimaryExpr{Literal{True{advance().pos}}};
    }
    if (check(TokenKind::KwFalse)) {
      return PrimaryExpr{Literal{False{advance().pos}}};
    }
    if (check(TokenKind::HexLit)) {
      Token t = advance();
      std::uint64_t val = 0;
      std::from_chars(t.text.data() + 2, t.text.data() + t.text.size(), val,
                      16);
      return PrimaryExpr{Literal{HexLit{t.pos, val}}};
    }
    if (check(TokenKind::IntLit)) {
      Token t = advance();
      std::int64_t val = 0;
      std::from_chars(t.text.data(), t.text.data() + t.text.size(), val, 10);
      return PrimaryExpr{Literal{IntLit{t.pos, val}}};
    }
    if (check(TokenKind::UintLit)) {
      Token t = advance();
      std::uint64_t val = 0;
      std::from_chars(t.text.data(), t.text.data() + t.text.size() - 1, val,
                      10);
      return PrimaryExpr{Literal{UintLit{t.pos, val}}};
    }
    if (check(TokenKind::FloatLit)) {
      Token t = advance();
      double val = 0.0;
      std::from_chars(t.text.data(), t.text.data() + t.text.size(), val);
      return PrimaryExpr{Literal{FloatLit{t.pos, val}}};
    }

    if (check(TokenKind::Ident) || check(TokenKind::ModIdent)) {
      auto qid = parseQualIdent();
      if (!qid) return std::nullopt;

      if (match(TokenKind::LParen)) {
        ArgsList args;
        if (!check(TokenKind::RParen)) {
          do {
            auto arg = parseExpr();
            if (!arg) return std::nullopt;
            args.push_back(std::move(*arg));
          } while (match(TokenKind::Comma));
        }
        if (!expect(TokenKind::RParen, "')'")) return std::nullopt;
        return PrimaryExpr{Call{std::move(*qid), std::move(args)}};
      }

      if (match(TokenKind::LBracket)) {
        auto idx = parseExpr();
        if (!idx) return std::nullopt;
        if (!expect(TokenKind::RBracket, "']'")) return std::nullopt;
        return PrimaryExpr{Subscript{std::move(*qid),
                                     std::make_unique<Expr>(std::move(*idx))}};
      }

      return PrimaryExpr{std::move(*qid)};
    }

    errors.push_back({
        tok.pos.line,
        tok.pos.column,
        std::format("unexpected token '{}' in primary expression",
                    tok.kind == TokenKind::Eof ? "EOF" : tok.text),
    });
    return std::nullopt;
  }

 public:
  explicit RecursiveDescentParser(std::vector<Token> toks)
      : tokens(std::move(toks)) {}

  std::expected<Program, std::vector<SkibError>> parseProgram() {
    Program prog;
    while (!check(TokenKind::Eof)) {
      auto decl = parseTopDecl();
      if (!decl) return std::unexpected(std::move(errors));
      prog.push_back(std::move(*decl));
    }
    if (!errors.empty()) return std::unexpected(std::move(errors));
    return prog;
  }
};

}  // namespace parser

class Parser {
 public:
  std::expected<Program, std::vector<SkibError>> parseString(
      std::string_view src) {
    parser::Lexer lexer(src);
    auto tokens = lexer.tokenize();
    if (!tokens) return std::unexpected(std::move(tokens.error()));

    parser::RecursiveDescentParser rd(std::move(*tokens));
    return rd.parseProgram();
  }

  std::expected<Program, std::vector<SkibError>> parse(const char* path) {
    std::ifstream file(path);
    if (!file) {
      throw std::runtime_error(std::format("failed to open file: {}", path));
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return parseString(ss.str());
  }
};
