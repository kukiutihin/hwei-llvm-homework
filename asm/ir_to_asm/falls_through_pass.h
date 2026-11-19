#pragma once

#include <memory>
#include <vector>

#include "isa.h"

inline void fallsThroughPass(std::vector<std::unique_ptr<Opcode>>& code) {
  for (std::size_t i = 0; i < code.size(); i++) {
    if (code[i]->type == OpcodeType::OpJmp) {
      if (i != code.size() - 1 && code[i + 1]->type == OpcodeType::Block) {
        auto jmp = static_cast<Jmp*>(code[i].get());
        auto block = static_cast<Block*>(code[i + 1].get());
        if (jmp->target == block->id) code[i] = std::make_unique<Dummy>();
      }
    }
  }
}
