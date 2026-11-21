#include <cstdio>
#include <expected>
#include <print>
#include <vector>

#include "parser.h"
#include "typechecker.h"

int main(int argc, char* argv[]) {
  Parser parser;
  TypeChecker checker;

  auto parsed =
      parser.parse(argv[1])
          .and_then(
              [&](const Program& program) { return checker.check(program); })
          .or_else([](const std::vector<SkibError>& errs)
                       -> decltype(checker.check(std::declval<Program>())) {
            std::println(stderr, "{}", errsToString("app", errs));
            return std::unexpected(errs);
          });
}
