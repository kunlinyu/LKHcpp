#include <plog/Log.h>

#include <functional>

#include "LKH.h"
#include "data/Context.h"

/*
 * The MergeWithTourIPT function attempts to find a short tour
 * by merging a given tour, T1, with another tour, T2.
 * T1 is given by the Suc pointers of its nodes.
 * T2 is given by the Next pointers of its nodes.
 *
 * The merging algorithm may be described as follows:
 * Let G be the graph consisting of the nodes and the union of the
 * edges of T1 and T2. Attempt - in all possible ways -
 * to separate the nodes of G into two disjoint sets (A,B) such
 * that the cardinality of edges connecting A with B is exactly two.
 * If this is possible, any replacement of T1's A-edges with T2's
 * A-edges results in a tour. The same holds for the B-edges.
 * If such a replacement reduces the cost of one the tours, then make it.
 *
 * If a tour T shorter than T2 is found, Pred and Suc of each node
 * point to its neighbors in T, and T's cost is returned.
 *
 * The function is called from the FindTour function.
 *
 * The implementation is inspired by the algorithm described in the
 * paper
 *
 *   A. Mobius, B. Freisleben, P. Merz, and M. Schreiber,
 *   "Combinatorial Optimization by Iterative Partial Transcription",
 *   Physical Review E, Volume 59, Number 4, pp. 4667-4674, 1999.
 */

GainType MergeWithTourIPT() {
  int Rank = 0, Improved1 = 0, Improved2 = 0;
  int SubSize1, SubSize2, MaxSubSize1, NewDimension = 0, Forward;
  int MinSubSize, BestMinSubSize = 3, MinForward = 0;
  GainType Cost1 = 0, Cost2 = 0, Gain, OldCost1, MinGain = 0;
  Node *N, *NNext, *N1, *N2, *MinN1, *MinN2, *First = 0, *Last;

  std::set<Node*> active;

  N = context.FirstNode;
  do {
    N->Suc->Prd = N->Next->Prev = N;
  } while ((N = N->SucNode()) != context.FirstNode);
  do {
    Cost1 += N->Cost = context.C(N, N->SucNode()) - N->Pi - N->SucNode()->Pi;
    if ((N->Suc != N->Prev && N->Suc != N->Next) ||
        (N->Prd != N->Prev && N->Prd != N->Next)) {
      active.insert(N);
      NewDimension++;
      First = N;
    }
  } while ((N = N->SucNode()) != context.FirstNode);
  if (NewDimension == 0) {
    return Cost1;
  }

  do {
    Cost2 += N->NextCost = N->Next == N->Prd ? N->PrdNode()->Cost
                           : N->Next == N->Suc
                               ? N->Cost
                               : context.C(N, N->Next) - N->Pi - N->Next->Pi;
  } while ((N = N->SucNode()) != context.FirstNode);
  OldCost1 = Cost1;

  /* Shrink the tours.
     OldPred and OldSuc represent the shrunken T1.
     Prev and Next represent the shrunken T2 */
  N = First;
  Last = 0;
  do {
    if (active.find(N) != active.end()) {
      N->Rank = ++Rank;
      if (Last) {
        (Last->OldSuc = N)->OldPrd = Last;
        if (Last != N->Prd) Last->Cost = 0;
      }
      Last = N;
    }
  } while ((N = N->SucNode()) != First);
  (Last->OldSuc = First)->OldPrd = Last;
  if (Last != First->Prd) Last->Cost = 0;
  N = First;
  Last = 0;
  do {
    if (active.find(N) != active.end()) {
      if (Last) {
        Last->Next = N;
        if (Last != N->Prev) {
          N->Prev = Last;
          Last->NextCost = 0;
        }
      }
      Last = N;
    }
  } while ((N = N->Next) != First);
  Last->Next = First;
  if (Last != First->Prev) {
    First->Prev = Last;
    Last->NextCost = 0;
  }

  /* Merge the shrunken tours */
  do {
    MinN1 = MinN2 = 0;
    MinSubSize = NewDimension / 2;
    N1 = First;
    do {
      while (N1->OldSuc != First &&
             (N1->OldSuc == N1->Next || N1->OldSuc == N1->Prev))
        N1 = N1->OldSuc;
      if (N1->OldSuc == First &&
          (N1->OldSuc == N1->Next || N1->OldSuc == N1->Prev))
        break;
      for (Forward = 1, N2 = N1->Next; Forward >= 0; Forward--, N2 = N1->Prev) {
        if (N2 == N1->OldSuc || N2 == N1->OldPrd) continue;
        SubSize2 = MaxSubSize1 = 0;
        do {
          if (++SubSize2 >= MinSubSize) break;
          if ((SubSize1 = N2->Rank - N1->Rank) < 0) SubSize1 += NewDimension;
          if (SubSize1 >= MinSubSize) break;
          if (SubSize1 > MaxSubSize1) {
            if (SubSize1 == SubSize2) {
              for (N = N1, Gain = 0; N != N2; N = N->OldSuc)
                Gain += N->Cost - N->NextCost;
              if (!Forward) Gain += N1->NextCost - N2->NextCost;
              if (Gain != 0) {
                MinSubSize = SubSize1;
                MinN1 = N1;
                MinN2 = N2;
                MinGain = Gain;
                MinForward = Forward;
              }
              break;
            }
            MaxSubSize1 = SubSize1;
          }
        } while ((N2 = Forward ? N2->Next : N2->Prev) != N1);
      }
    } while ((N1 = N1->OldSuc) != First && MinSubSize != BestMinSubSize);
    if (MinN1) {
      BestMinSubSize = MinSubSize;
      if (MinGain > 0) {
        Improved1 = 1;
        Cost1 -= MinGain;
        Rank = MinN1->Rank;
        for (N = MinN1; N != MinN2; N = NNext) {
          NNext = MinForward ? N->Next : N->Prev;
          (N->OldSuc = NNext)->OldPrd = N;
          N->Rank = Rank;
          N->Cost = MinForward ? N->NextCost : NNext->NextCost;
          if (++Rank > NewDimension) Rank = 1;
        }
      } else {
        Improved2 = 1;
        Cost2 += MinGain;
        for (N = MinN1; N != MinN2; N = N->OldSuc) {
          if (MinForward) {
            (N->Next = N->OldSuc)->Prev = N;
            N->NextCost = N->Cost;
          } else {
            (N->Prev = N->OldSuc)->Next = N;
            N->Prev->NextCost = N->Cost;
          }
        }
        if (MinForward)
          MinN2->Prev = N->OldPrd;
        else {
          MinN2->Next = N->OldPrd;
          MinN2->NextCost = N->OldPrd->Cost;
        }
      }
      First = MinForward ? MinN2 : MinN1;
    }
  } while (MinN1);

  if (Cost1 < Cost2   ? !Improved1
      : Cost2 < Cost1 ? !Improved2
                      : !Improved1 || !Improved2) {
    return OldCost1;
  }

  // Expand the best tour into a full tour
  N = First;
  std::set<Node*> visited;
  visited.insert(N);
  std::function<bool(Node*)> Visited = [&visited](Node* node) {
    return visited.find(node) != visited.end();
  };
  do {
    if (!Visited(N->SucNode()) && (active.find(N) == active.end() ||
                                   active.find(N->SucNode()) == active.end()))
      N->OldSuc = N->SucNode();
    else if (!Visited(N->PrdNode()) &&
             (active.find(N) == active.end() ||
              active.find(N->PrdNode()) == active.end()))
      N->OldSuc = N->PrdNode();
    else if (Cost1 <= Cost2) {
      if (Visited(N->OldSuc))
        N->OldSuc = !Visited(N->OldPrd) ? N->OldPrd : First;
    } else if (!Visited(N->Next))
      N->OldSuc = N->Next;
    else if (!Visited(N->Prev))
      N->OldSuc = N->Prev;
    else
      N->OldSuc = First;
    visited.insert(N);
  } while ((N = N->OldSuc) != First);
  do {
    N->OldSuc->Prd = N;
    N->Suc = N->OldSuc;
  } while ((N = N->OldSuc) != First);
  context.hash_table.reset_hash();
  N = First;
  do context.hash_table.update_hash(N->index, N->SucNode()->index);
  while ((N = N->SucNode()) != First);
  GainType Cost = (Cost1 <= Cost2 ? Cost1 : Cost2);
  LOGD << "IPT: " << Cost;
  return Cost;
}