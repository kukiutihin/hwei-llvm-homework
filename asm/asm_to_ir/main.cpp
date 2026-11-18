#include <cstdlib>
#include <iostream>
#include <print>
#include <stdexcept>

#include "parser.h"

int main(int argc, char* argv[]) {
  try {
    if (argc != 3)
      throw std::runtime_error(
          "expected usage: lifter <path_to_asm> <mode: emulate | lift>\n");

    Parser parser;
    auto program = parser.parse(argv[1]);

    for (auto& instr : program.code) std::println("{}", instr->toString());
  } catch (std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}
