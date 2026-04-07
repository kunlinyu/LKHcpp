#pragma once

#include <string>
#include "ProblemType.h"

struct Problem {
  std::string name;
  ProblemType type = UNKNOW_PROBLEM;
  int dimension = 0;
};

extern thread_local Problem problem;
