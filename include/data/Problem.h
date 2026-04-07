#pragma once

#include <string>
#include "ProblemType.h"

struct Problem {
  std::string name;
  ProblemType type = UNKNOW_PROBLEM;
  int dimension = 0;

  bool IsAsymmetric() const { return false; }
};

extern thread_local Problem problem;
