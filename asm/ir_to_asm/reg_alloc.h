#pragma once

#include <cstddef>
#include <set>
#include <stack>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "llvm/IR/InstrTypes.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

inline std::unordered_map<Value*, std::size_t> allocate(
    llvm::Module& mod, std::size_t regCount,
    const std::unordered_set<std::string_view>& skipped) {
  std::unordered_map<Value*, std::size_t> result;

  for (auto& func : mod) {
    if (skipped.contains(func.getName())) continue;

    std::vector<Value*> lifetimeStart;
    std::unordered_map<Value*, std::size_t> lifetimeCurrent;

    for (auto& bb : func) {
      for (auto& instr : bb) {
        if (isa<ZExtInst>(instr) || isa<SExtInst>(instr) ||
            isa<AllocaInst>(instr) || isa<TruncInst>(instr) ||
            isa<BitCastInst>(instr))
          continue;

        if (!instr.getType()->isVoidTy()) {
          lifetimeStart.push_back(&instr);
          lifetimeCurrent[&instr] = lifetimeStart.size();
        }
        for (auto& op : instr.operands())
          lifetimeCurrent[op] = lifetimeStart.size();
      }
    }

    std::vector<std::vector<Value*>> adj(lifetimeStart.size() + 1);

    for (std::size_t startX = 0; startX < lifetimeStart.size(); startX++) {
      for (std::size_t startY = 0; startY < lifetimeStart.size(); startY++) {
        auto* valueX = lifetimeStart[startX];
        auto* valueY = lifetimeStart[startY];
        if (startX > startY && lifetimeCurrent[valueX] >= startY &&
            lifetimeCurrent[valueY] >= startX) {
          adj[startX].push_back(valueY);
          adj[startY].push_back(valueX);
        }
      }
    }

    for (std::size_t id = 0; id < lifetimeStart.size(); id++) {
      auto* value = lifetimeStart[id];
      std::set<std::size_t> used;
      for (auto& neighbor : adj[id])
        if (result.contains(neighbor)) used.insert(result.at(neighbor));

      bool fit = false;
      for (int i = 0; i < regCount; i++) {
        if (!used.contains(i)) {
          result[value] = i;
          fit = true;
          break;
        }
      }
      if (!fit) throw std::runtime_error("all registers are busy");
    }
  }

  for (auto& func : mod) {
    if (func.arg_size() == 2) {
      result[func.getArg(0)] = 13;
      result[func.getArg(1)] = 14;

    } else if (func.arg_size() == 3) {
      result[func.getArg(0)] = 13;
      result[func.getArg(1)] = 14;
      result[func.getArg(2)] = 15;
    }
  }

  return result;
}
