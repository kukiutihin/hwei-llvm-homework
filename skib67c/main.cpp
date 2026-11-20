#include <cstdio>
#include <expected>
#include <print>

#include "parser.h"

int main(int argc, char* argv[]) {
  Parser parser;
  auto parsed = parser.parse(argv[1]);
  if (!parsed)
    std::println(stderr, "{}", errsToString("app", parsed.error()));
}
