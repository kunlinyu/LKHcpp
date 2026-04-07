#pragma once

#include "ProblemType.h"

struct Problem {
  ProblemType type = UNKNOW_PROBLEM;
  int dimension = 0;
};

extern thread_local Problem problem;
