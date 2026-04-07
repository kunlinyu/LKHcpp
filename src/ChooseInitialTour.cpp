#include "LKH.h"
#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"

/*
 * The ChooseInitialTour function generates a pseudo-random initial tour.
 * The algorithm constructs a tour as follows.
 *
 * First, a random node N is chosen.
 *
 * Then, as long as no all nodes have been chosen, choose the next node to
 * follow N in the tour, NextN, and set N equal to NextN.
 *
 * NextN is chosen as follows:
 *
 *  (A) If possible, and Trial = 1, choose NextN such that
 *      (N,NextN) is an edge of a given initial tour.
 *  (B) Otherwise, if  possible, choose NextN such that (N,NextN) is a
 *      fixed edge, or is common to two or more tours to be merged.
 *  (C) Otherwise, if possible, choose NextN so that (N,NextN) is a
 *      candidate edge, the alpha-value of (N,NextN) is zero, and (N,NextN)
 *      belongs to the current best or next best tour.
 *  (D) Otherwise, if possible, choose NextN such that (N,NextN) is a
 *      candidate edge.
 *  (E) Otherwise, choose NextN at random among those nodes not already
 *      chosen.
 *
 *  When more than one node may be chosen, the node is chosen at random
 *  among the alternatives (a one-way list of nodes).
 *
 *  The sequence of chosen nodes constitutes the initial tour.
 */

void ChooseInitialTour(Node* FirstNode) {
  Node *N, *NextN, *FirstAlternative, *Last;
  Candidate* NN;
  int Alternatives, i;

  std::set<Node*> chosen;

Start:
  chosen.clear();
  N = FirstNode;
  int Count = 0;

  FirstNode = N;

  /* Move nodes with two incident fixed or common candidate edges in
     front of FirstNode */
  for (Last = FirstNode->PrdNode(); N != Last; N = NextN) {
    NextN = N->SucNode();
  }

  // Mark FirstNode as chosen
  chosen.insert(FirstNode);
  N = FirstNode;

  /* Loop as long as not all nodes have been chosen */
  while (N->Suc != FirstNode) {
    FirstAlternative = 0;
    Alternatives = 0;
    Count++;

    /* Case A */

    /* Case B */
    if (Alternatives == 0) {
    }
    if (Alternatives == 0 && context.Trial > 1) {
      /* Case C */
      for (const auto& c : N->candidates) {
        NN = c;
        NextN = NN->To;
        if (chosen.find(NextN) == chosen.end() && NN->Alpha == 0 &&
            (InBestTour(N, NextN) || InNextBestTour(N, NextN))) {
          Alternatives++;
          NextN->Next = FirstAlternative;
          FirstAlternative = NextN;
        }
      }
    }
    if (Alternatives == 0) {
      /* Case D */
      for (const auto& c : N->candidates) {
        NN = c;
        NextN = NN->To;
        if (chosen.find(NextN) == chosen.end()) {
          Alternatives++;
          NextN->Next = FirstAlternative;
          FirstAlternative = NextN;
        }
      }
    }
    if (Alternatives == 0) {
      /* Case E (actually not really a random choice) */
      NextN = N->SucNode();
      while (N == NextN && NextN->Suc != FirstNode) NextN = NextN->SucNode();
      if (N == NextN) {
        FirstNode = N;
        goto Start;
      }
    } else {
      NextN = FirstAlternative;
      if (Alternatives > 1) {
        /* Select NextN at random among the alternatives */
        i = Random() % Alternatives;
        while (i--) NextN = NextN->Next;
      }
    }
    /* Include NextN as the successor of N */
    Follow(NextN, N);
    N = NextN;
    chosen.insert(N);
  }
  if (N == N->SucNode()) {
    FirstNode = N;
    goto Start;
  }
  if (param.max_trials == 0) {
    GainType Cost = 0;
    N = FirstNode;
    do Cost += context.C(N, N->SucNode()) - N->Pi - N->SucNode()->Pi;
    while ((N = N->SucNode()) != FirstNode);
    if (Cost < context.BetterCost) {
      context.BetterCost = Cost;
      RecordBetterTour(context.BetterTour, FirstNode);
    }
  }
}
