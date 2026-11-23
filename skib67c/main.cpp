#include <cstdio>
#include <cstdlib>
#include <exception>
#include <expected>
#include <print>
#include <vector>

#include "interpreter.h"
#include "parser.h"
#include "typechecker.h"

int main(int argc, char* argv[]) {
  try {
    Parser parser;
    TypeChecker checker;

    auto res = parser.parse(argv[1]).and_then([&](Program&& program) {
      return checker.check(program).transform(
          [&] { return std::move(program); });
    });

    if (!res) {
      std::println(stderr, "{}", errsToString("app", res.error()));
      return EXIT_FAILURE;
    }

    Interpreter interpreter;
    interpreter.interpret(*res);

  } catch (const std::exception& err) {
    std::println(stderr, "{}", err.what());
    return EXIT_FAILURE;
  }
}
