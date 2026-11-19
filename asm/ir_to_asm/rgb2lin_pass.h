#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "isa.h"

inline void rgb2LinPass(std::vector<std::unique_ptr<Opcode>>& code) {
  auto nextIdx = [&](size_t idx) -> size_t {
    idx++;
    while (idx < code.size() && code[idx]->type == OpcodeType::Dummy) {
      idx++;
    }
    return idx;
  };

  auto approxEq = [](double a, double b) -> bool {
    return std::abs(a - b) < 1e-6;
  };

  for (size_t i = 0; i < code.size(); i++) {
    if (code[i]->type != OpcodeType::OpUiToFp) continue;

    std::vector<size_t> seq;
    size_t curr = i;
    while (curr < code.size() && seq.size() < 14) {
      seq.push_back(curr);
      curr = nextIdx(curr);
    }

    if (seq.size() < 14) continue;

    if (code[seq[1]]->type != OpcodeType::OpFDivI ||
        code[seq[2]]->type != OpcodeType::OpFGtI ||
        code[seq[3]]->type != OpcodeType::OpCJmp ||
        code[seq[4]]->type != OpcodeType::Block ||
        code[seq[5]]->type != OpcodeType::OpFDivI ||
        code[seq[6]]->type != OpcodeType::OpJmp ||
        code[seq[7]]->type != OpcodeType::Block ||
        code[seq[8]]->type != OpcodeType::OpFAddI ||
        code[seq[9]]->type != OpcodeType::OpFDivI ||
        code[seq[10]]->type != OpcodeType::OpMov ||
        code[seq[11]]->type != OpcodeType::OpMov ||
        code[seq[12]]->type != OpcodeType::OpCallEV ||
        code[seq[13]]->type != OpcodeType::Block) {
      continue;
    }

    auto* uitofp = static_cast<UiToFp*>(code[seq[0]].get());
    auto* fdiv255 = static_cast<FDivI*>(code[seq[1]].get());
    auto* fgti = static_cast<FGtI*>(code[seq[2]].get());
    auto* cjmp = static_cast<CJmp*>(code[seq[3]].get());
    auto* b1 = static_cast<Block*>(code[seq[4]].get());
    auto* fdiv12 = static_cast<FDivI*>(code[seq[5]].get());
    auto* jmpB3 = static_cast<Jmp*>(code[seq[6]].get());
    auto* b2 = static_cast<Block*>(code[seq[7]].get());
    auto* fadd = static_cast<FAddI*>(code[seq[8]].get());
    auto* fdiv1055 = static_cast<FDivI*>(code[seq[9]].get());
    auto* callev = static_cast<CallEV*>(code[seq[12]].get());
    auto* b3 = static_cast<Block*>(code[seq[13]].get());

    if (!approxEq(fdiv255->rhs, 255.0) || !approxEq(fgti->rhs, 0.04045) ||
        !approxEq(fdiv12->rhs, 12.92) || !approxEq(fadd->rhs, 0.055) ||
        !approxEq(fdiv1055->rhs, 1.055)) {
      continue;
    }

    if (cjmp->falseTarget != b1->id || cjmp->trueTarget != b2->id ||
        jmpB3->target != b3->id) {
      continue;
    }

    code[seq[0]] = std::make_unique<Rgb2Lin>(callev->dst, uitofp->src);
    for (size_t k = 1; k <= 13; k++) {
      code[seq[k]] = std::make_unique<Dummy>();
    }
  }
}

inline void lin2RgbPass(std::vector<std::unique_ptr<Opcode>>& code) {
  auto nextIdx = [&](size_t idx) -> size_t {
    idx++;
    while (idx < code.size() && code[idx]->type == OpcodeType::Dummy) {
      idx++;
    }
    return idx;
  };

  auto approxEq = [](double a, double b) -> bool {
    return std::abs(a - b) < 1e-6;
  };

  for (size_t i = 0; i < code.size(); i++) {
    if (code[i]->type != OpcodeType::OpFGtI) continue;

    std::vector<size_t> seq;
    size_t curr = i;
    while (curr < code.size() && seq.size() < 14) {
      seq.push_back(curr);
      curr = nextIdx(curr);
    }

    if (seq.size() < 14) continue;

    if (code[seq[1]]->type  != OpcodeType::OpCJmp   ||
        code[seq[2]]->type  != OpcodeType::Block    ||
        code[seq[3]]->type  != OpcodeType::OpFMulI  ||
        code[seq[4]]->type  != OpcodeType::OpJmp    ||
        code[seq[5]]->type  != OpcodeType::Block    ||
        code[seq[6]]->type  != OpcodeType::OpMov    ||
        code[seq[7]]->type  != OpcodeType::OpMov    ||
        code[seq[8]]->type  != OpcodeType::OpCallEV ||
        code[seq[9]]->type  != OpcodeType::OpMov    ||
        code[seq[10]]->type != OpcodeType::OpMov    ||
        code[seq[11]]->type != OpcodeType::OpMov    ||
        code[seq[12]]->type != OpcodeType::OpCallEV ||
        code[seq[13]]->type != OpcodeType::Block) {
      continue;
    }

    auto* fgti   = static_cast<FGtI*>(code[seq[0]].get());
    auto* cjmp   = static_cast<CJmp*>(code[seq[1]].get());
    auto* b1     = static_cast<Block*>(code[seq[2]].get());
    auto* fmul12 = static_cast<FMulI*>(code[seq[3]].get());
    auto* jmpB3  = static_cast<Jmp*>(code[seq[4]].get());
    auto* b2     = static_cast<Block*>(code[seq[5]].get());
    auto* call2  = static_cast<CallEV*>(code[seq[12]].get());
    auto* b3     = static_cast<Block*>(code[seq[13]].get());

    if (!approxEq(fgti->rhs, 0.0031308) || !approxEq(fmul12->rhs, 12.92)) {
      continue;
    }

    if (cjmp->falseTarget != b1->id ||
        cjmp->trueTarget  != b2->id ||
        jmpB3->target     != b3->id) {
      continue;
    }

    code[seq[0]] = std::make_unique<Lin2Rgb>(call2->dst, fgti->lhs);
    for (size_t k = 1; k <= 13; k++) {
      code[seq[k]] = std::make_unique<Dummy>();
    }
  }
}
