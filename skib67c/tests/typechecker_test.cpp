#include "typechecker.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

#include "parser.h"

namespace {

void checkOk(std::string code) {
  Parser p;
  auto prog = p.parseString(std::move(code));
  if (!prog.has_value()) {
    for (const auto& err : prog.error()) {
      UNSCOPED_INFO("Parse error at " << err.line << ":" << err.column << " -> "
                                      << err.message);
    }
  }
  REQUIRE(prog.has_value());

  TypeChecker tc;
  auto res = tc.check(*prog);
  if (!res.has_value()) {
    for (const auto& err : res.error()) {
      UNSCOPED_INFO("Type error at " << err.line << ":" << err.column << " -> "
                                     << err.message);
    }
  }
  REQUIRE(res.has_value());
}

void checkErr(std::string code) {
  Parser p;
  auto prog = p.parseString(std::move(code));
  if (!prog.has_value()) {
    for (const auto& err : prog.error()) {
      UNSCOPED_INFO("Parse error at " << err.line << ":" << err.column << " -> "
                                      << err.message);
    }
  }
  REQUIRE(prog.has_value());

  TypeChecker tc;
  auto res = tc.check(*prog);
  REQUIRE_FALSE(res.has_value());
  REQUIRE_FALSE(res.error().empty());
}

}  // namespace

TEST_CASE("Literal type inference", "[typechecker][literals]") {
  SECTION("Scalar and unit literals") {
    checkOk(R"(
      i = 42
      u = 42u
      h = 0xFF
      f = 3.14
      t = True
      fl = False
      s = ()

      checkTypes(a: Int, b: Uint, c: Uint, d: Float, e: Bool, g: Bool, z: ()): () = ()
      run: () = checkTypes(i, u, h, f, t, fl, s)
    )");
  }

  SECTION("Array literal and array(Uint) inference") {
    checkOk(R"(
      aInt = [1, 2, 3]
      aUint = [1u, 0xFF]
      aFloat = [1.0, 2.5]
      aBool = [True, False]
      aMixed = [1u, 2, 3.5]
      aEmpty: Array[Int] = []
      aAlloc: Array[Float] = array(10u)

      takeArrays(
        x1: Array[Int],
        x2: Array[Uint],
        x3: Array[Float],
        x4: Array[Bool],
        x5: Array[Float],
        x6: Array[Int],
        x7: Array[Float]
      ): () = ()

      run: () = takeArrays(aInt, aUint, aFloat, aBool, aMixed, aEmpty, aAlloc)
    )");
  }

  SECTION("Inferred literal types reject mismatched annotations") {
    checkErr("x: Uint = 42");
    checkErr("x: Int = 3.14");
    checkErr("x: Bool = 1");
    checkErr("x: Int = True");
    checkErr("x: () = 0");
    checkErr("x = []");
    checkErr("x = array(10u)");
    checkErr("x: Array[Int] = array(10)");
  }
}

TEST_CASE("Implicit type casts", "[typechecker][cast]") {
  SECTION("Allowed widening casts: Uint -> Int -> Float") {
    checkOk(R"(
      u: Uint = 5u
      iFromU: Int = u
      fFromU: Float = u
      fFromI: Float = iFromU

      uLitToInt: Int = 10u
      uLitToFloat: Float = 0x10
      iLitToFloat: Float = 10
    )");
  }

  SECTION("Allowed widening in local bindings and array mutation") {
    checkOk(R"(
      mutate(aInt: Array[Int], aFloat: Array[Float]): () =
        x: Int = 1u
        y: Float = x
        aInt[0u] *= 42u;
        aFloat[0u] *= 42u;
        aFloat[1u] *= 42;
        ()
    )");
  }

  SECTION("Disallowed narrowing and cross-domain casts") {
    checkErr("x: Uint = 1");            // Int -> Uint
    checkErr("x: Uint = 1.0");          // Float -> Uint
    checkErr("x: Int = 1.0");           // Float -> Int
    checkErr("x: Int = True");          // Bool -> Int
    checkErr("x: Float = False");       // Bool -> Float
    checkErr("x: Bool = 0");            // Int -> Bool
    checkErr("x: Bool = 0u");           // Uint -> Bool
    checkErr("x: Array[Int] = [1.0]");  // Array[Float] -> Array[Int]
    checkErr(R"(
      badMut(aUint: Array[Uint]): () =
        aUint[0u] *= 1
    )");                                // Int -> Uint in mutation
  }
}

TEST_CASE("Function calls and return values", "[typechecker][call]") {
  SECTION("Valid calls and mutual recursion") {
    checkOk(R"(
      isEven(n: Uint): Bool = if n == 0u then True else isOdd(n - 1u)
      isOdd(n: Uint): Bool = if n == 0u then False else isEven(n - 1u)
      nullary: Int = 42
      useNullary(): Int = nullary
    )");
  }

  SECTION("Calls with implicit argument widening") {
    checkOk(R"(
      takeInt(x: Int): Int = x
      takeFloat(x: Float): Float = x

      run(): Float =
        a = takeInt(10u)
        b = takeFloat(10u)
        c = takeFloat(a)
        b + c
    )");
  }

  SECTION("Return value widening") {
    checkOk(R"(
      retIntFromUint(): Int = 7u
      retFloatFromUint(): Float = 7u
      retFloatFromInt(): Float = 7
    )");
  }

  SECTION("Incorrect argument count") {
    checkErr(R"(
      add(a: Int, b: Int): Int = a + b
      run(): Int = add(1)
    )");
    checkErr(R"(
      add(a: Int, b: Int): Int = a + b
      run(): Int = add(1, 2, 3)
    )");
    checkErr(R"(
      zero(): Int = 0
      run(): Int = zero(1)
    )");
  }

  SECTION("Incorrect argument types") {
    checkErr(R"(
      takeUint(x: Uint): Uint = x
      run(): Uint = takeUint(1)
    )");
    checkErr(R"(
      takeInt(x: Int): Int = x
      run(): Int = takeInt(1.5)
    )");
    checkErr(R"(
      takeBool(x: Bool): Bool = x
      run(): Bool = takeBool(1)
    )");
  }

  SECTION("Incorrect return value type") {
    checkErr("bad(): Uint = 1");
    checkErr("bad(): Int = 1.5");
    checkErr("bad(): Bool = 0");
    checkErr("bad(): () = 1");
  }
}

TEST_CASE("if-then-else type checking", "[typechecker][if]") {
  SECTION("Matching branch types") {
    checkOk(R"(
      f(cond: Bool): Int = if cond then 1 else 2
      g(cond: Bool): () = if cond then () else ()
    )");
  }

  SECTION("Branch unification with allowed widening casts") {
    checkOk(R"(
      uAndI(cond: Bool): Int = if cond then 1u else 2
      iAndU(cond: Bool): Int = if cond then 1 else 2u
      uAndF(cond: Bool): Float = if cond then 1u else 2.5
      iAndF(cond: Bool): Float = if cond then 1 else 2.5
    )");
  }

  SECTION("Widened if-then-else result still respects target type") {
    // Branch result widens to Float, cannot be assigned to Int
    checkErr("f(cond: Bool): Int = if cond then 1 else 2.5");
    // Branch result widens to Int, cannot be assigned to Uint
    checkErr("f(cond: Bool): Uint = if cond then 1u else 2");
  }

  SECTION("Non-Bool condition is an error") {
    checkErr("f(): Int = if 1 then 1 else 2");
    checkErr("f(): Int = if 1u then 1 else 2");
    checkErr("f(): Int = if 1.0 then 1 else 2");
    checkErr("f(): Int = if () then 1 else 2");
  }

  SECTION("Incompatible branch types are an error") {
    checkErr("f(cond: Bool): Int = if cond then 1 else True");
    checkErr("f(cond: Bool): Bool = if cond then True else ()");
    checkErr("f(cond: Bool): Float = if cond then 1.0 else False");
  }
}

TEST_CASE("Binary operations and casts", "[typechecker][binop]") {
  SECTION("Arithmetic (+, -, *, /) with widening") {
    checkOk(R"(
      a: Uint = 1u + 2u
      b: Int = 1u + 2
      c: Int = 1 - 2u
      d: Float = 1u * 2.0
      e: Float = 1 / 2.0
      f: Float = 1.0 + 2u * 3
    )");
  }

  SECTION(
      "Bitwise (|, ^, &), shifts (<<, >>), and modulo (%) with integral "
      "types") {
    checkOk(R"(
      a: Uint = (0xFF | 0x0F) ^ (0x10 & 0xFF)
      b: Uint = (1u << 4u) >> 1u
      c: Uint = 10u % 3u
      d: Int = 1u | 2
      e: Int = 1 << 2u
      f: Int = 10 % 3u
    )");
  }

  SECTION("Comparisons and equality with widening") {
    checkOk(R"(
      c1: Bool = 1u < 2
      c2: Bool = 1 <= 2.5
      c3: Bool = 3.5 > 1u
      c4: Bool = 1u >= 0u
      e1: Bool = 1u == 1
      e2: Bool = 1 != 1.0
      e3: Bool = True == False
    )");
  }

  SECTION("Widened binary result cannot narrow back") {
    checkErr("x: Uint = 1u + 2");
    checkErr("x: Int = 1 + 2.0");
    checkErr("x: Uint = 1u << 2");
    checkErr("x: Uint = 10u % 3");
    checkErr("x: Uint = 1u | 2");
  }

  SECTION("Invalid operand types for binary operators") {
    checkErr("x = 1.0 | 2");
    checkErr("x = 1 ^ 2.0");
    checkErr("x = 1.0 & 2.0");
    checkErr("x = 1.0 << 1u");
    checkErr("x = 1u >> 1.0");
    checkErr("x = 5.0 % 2");

    checkErr("x = True + 1");
    checkErr("x = 1 * False");
    checkErr("x = True < False");
    checkErr("x = True | False");

    checkErr("x = 1 == True");
    checkErr("x = () != 0");
  }
}

TEST_CASE("Undefined functions and variables", "[typechecker][undefined]") {
  SECTION("Undefined variables") {
    checkErr("x = unknownVar");
    checkErr("f(a: Int): Int = a + b");
    checkErr(R"(
      f(): Int =
        x = (a = 1 a)
        a
    )");
  }

  SECTION("Undefined functions") {
    checkErr("f(): Int = missingFunc(1, 2)");
    checkErr("f(): () = missingNullary");
    checkErr(R"(
      outer(): Int =
        inner(): Int = 1
        inner()
      caller(): Int = inner()
    )");
  }
}

TEST_CASE("7. Color conversion keywords: rgb2linear and linear2rgb",
          "[typechecker][color-conv]") {
  SECTION("Valid rgb2linear and linear2rgb usage") {
    checkOk(R"(
      a: Float = rgb2linear (0xff)
      b: Float = rgb2linear (128u)
      c: Uint = linear2rgb (0.5)
      d: Uint = linear2rgb (1)    
      e: Uint = linear2rgb (1u)   

      interpolate(color1: Uint, color2: Uint, t: Float): Uint =
        r1 = rgb2linear ((color1 >> 16u) & 0xff)
        r2 = rgb2linear ((color2 >> 16u) & 0xff)
        linear2rgb ((r2 - r1) * t + r1)
    )");
  }

  SECTION("Invalid argument types and return assignments") {
    // rgb2linear requires Uint (no Int/Float/Bool/() narrowing)
    checkErr("x = rgb2linear (10)");
    checkErr("x = rgb2linear (0.5)");
    checkErr("x = rgb2linear (True)");
    checkErr("x = rgb2linear (())");

    // linear2rgb requires Float (or numeric widening to Float)
    checkErr("x = linear2rgb (True)");
    checkErr("x = linear2rgb (())");

    // rgb2linear returns Float, cannot assign to Int or Uint
    checkErr("x: Int = rgb2linear (0xff)");
    checkErr("x: Uint = rgb2linear (0xff)");
  }
}
