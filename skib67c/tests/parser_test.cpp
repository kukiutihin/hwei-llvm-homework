#include "parser.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

#include "ast.h"

namespace {

Program parseOk(std::string code) {
  Parser p;
  auto res = p.parseString(std::move(code));
  if (!res.has_value()) {
    for (const auto& err : res.error()) {
      UNSCOPED_INFO("Parse error at " << err.line << ":" << err.column << " -> "
                                      << err.message);
    }
  }
  REQUIRE(res.has_value());
  return std::move(res).value();
}

void parseErr(std::string code) {
  Parser p;
  auto res = p.parseString(std::move(code));
  REQUIRE_FALSE(res.has_value());
  REQUIRE_FALSE(res.error().empty());
}

const BitOrExpr& unwrapBin(const ValueExpr& v) {
  REQUIRE(std::holds_alternative<BitOrExpr>(v));
  return std::get<BitOrExpr>(v);
}

const PrimaryExpr& unwrapPrimary(const BitOrExpr& b) {
  return b.left.left.left.left.left.left.left.left.primary;
}

}  // namespace

// Top-level value declarations (single and multiple)
TEST_CASE("Top-level value declarations", "[parser][top-decl]") {
  SECTION("Single untyped top-level value") {
    auto prog = parseOk("ySize = 1080");
    REQUIRE(prog.size() == 1);
    REQUIRE(std::holds_alternative<ValDecl>(prog[0]));
  }

  SECTION("Multiple top-level values (typed and untyped)") {
    auto prog = parseOk(R"(
      xSize = 1920
      ySize: Uint = 1080
      alpha: Float = 0.25
    )");
    REQUIRE(prog.size() == 3);
    REQUIRE(std::holds_alternative<ValDecl>(prog[0]));
    REQUIRE(std::holds_alternative<ValDecl>(prog[1]));
    REQUIRE(std::holds_alternative<ValDecl>(prog[2]));
  }
}

// Top-level function declarations (with/without paramList, multiple
// functions)
TEST_CASE("Top-level function declarations", "[parser][func-decl]") {
  SECTION("Function with arguments and return type") {
    auto prog = parseOk("add(a: Int, b: Int): Int = a + b");
    REQUIRE(prog.size() == 1);
    auto& fn = std::get<FuncDecl>(prog[0]);
    REQUIRE(fn.paramList.size() == 2);
  }

  SECTION("Function with empty parameter list ()") {
    auto prog = parseOk("zero(): Int = 0");
    REQUIRE(prog.size() == 1);
    auto& fn = std::get<FuncDecl>(prog[0]);
    REQUIRE(fn.paramList.empty());
  }

  SECTION("Nullary function with optional parameter list omitted") {
    auto prog = parseOk("app: () = ()");
    REQUIRE(prog.size() == 1);
    auto& fn = std::get<FuncDecl>(prog[0]);
    REQUIRE(fn.paramList.empty());
  }

  SECTION("Two distinct functions") {
    auto prog = parseOk(R"(
      foo(x: Int): Int = x + 1
      bar(a: Float, b: Float): Float = a * b
    )");
    REQUIRE(prog.size() == 2);
    REQUIRE(std::holds_alternative<FuncDecl>(prog[0]));
    REQUIRE(std::holds_alternative<FuncDecl>(prog[1]));
  }
}

// All built-in types
TEST_CASE("All types", "[parser][types]") {
  auto prog = parseOk(R"(
    f(
      a: Int,
      b: Uint,
      c: Float,
      d: Bool,
      e: (),
      arr1: Array[Int],
      arr2: Array[Uint],
      arr3: Array[Float],
      arr4: Array[Bool]
    ): () = ()
  )");
  REQUIRE(prog.size() == 1);
  auto& fn = std::get<FuncDecl>(prog[0]);
  REQUIRE(fn.paramList.size() == 9);
  REQUIRE(std::holds_alternative<ScalarType>(fn.paramList[0].type));
  REQUIRE(
      std::holds_alternative<Int>(std::get<ScalarType>(fn.paramList[0].type)));
  REQUIRE(
      std::holds_alternative<Uint>(std::get<ScalarType>(fn.paramList[1].type)));
  REQUIRE(std::holds_alternative<Float>(
      std::get<ScalarType>(fn.paramList[2].type)));
  REQUIRE(
      std::holds_alternative<Bool>(std::get<ScalarType>(fn.paramList[3].type)));
  REQUIRE(std::holds_alternative<Skib>(fn.paramList[4].type));
  REQUIRE(std::holds_alternative<ArrayType>(fn.paramList[5].type));
  REQUIRE(std::holds_alternative<Skib>(fn.type));

  SECTION("Invalid type is rejected") {
    parseErr("f(x: String): Int = 0");
    parseErr("f(x: Array[()]): Int = 0");
  }
}

// Nested function & following expression outside nested function
TEST_CASE("Nested functions", "[parser][nested-func]") {
  SECTION("Dangling nested function without following expression is an error") {
    parseErr(R"(
      outer(x: Int): Int =
        inner(y: Int): Int = y + 1
    )");
  }

  SECTION("Nested function followed by expression outside it") {
    auto prog = parseOk(R"(
      outer(x: Int): Int =
        inner(y: Int): Int = y + 1
        inner(x)
    )");
    REQUIRE(prog.size() == 1);
    auto& fn = std::get<FuncDecl>(prog[0]);
    REQUIRE(fn.expr->binds.size() == 1);
    REQUIRE(std::holds_alternative<FuncDeclExpr>(fn.expr->binds[0]));

    auto& nested = std::get<FuncDeclExpr>(fn.expr->binds[0]);
    REQUIRE(nested.next != nullptr);
    REQUIRE(std::holds_alternative<ValueExpr>(nested.next->binds[0]));
  }
}

// Several bindings inside a function
TEST_CASE("Several bindings inside function", "[parser][bindings]") {
  auto prog = parseOk(R"(
    compute(x: Int): Int =
      a = x + 1
      b: Int = a * 2
      c = b - 3
      c
  )");
  REQUIRE(prog.size() == 1);
  auto& fn = std::get<FuncDecl>(prog[0]);
  REQUIRE(std::holds_alternative<Assignment>(fn.expr->binds[0]));
  auto& a_bind = std::get<Assignment>(fn.expr->binds[0]);
  REQUIRE(a_bind.next != nullptr);
  REQUIRE(std::holds_alternative<Assignment>(a_bind.next->binds[0]));
}

// Several ()-returning expressions separated by ;
TEST_CASE("Multiple unit expressions separated by semicolon",
          "[parser][seq]") {
  auto prog = parseOk(R"(
    run(arr: Array[Int]): () =
      arr[0] *= 1;
      arr[1] *= 2;
      ()
  )");
  REQUIRE(prog.size() == 1);
  auto& fn = std::get<FuncDecl>(prog[0]);
  REQUIRE(fn.expr->binds.size() == 3);
  REQUIRE(std::holds_alternative<MutExpr>(fn.expr->binds[0]));
  REQUIRE(std::holds_alternative<MutExpr>(fn.expr->binds[1]));
  REQUIRE(std::holds_alternative<ValueExpr>(fn.expr->binds[2]));
}

// Two ()-returning expressions without ; must be an error
TEST_CASE("Missing semicolon between unit expressions is an error",
          "[parser][seq][error]") {
  parseErr(R"(
    run(): () =
      ()
      ()
  )");

  parseErr(R"(
    run(arr: Array[Int]): () =
      arr[0] *= 1
      arr[1] *= 2
  )");
}

// Function calls (with args, empty (), and bare nullary identifier)
TEST_CASE("Function calls", "[parser][call]") {
  auto prog = parseOk(R"(
    test(): Int =
      a = foo(1, 2 + 3, bar)
      b = baz()
      c = nullaryFn
      d = Mod.func(a, b)
      a + b + c + d
  )");
  REQUIRE(prog.size() == 1);
}

// if-then-else inside function with multiple expressions in then & else
TEST_CASE("if-then-else with multi-expression branches", "[parser][if]") {
  auto prog = parseOk(R"(
    step(arr: Array[Int], flag: Bool): () =
      if flag then
        a = 1
        arr[0] *= a;
        ()
      else
        b = 2
        arr[1] *= b;
        ()
  )");
  REQUIRE(prog.size() == 1);
  auto& fn = std::get<FuncDecl>(prog[0]);
  auto& val = std::get<ValueExpr>(fn.expr->binds[0]);
  REQUIRE(std::holds_alternative<IfExpr>(val));
}

// Sequential if-then-else bindings (left is outside bottom)
TEST_CASE("Sequential if-then-else value bindings",
          "[parser][if][bindings]") {
  auto prog = parseOk(R"(
    calc(x: Uint, y: Uint, xSize: Uint, ySize: Uint, field: Array[Float], currT: Float): Float =
      bottom = if y < ySize - 1 then field[(y + 1) * xSize + x] else currT
      left = if x > 0 then field[y * xSize + x - 1] else currT
      bottom + left
  )");
  REQUIRE(prog.size() == 1);
  auto& fn = std::get<FuncDecl>(prog[0]);

  // First binding is `bottom`
  auto& bottom = std::get<Assignment>(fn.expr->binds[0]);
  REQUIRE(bottom.ident.ident == "bottom");
  REQUIRE(std::holds_alternative<IfExpr>(bottom.expr));

  // `left` must be in `bottom.next`, NOT swallowed into `bottom`'s else-branch
  REQUIRE(bottom.next != nullptr);
  auto& left = std::get<Assignment>(bottom.next->binds[0]);
  REQUIRE(left.ident.ident == "left");
  REQUIRE(std::holds_alternative<IfExpr>(left.expr));

  // Final expression `bottom + left` is in `left.next`
  REQUIRE(left.next != nullptr);
  REQUIRE(std::holds_alternative<ValueExpr>(left.next->binds[0]));
}

// Literals (int, hex, float, bool, ()) and invalid trailing dot
TEST_CASE("Literals", "[parser][literals]") {
  SECTION("Valid literals") {
    auto prog = parseOk(R"(
      i = 42
      h = 0xDEADBEEF
      f = 3.14159
      t = True
      fl = False
      u = ()
    )");
    REQUIRE(prog.size() == 6);
  }

  SECTION("Trailing decimal point is invalid") {
    parseErr("f = 3.");
    parseErr("f = 0.");
  }
}
// All binary & unary operators and full precedence hierarchy (per spec)
TEST_CASE("Operator precedence and unary/binary operators",
          "[parser][precedence]") {
  SECTION("All unary operators: -, !, ~ and chaining") {
    auto prog = parseOk(R"(
      a = -1
      b = !True
      c = ~0xFF
      d = -!~x
    )");
    REQUIRE(prog.size() == 4);
  }

  SECTION(
      "Full precedence ladder: | < ^ < & < ==/!= < </<=/>/>= < <</>> < +/- < "
      "*/%//") {
    // a | b ^ c & d == e < f << g + h * -i
    // Each operator should nest strictly inside the one to its left.
    auto prog = parseOk("x = a | b ^ c & d == e < f << g + h * -i");
    const auto& bitOr = std::get<BitOrExpr>(std::get<ValDecl>(prog[0]).expr);
    REQUIRE(bitOr.right.size() == 1);  // a | (...)

    const auto& bitXor = bitOr.right[0];
    REQUIRE(bitXor.right.size() == 1);  // b ^ (...)

    const auto& bitAnd = bitXor.right[0];
    REQUIRE(bitAnd.right.size() == 1);  // c & (...)

    const auto& eq = bitAnd.right[0];
    REQUIRE(eq.right.size() == 1);
    REQUIRE(eq.right[0].op == EqOp::Eq);  // d == (...)

    const auto& cmp = eq.right[0].rhs;
    REQUIRE(cmp.right.size() == 1);
    REQUIRE(cmp.right[0].op == CmpOp::Lt);  // e < (...)

    const auto& shift = cmp.right[0].rhs;
    REQUIRE(shift.right.size() == 1);
    REQUIRE(shift.right[0].op == ShiftOp::Shl);  // f << (...)

    const auto& add = shift.right[0].rhs;
    REQUIRE(add.right.size() == 1);
    REQUIRE(add.right[0].op == AddOp::Add);  // g + (...)

    const auto& mul = add.right[0].rhs;
    REQUIRE(mul.right.size() == 1);
    REQUIRE(mul.right[0].op == MulOp::Mul);  // h * (-i)

    const auto& unary = mul.right[0].rhs;
    REQUIRE(unary.ops.size() == 1);
    REQUIRE(unary.ops[0] == UnaryOp::Neg);
  }

  SECTION("Parentheses override precedence") {
    auto prog = parseOk("x = (a + b) * c");
    const auto& bitOr = std::get<BitOrExpr>(std::get<ValDecl>(prog[0]).expr);
    const auto& mul = bitOr.left.left.left.left.left.left.left;
    REQUIRE(mul.right.size() == 1);
    REQUIRE(std::holds_alternative<InParens>(mul.left.primary));
  }

  SECTION("All remaining binary operators parse") {
    auto prog = parseOk(R"(
      a = 1 != 2
      b = 1 <= 2
      c = 1 >= 2
      d = 1 > 2
      e = 1 >> 2
      f = 1 - 2
      g = 1 / 2
      h = 1 % 2
    )");
    REQUIRE(prog.size() == 8);
  }
}

TEST_CASE("Source positions on all AST nodes", "[parser][position]") {
  auto checkPos = [](const Position& p, std::size_t line, std::size_t col) {
    CHECK(p.line == line);
    CHECK(p.column == col);
  };

  auto unwrapPrimary = [](const ValueExpr& v) -> const PrimaryExpr& {
    return std::get<BitOrExpr>(v)
        .left.left.left.left.left.left.left.left.primary;
  };

  SECTION("FuncDecl, FuncDeclExpr, Param, Ident, and all Types") {
    // 12345678901234567890123456789012345678901234567890123456789012
    // foo (a: Int, b: Uint, c: Float, d: Bool, e: Array[Int]) : () =
    //   bar : () = ()
    //   ()
    auto prog = parseOk(
        "foo (a: Int, b: Uint, c: Float, d: Bool, e: Array[Int]) : () =\n"
        "  bar : () = ()\n"
        "  ()\n");
    REQUIRE(prog.size() == 1);

    const auto& fn = std::get<FuncDecl>(prog[0]);
    checkPos(fn.pos, 1, 1);
    checkPos(fn.ident.pos, 1, 1);

    REQUIRE(fn.paramList.size() == 5);
    checkPos(fn.paramList[0].pos, 1, 6);
    checkPos(fn.paramList[0].ident.pos, 1, 6);
    checkPos(std::get<Int>(std::get<ScalarType>(fn.paramList[0].type)).pos, 1, 9);

    checkPos(fn.paramList[1].pos, 1, 14);
    checkPos(fn.paramList[1].ident.pos, 1, 14);
    checkPos(std::get<Uint>(std::get<ScalarType>(fn.paramList[1].type)).pos, 1, 17);

    checkPos(fn.paramList[2].pos, 1, 23);
    checkPos(fn.paramList[2].ident.pos, 1, 23);
    checkPos(std::get<Float>(std::get<ScalarType>(fn.paramList[2].type)).pos, 1, 26);

    checkPos(fn.paramList[3].pos, 1, 33);
    checkPos(fn.paramList[3].ident.pos, 1, 33);
    checkPos(std::get<Bool>(std::get<ScalarType>(fn.paramList[3].type)).pos, 1, 36);

    checkPos(fn.paramList[4].pos, 1, 42);
    checkPos(fn.paramList[4].ident.pos, 1, 42);
    const auto& arrType = std::get<ArrayType>(fn.paramList[4].type);
    checkPos(arrType.pos, 1, 45);
    checkPos(std::get<Int>(arrType.generic).pos, 1, 51);

    // Return type ()
    checkPos(std::get<Skib>(fn.type).pos, 1, 59);

    // Nested FuncDeclExpr
    const auto& nested = std::get<FuncDeclExpr>(fn.expr->binds[0]);
    checkPos(nested.pos, 2, 3);
    checkPos(nested.decl.pos, 2, 3);
    checkPos(nested.decl.ident.pos, 2, 3);
    checkPos(std::get<Skib>(nested.decl.type).pos, 2, 9);

    // Continuation () literal on line 3
    const auto& nextVal = std::get<ValueExpr>(nested.next->binds[0]);
    checkPos(std::get<Skib>(std::get<Literal>(unwrapPrimary(nextVal))).pos, 3, 3);
  }

  SECTION("All Literals and ModIdent") {
    // 123456789012
    // a = 42
    // b = 3.14
    // c = 0xFF
    // d = True
    // e = False
    // f = ()
    // g = Std.fmax
    auto prog = parseOk(
        "a = 42\n"
        "b = 3.14\n"
        "c = 0xFF\n"
        "d = True\n"
        "e = False\n"
        "f = ()\n"
        "g = Std.fmax\n");
    REQUIRE(prog.size() == 7);

    auto litAt = [&](std::size_t i) -> const Literal& {
      return std::get<Literal>(unwrapPrimary(std::get<ValDecl>(prog[i]).expr));
    };

    checkPos(std::get<ValDecl>(prog[0]).ident.pos, 1, 1);
    checkPos(std::get<IntLit>(litAt(0)).pos, 1, 5);

    checkPos(std::get<ValDecl>(prog[1]).ident.pos, 2, 1);
    checkPos(std::get<FloatLit>(litAt(1)).pos, 2, 5);

    checkPos(std::get<ValDecl>(prog[2]).ident.pos, 3, 1);
    checkPos(std::get<HexLit>(litAt(2)).pos, 3, 5);

    checkPos(std::get<ValDecl>(prog[3]).ident.pos, 4, 1);
    checkPos(std::get<True>(litAt(3)).pos, 4, 5);

    checkPos(std::get<ValDecl>(prog[4]).ident.pos, 5, 1);
    checkPos(std::get<False>(litAt(4)).pos, 5, 5);

    checkPos(std::get<ValDecl>(prog[5]).ident.pos, 6, 1);
    checkPos(std::get<Skib>(litAt(5)).pos, 6, 5);

    checkPos(std::get<ValDecl>(prog[6]).ident.pos, 7, 1);
    const auto& qid =
        std::get<QualIdent>(unwrapPrimary(std::get<ValDecl>(prog[6]).expr));
    REQUIRE(qid.module.has_value());
    checkPos(qid.module->pos, 7, 5);
    checkPos(qid.ident.pos, 7, 9);
  }

  SECTION("Operators: CmpRhs, ShiftRhs, AddExpr, AddRhs, MulRhs, UnaryExpr") {
    // 12345678901234567890123
    // x = 1 < 2 << 3 + 4 * -5
    auto prog = parseOk("x = 1 < 2 << 3 + 4 * -5");
    REQUIRE(prog.size() == 1);

    const auto& bitOr = std::get<BitOrExpr>(std::get<ValDecl>(prog[0]).expr);
    const auto& cmp = bitOr.left.left.left.left;

    // 1 < (...)
    REQUIRE(cmp.right.size() == 1);
    checkPos(cmp.right[0].pos, 1, 7);

    // 2 << (...)
    const auto& shift = cmp.right[0].rhs;
    checkPos(shift.left.pos, 1, 9);  // AddExpr for `2`
    REQUIRE(shift.right.size() == 1);
    checkPos(shift.right[0].pos, 1, 11);

    // 3 + (...)
    const auto& add = shift.right[0].rhs;
    checkPos(add.pos, 1, 14);  // AddExpr starting at `3`
    REQUIRE(add.right.size() == 1);
    checkPos(add.right[0].pos, 1, 16);

    // 4 * (-5)
    const auto& mul = add.right[0].rhs;
    REQUIRE(mul.right.size() == 1);
    checkPos(mul.right[0].pos, 1, 20);

    // -5
    const auto& unary = mul.right[0].rhs;
    checkPos(unary.pos, 1, 22);
    checkPos(std::get<IntLit>(std::get<Literal>(unary.primary)).pos, 1, 23);
  }
}
