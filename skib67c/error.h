#pragma once

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct SkibError {
  std::size_t line;
  std::size_t column;
  std::string message;
};

inline std::string errsToString(std::string fileName,
                                const std::vector<SkibError>& errs) {
  std::stringstream buffer;
  for (const auto& err : errs)
    buffer << fileName << ":" << err.line << ":" << err.column
           << ": \033[1;31merror:\033[0m: " << err.message << "\n";
  return buffer.str();
}
