#include <plog/Log.h>

#include <cassert>
#include <cmath>
#include <functional>

#include "ActiveNodeQueue.h"
#include "Function.h"
#include "HashTable.h"
#include "LKH.h"
#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "utils/GetTime.h"

/*
 * The LinKernighan function seeks to improve a tour by sequential
 * and non-sequential edge exchanges.
 *
 * The function returns the cost of the resulting tour.
 */
void StoreTour(const std::function<void(Node *)> &enqueue);
GainType LinKernighan() {
  GainType Gain;
  int log_counter = 0;
  int X2, i;
  Node *t1, *t2, *SUCt1;
  Candidate *Nt1;
  TreeNode *S;
  TreeNode *SS;
  double EntryTime = GetTime();

  context.Reversed = false;
  S = &context.segments.front();
  i = 0;
  do {
    S->Size = 0;
    S->Rank = ++i;
    S->Reversed = false;
    S->First = S->Last = nullptr;
  } while ((S = S->Suc) != &context.segments.front());
  SS = &context.super_segments.front();
  i = 0;
  do {
    SS->Size = 0;
    SS->Rank = ++i;
    SS->Reversed = false;
    SS->First = SS->Last = nullptr;
  } while ((SS = SS->Suc) != &context.super_segments.front());

  context.swap_stack.clear();

  /* Compute the cost of the initial tour, Cost.
     Compute the corresponding hash value, Hash.
     Initialize the segment list.
     Make all nodes "active" (so that they can be used as t1). */

  GainType Cost = 0;
  context.hash_table.reset_hash();
  i = 0;
  t1 = context.FirstNode;
  ActiveQueue active_queue;
  do {
    t2 = t1->OldSuc = t1->SucNode();
    t1->OldPrd = t1->PrdNode();
    t1->Rank = ++i;
    Cost += context.C(t1, t2) - t1->Pi - t2->Pi;
    context.hash_table.update_hash(t1->index, t2->index);
    t1->Cost = INT_MAX;
    for (const auto &c : t1->candidates) {
      Nt1 = c;
      t2 = Nt1->To;
      if (t2 != t1->Prd && t2 != t1->Suc && Nt1->Cost < t1->Cost)
        t1->Cost = Nt1->Cost;
    }
    t1->Parent = S;
    S->Size++;
    if (S->Size == 1) S->First = t1;
    S->Last = t1;
    if (SS->Size == 0) SS->First = S;
    S->Parent = SS;
    SS->Last = S;
    if (S->Size == context.SegmentSize) {
      S = S->Suc;
      SS->Size++;
      if (SS->Size == context.SuperSegmentSize) SS = SS->Suc;
    }
    t1->ResetExcluded();
    t1->Next = nullptr;
    active_queue.push(t1);
  } while ((t1 = t1->SucNode()) != context.FirstNode);
  if (S->Size < context.SegmentSize) SS->Size++;
  PLOGV << StatusReport(Cost, EntryTime, "");
  PLOGD_IF(Cost < context.BetterCost) << StatusReport(Cost, EntryTime, "");

  /* Loop as long as improvements are found */
  do {
    /* Choose t1 as the first "active" node */
    // while ((t1 = RemoveFirstActive())) {
    while ((t1 = active_queue.pop())) {
      if (GetTime() - EntryTime >= param.time_limit ||
          GetTime() - context.StartTime >= param.total_time_limit) {
        PLOGI << "*** Time limit exceeded";
        goto End_LinKernighan;
      }
      // t1 is now "passive"
      SUCt1 = SUC(t1);
      if (++log_counter % (int)pow(10, (int)log10(context.dimension - 1)) ==
          0) {
        PLOGD << "#" << log_counter << ": Time = " << GetTime() - EntryTime
              << " sec.";
        PLOGI_IF(context.Trial == 1)
            << "#" << log_counter << ": Time = " << GetTime() - EntryTime
            << " sec.";
      }
      // Choose t2 as one of t1's two neighbors on the tour
      for (X2 = 1; X2 <= 2; X2++) {
        t2 = X2 == 1 ? PRD(t1) : SUCt1;
        if (param.restricted_search && Near(t1, t2)) continue;
        GainType G0 = context.C(t1, t2);
        // Try to find a tour-improving chain of moves
        do
          t2 = context.swap_stack.size() == 0
                   ? context.BestMove(t1, t2, &G0, &Gain)
                   : context.BestSubsequentMove(t1, t2, &G0, &Gain);
        while (t2);
        if (Gain > 0) {
          // An improvement has been found
          Cost -= Gain;
          StoreTour([&active_queue](Node *n) { active_queue.push(n); });
          PLOGV << StatusReport(Cost, EntryTime, "");
          PLOGD_IF(Cost < context.BetterCost)
              << StatusReport(Cost, EntryTime, "");
          if (context.hash_table.search(Cost)) goto End_LinKernighan;
          /* Make t1 "active" again */
          active_queue.push(t1);
          context.swap_stack.clear_check_point();
          break;
        }
        context.swap_stack.clear_check_point();
        RestoreTour();
      }
    }
    if (context.hash_table.search(Cost)) goto End_LinKernighan;
    context.hash_table.insert(Cost);
    /* Try to find improvements using non-sequential 4/5-opt moves */
    Gain = 0;
    if (param.gain23_used && (Gain = Gain23()) > 0) {
      /* An improvement has been found */
      Cost -= Gain;
      StoreTour([&active_queue](Node *n) { active_queue.push(n); });
      PLOGV << StatusReport(Cost, EntryTime, " + ");
      PLOGD_IF(Cost < context.BetterCost)
          << StatusReport(Cost, EntryTime, " + ");
      if (context.hash_table.search(Cost)) goto End_LinKernighan;
    }
  } while (Gain > 0);
End_LinKernighan:
  NormalizeNodeList();
  NormalizeSegmentList();
  context.Reversed = false;
  return Cost;
}
