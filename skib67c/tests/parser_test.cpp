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
TEST_CASE("1. Top-level value declarations", "[parser][top-decl]") {
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
TEST_CASE("2. Top-level function declarations", "[parser][func-decl]") {
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
TEST_CASE("3. All types", "[parser][types]") {
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
TEST_CASE("4. Nested functions", "[parser][nested-func]") {
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
TEST_CASE("5. Several bindings inside function", "[parser][bindings]") {
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
TEST_CASE("6. Multiple unit expressions separated by semicolon",
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
TEST_CASE("7. Missing semicolon between unit expressions is an error",
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
TEST_CASE("9. Function calls", "[parser][call]") {
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
TEST_CASE("10. if-then-else with multi-expression branches", "[parser][if]") {
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
TEST_CASE("11. Sequential if-then-else value bindings",
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
  REQUIRE(bottom.ident == "bottom");
  REQUIRE(std::holds_alternative<IfExpr>(bottom.expr));

  // `left` must be in `bottom.next`, NOT swallowed into `bottom`'s else-branch
  REQUIRE(bottom.next != nullptr);
  auto& left = std::get<Assignment>(bottom.next->binds[0]);
  REQUIRE(left.ident == "left");
  REQUIRE(std::holds_alternative<IfExpr>(left.expr));

  // Final expression `bottom + left` is in `left.next`
  REQUIRE(left.next != nullptr);
  REQUIRE(std::holds_alternative<ValueExpr>(left.next->binds[0]));
}

// Literals (int, hex, float, bool, ()) and invalid trailing dot
TEST_CASE("13. Literals", "[parser][literals]") {
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
TEST_CASE("14. Operator precedence and unary/binary operators",
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
