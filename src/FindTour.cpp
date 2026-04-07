#include <plog/Log.h>

#include <limits>

#include "LKH.h"
#include "candidate/CandidateFuncs.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "utils/GetTime.h"

/*
 * After the candidate set has been created the FindTour function is called
 * a predetermined number of times (Runs).
 *
 * FindTour performs a number of trials, where in each trial it attempts
 * to improve a chosen initial tour using the modified Lin-Kernighan edge
 * exchange heuristics.
 *
 * Each time a better tour is found, the tour is recorded, and the candidates
 * are reorderded by the AdjustCandidateSet function. Precedence is given to
 * edges that are common to two currently best tours. The candidate set is
 * extended with those tour edges that are not present in the current set.
 * The original candidate set is re-established at exit from FindTour.
 */

GainType CalcOrdinalTourCost() {
  GainType OrdinalTourCost = 0;
  for (size_t i = 1; i < problem.dimension; i++)
    OrdinalTourCost +=
        context.C(context.node_set.get(i), context.node_set.get(i + 1)) -
        context.node_set.get(i)->Pi - context.node_set.get(i + 1)->Pi;
  OrdinalTourCost += context.C(context.node_set.get(problem.dimension),
                               context.node_set.get(1)) -
                     context.node_set.get(problem.dimension)->Pi -
                     context.node_set.get(1)->Pi;
  return OrdinalTourCost;
}

GainType MergeWithTourIPT();

GainType FindTour(GainType OrdinalTourCost) {
  const double EntryTime = GetTime();

  Node* t = context.FirstNode;
  do {
    t->OldPrd = nullptr;
    t->OldSuc = nullptr;
    t->NextBestSuc = nullptr;
    t->BestSuc = nullptr;
  } while ((t = t->SucNode()) != context.FirstNode);

  context.BetterCost = std::numeric_limits<GainType>::max();
  if (param.max_trials <= 0) {
    context.Trial = 1;
    ChooseInitialTour(context.FirstNode);
  }
  for (context.Trial = 1; context.Trial <= param.max_trials; context.Trial++) {
    if (GetTime() - EntryTime >= param.time_limit ||
        GetTime() - context.StartTime >= param.total_time_limit) {
      PLOGI << "*** Time limit exceeded ***";
      context.Trial--;
      break;
    }
    // Choose FirstNode at random
    context.FirstNode = context.node_set.get(1 + Random() % problem.dimension);
    ChooseInitialTour(context.FirstNode);
    GainType Cost = LinKernighan();
    if (GetTime() - EntryTime < param.time_limit &&
        GetTime() - context.StartTime < param.total_time_limit) {
      if (context.FirstNode->BestSuc) {
        // Merge tour with current best tour
        t = context.FirstNode;
        while ((t = t->Next = t->BestSuc) != context.FirstNode);
        Cost = MergeWithTourIPT();
      }
      if (Cost >= OrdinalTourCost && context.BetterCost > OrdinalTourCost) {
        // Merge tour with ordinal tour
        for (int i = 1; i < problem.dimension; i++)
          context.node_set.ref(i).Next = context.node_set.get(i + 1);
        context.node_set.ref(problem.dimension).Next = context.node_set.get(1);
        Cost = MergeWithTourIPT();
      }
    }
    if (Cost < context.BetterCost) {
      PLOGI << "* " << context.Trial << ": "
            << StatusReport(Cost, EntryTime, " ");
      context.BetterCost = Cost;
      RecordBetterTour(context.BetterTour, context.FirstNode);
      if (param.stop_at_optimum) {
        if (Cost == context.Optimum) break;
      }
      AdjustCandidateSet(context.FirstNode);
      context.hash_table.clear();
      context.hash_table.insert(Cost);
    } else {
      PLOGD << "  " << context.Trial << ": "
            << StatusReport(Cost, EntryTime, " ");
    }
  }
  t = context.FirstNode;
  if (context.Norm == 0 || param.max_trials == 0 || !t->BestSuc) {
    do t = t->BestSuc = t->SucNode();
    while (t != context.FirstNode);
  }
  context.hash_table.reset_hash();
  do {
    (t->Suc = t->BestSuc)->Prd = t;
    context.hash_table.update_hash(t->index, t->SucNode()->index);
  } while ((t = t->BestSuc) != context.FirstNode);
  if (context.Trial > param.max_trials) context.Trial = param.max_trials;
  ResetCandidateSet();
  return context.BetterCost;
}
