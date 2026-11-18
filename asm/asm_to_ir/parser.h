#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <lexy/action/parse.hpp>
#include <lexy/callback/adapter.hpp>
#include <lexy/callback/container.hpp>
#include <lexy/callback/forward.hpp>
#include <lexy/callback/object.hpp>
#include <lexy/callback/string.hpp>
#include <lexy/dsl.hpp>
#include <lexy/dsl/ascii.hpp>
#include <lexy/dsl/capture.hpp>
#include <lexy/dsl/delimited.hpp>
#include <lexy/dsl/digit.hpp>
#include <lexy/dsl/identifier.hpp>
#include <lexy/dsl/integer.hpp>
#include <lexy/dsl/literal.hpp>
#include <lexy/dsl/loop.hpp>
#include <lexy/dsl/newline.hpp>
#include <lexy/dsl/option.hpp>
#include <lexy/dsl/peek.hpp>
#include <lexy/dsl/terminator.hpp>
#include <lexy/dsl/times.hpp>
#include <lexy/input/file.hpp>
#include <lexy/lexeme.hpp>
#include <lexy_ext/report_error.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../isa.h"

using Opptr = std::unique_ptr<Opcode>;
using Map = std::unordered_map<std::size_t, std::string>;

struct Program {
  Map globals;
  Map externals;
  std::vector<Opptr> code;
};

namespace grammar {
namespace dsl = lexy::dsl;

#define let static constexpr auto

let word = dsl::identifier(dsl::ascii::alpha_underscore,
                           dsl::ascii::alpha_digit_underscore);

struct doubl {
  let rule = [] {
    auto sign = dsl::opt(dsl::lit_c<'-'>);
    auto intPart = dsl::digits<>;
    auto frac = dsl::opt(dsl::lit_c<'.'> >> dsl::digits<>);
    return dsl::capture(dsl::token(sign + intPart + frac));
  }();

  let value = lexy::callback<double>([](auto lexeme) {
    double result = 0.0;
    std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), result);
    return result;
  });
};

template <typename T>
struct intt {
  let rule = dsl::capture(dsl::token(opt(dsl::lit_c<'-'>) + dsl::digits<>));
  let value = lexy::callback<T>([](auto lexeme) {
    T result = 0;
    std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), result);
    return result;
  });
};

struct str {
  let rule = dsl::quoted(dsl::ascii::print - dsl::lit_c<'"'>);
  let value = lexy::as_string<std::string>;
};

template <typename T>
struct num {
  let rule = dsl::sign + dsl::integer<T>;
  let value = lexy::forward<T>;
};

struct reg {
  let rule = dsl::lit_c<'r'> >> dsl::integer<std::size_t>;
  let value = lexy::forward<std::size_t>;
};

struct block {
  let rule = dsl::lit_c<'b'> >> dsl::integer<std::size_t>;
  let value = lexy::forward<std::size_t>;
};

struct ext {
  let rule = LEXY_LIT("ext") >> dsl::integer<std::size_t>;
  let value = lexy::forward<std::size_t>;
};

struct glo {
  let rule = LEXY_LIT("glo") >> dsl::integer<std::size_t>;
  let value = lexy::forward<std::size_t>;
};

struct func {
  let rule = LEXY_LIT("fn") >> dsl::integer<std::size_t>;
  let value = lexy::forward<std::size_t>;
};

#define MAKE_OP(type) lexy::_new<type, Opptr>()

#define REGSONLY(code, type, regs)                                         \
  struct _##code {                                                         \
    let rule = LEXY_KEYWORD(#code, word) >> dsl::times<regs>(dsl::p<reg>); \
    let value = MAKE_OP(type);                                             \
  };

#define REGINTEGER(code, type, regs, numType)                         \
  struct _##code {                                                    \
    let rule = LEXY_KEYWORD(#code, word) >>                           \
               dsl::times<regs>(dsl::p<reg>) + dsl::p<intt<numType>>; \
    let value = MAKE_OP(type);                                        \
  };

#define REGDOUBLE(code, type, regs)                           \
  struct _##code {                                            \
    let rule = LEXY_KEYWORD(#code, word) >>                   \
               dsl::times<regs>(dsl::p<reg>) + dsl::p<doubl>; \
    let value = MAKE_OP(type);                                \
  };

REGSONLY(add, Add, 3)
REGSONLY(mul, Mul, 3)
REGSONLY(sub, Sub, 3)
REGSONLY(uitofp, UiToFp, 2)
REGSONLY(fptoui, FpToUi, 2)
REGSONLY(ilt, ILt, 3)
REGSONLY(or, Or, 3)
REGSONLY(fadd, FAdd, 3)
REGSONLY(fsub, FSub, 3)
REGSONLY(fmul, FMul, 3)
REGSONLY(retv, RetV, 1)
REGSONLY(load, Load, 2)
REGSONLY(store, Store, 2)
REGSONLY(mov, Mov, 2)
REGSONLY(prep3, Prepare3, 3)
REGSONLY(rgb2lin, Rgb2Lin, 2)
REGSONLY(lin2rgb, Lin2Rgb, 2)
REGSONLY(gep, Gep, 3)
REGSONLY(gep8, Gep8, 3)

REGINTEGER(addi, AddI, 2, std::int64_t)
REGINTEGER(muli, MulI, 2, std::int64_t)
REGINTEGER(ieqi, IEqI, 2, std::int64_t)
REGINTEGER(ilti, ILtI, 2, std::uint64_t)
REGINTEGER(shli, ShlI, 2, std::uint64_t)
REGINTEGER(lshri, LShrI, 2, std::uint64_t)
REGINTEGER(andi, AndI, 2, std::uint64_t)
REGINTEGER(alloc, Alloc, 1, std::uint64_t)
REGINTEGER(movi, MovI, 1, std::int64_t)
REGINTEGER(gepi, GepI, 2, std::int64_t)

REGDOUBLE(faddi, FAddI, 2)
REGDOUBLE(fmuli, FMulI, 2)
REGDOUBLE(fdivi, FDivI, 2)
REGDOUBLE(fgti, FGtI, 2)
REGDOUBLE(flti, FLtI, 2)

struct _jmp {
  let rule = LEXY_KEYWORD("jmp", word) >> dsl::p<block>;
  let value = MAKE_OP(Jmp);
};

struct _cjmp {
  let rule =
      LEXY_KEYWORD("cjmp", word) >> dsl::p<reg> + dsl::p<block> + dsl::p<block>;
  let value = MAKE_OP(CJmp);
};

struct _callv {
  let rule = LEXY_KEYWORD("callv", word) >> dsl::p<reg> + dsl::p<func>;
  let value = MAKE_OP(CallV);
};

struct _calle {
  let rule = LEXY_KEYWORD("calle", word) >> dsl::p<ext>;
  let value = MAKE_OP(CallE);
};

struct _callev {
  let rule = LEXY_KEYWORD("callev", word) >> dsl::p<reg> + dsl::p<ext>;
  let value = MAKE_OP(CallEV);
};

struct _gepg {
  let rule =
      LEXY_KEYWORD("gepg", word) >> dsl::p<reg> + dsl::p<glo> + dsl::p<reg>;
  let value = MAKE_OP(GepG);
};

struct _block_label {
  let rule = dsl::p<block> >> dsl::lit_c<':'>;
  let value = MAKE_OP(Block);
};

struct _func_label {
  let rule = dsl::p<func> >> dsl::lit_c<':'>;
  let value = MAKE_OP(Func);
};

struct _main_label {
  let rule = LEXY_KEYWORD("main", word) >> dsl::lit_c<':'>;
  let value = MAKE_OP(Main);
};

struct _instruction {
  let rule = dsl::p<_add> | dsl::p<_addi> | dsl::p<_mul> | dsl::p<_muli> |
             dsl::p<_fmul> | dsl::p<_sub> | dsl::p<_uitofp> | dsl::p<_fptoui> |
             dsl::p<_ieqi> | dsl::p<_ilt> | dsl::p<_ilti> | dsl::p<_shli> |
             dsl::p<_lshri> | dsl::p<_andi> | dsl::p<_or> | dsl::p<_fadd> |
             dsl::p<_faddi> | dsl::p<_fsub> | dsl::p<_fmuli> | dsl::p<_fdivi> |
             dsl::p<_fgti> | dsl::p<_flti> | dsl::p<_jmp> | dsl::p<_cjmp> |
             dsl::p<_callv> | dsl::p<_callev> | dsl::p<_calle> | dsl::p<_retv> |
             dsl::p<_load> | dsl::p<_store> | dsl::p<_alloc> | dsl::p<_mov> |
             dsl::p<_movi> | dsl::p<_prep3> | dsl::p<_rgb2lin> |
             dsl::p<_lin2rgb> | dsl::p<_gep> | dsl::p<_gep8> | dsl::p<_gepi> |
             dsl::p<_gepg> | dsl::p<_block_label> | dsl::p<_func_label> |
             dsl::p<_main_label>;

  let value = lexy::forward<Opptr>;
};

struct instructionSection {
  let rule = dsl::terminator(dsl::eof).opt_list(dsl::p<_instruction>);
  let value = lexy::as_list<std::vector<Opptr>>;
};

struct gloDecl {
  let rule = dsl::p<glo> >> dsl::p<str>;
  let value = lexy::construct<std::pair<std::size_t, std::string>>;
};

struct globalSection {
  let rule = dsl::opt(LEXY_KEYWORD("global", word) >>
                      dsl::lit_c<':'> + dsl::list(dsl::p<gloDecl>));
  let value = lexy::as_collection<Map>;
};

struct extDecl {
  let rule = dsl::p<ext> >> dsl::p<str>;
  let value = lexy::construct<std::pair<std::size_t, std::string>>;
};

struct externalSection {
  let rule = dsl::opt(LEXY_KEYWORD("extern", word) >>
                      dsl::lit_c<':'> + dsl::list(dsl::p<extDecl>));
  let value = lexy::as_collection<Map>;
};

struct program {
  let whitespace = dsl::ascii::space;

  let rule = dsl::p<globalSection> + dsl::p<externalSection> +
             dsl::p<instructionSection>;
  let value = lexy::construct<Program>;
};

}  // namespace grammar

class Parser {
 public:
  Program parse(const char* path) {
    auto file = lexy::read_file(path);
    if (!file)
      throw std::runtime_error(std::format("failed to open file: {}", path));
    auto result =
        lexy::parse<grammar::program>(file.buffer(), lexy_ext::report_error);

    return std::move(result).value();
  }
};
