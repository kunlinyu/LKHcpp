#include <cmath>
#include <iomanip>
#include <sstream>

#include "C.h"
#include "LKH.h"
#include "candidate/CandidateFuncs.h"
#include "data/Context.h"
#include "data/Param.h"
#include "plog/Log.h"
#include "utils/GetTime.h"

/*
 * The CreateCandidateSet function determines for each node its set of incident
 * candidate edges.
 *
 * The Ascent function is called to determine a lower bound on the optimal tour
 * using subgradient optimization. But only if the penalties (the Pi-values) is
 * not available on file. In the latter case, the penalties is read from the
 * file, and the lower bound is computed from a minimum 1-tree.
 *
 * The function GenerateCandidates is called to compute the Alpha-values and to
 * associate to each node a set of incident candidate edges.
 *
 * The CreateCandidateSet function itself is called from LKHmain.
 */

double CreateCandidateSet(POpMUSICCandicateSetCreator &popmusic) {
  double EntryTime = GetTime();

  context.Norm = std::numeric_limits<int>::max();
  PLOGD << "Creating candidates ...";

  Node *Na = context.FirstNode;
  do Na->Pi = 0;
  while ((Na = Na->SucNode()) != context.FirstNode);
  GainType Cost = Ascent(popmusic);

  double LowerBound = (double)Cost;

  std::stringstream ss;
  ss << "Lower bound = " << LowerBound;
  if (context.Optimum != std::numeric_limits<GainType>::min() &&
      context.Optimum != 0)
    ss << ", Gap = " << std::fixed << std::setprecision(4)
       << 100.0 * (Cost - context.Optimum) / context.Optimum << "%";
  ss << ", Ascent time = " << std::fixed << std::setprecision(2)
     << fabs(GetTime() - EntryTime) << " sec.";
  PLOGI << ss.str();

  GainType MaxAlpha = (GainType)fabs(param.excess * Cost);
  GainType A = context.Optimum - Cost;
  if (context.Optimum != std::numeric_limits<GainType>::min() &&
      context.Optimum != 0 && A > 0 && A < MaxAlpha)
    MaxAlpha = A;
  if (MaxAlpha == 0) MaxAlpha = INT_MAX;
  if (param.candidate_set_type == POPMUSIC || param.max_candidates == 0)
    OrderCandidateSet(param.max_candidates, MaxAlpha,
                      param.candidate_set_symmetric);
  else
    GenerateCandidates(param.max_candidates, MaxAlpha,
                       param.candidate_set_symmetric);

  ResetCandidateSet();
  if (param.max_trials > 0) {
    Na = context.FirstNode;
    do {
      if (Na->candidates.empty()) {
        if (param.max_candidates == 0)
          PLOGF << "MAX_CANDIDATES = 0: Node " << Na->Id
                << " has no candidates";
        else
          PLOGF << "Node " << Na->Id << " has no candidates";
      }
    } while ((Na = Na->SucNode()) != context.FirstNode);
  }
  Na = context.FirstNode;
  do
    for (int i = 1; i <= problem.dimension; i++)
      Na->C[i - 1] += Na->Pi + context.node_set.ref(i).Pi;
  while ((Na = Na->SucNode()) != context.FirstNode);
  PLOGI << CandidateReport(context.FirstNode);
  PLOGI << "Preprocessing time = " << std::fixed << std::setprecision(2)
        << fabs(GetTime() - EntryTime) << " sec.";

  return LowerBound;
}