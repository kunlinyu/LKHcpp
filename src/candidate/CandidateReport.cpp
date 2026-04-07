#include <limits>
#include <sstream>

#include "data/Node.h"
#include "data/Problem.h"

/*
 * The CandidateReport function prints the minimum, average and maximum
 * number of candidates associated with a node.
 */

std::string CandidateReport(Node *FirstNode) {
  size_t Min = std::numeric_limits<size_t>::max();
  size_t Max = std::numeric_limits<size_t>::min();
  size_t Sum = 0;

  Node *N = FirstNode;
  do {
    const size_t size = N->candidates.size();
    if (size > Max) Max = size;
    if (size < Min) Min = size;
    Sum += size;
  } while ((N = N->SucNode()) != FirstNode);
  std::stringstream ss;
  ss << "Cand.min = " << Min
     << ", Cand.avg = " << (double)Sum / problem.dimension
     << ", Cand.max = " << Max;
  return ss.str();
}
