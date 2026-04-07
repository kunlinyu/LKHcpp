#include <plog/Log.h>

#include <climits>
#include <cmath>
#include <limits>
#include <map>

#include "Graph.h"
#include "candidate/CandidateFuncs.h"
#include "data/Candidate.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"

/*
 * The Ascent function computes a lower bound on the optimal tour length
 * using subgradient optimization. The function also transforms the original
 * problem into a problem in which the Alpha-values reflect the likelihood
 * of edges being optimal.
 *
 * The function attempts to find penalties (Pi-values) that maximizes the
 * lower bound L(T(Pi)) - 2*PiSum, where L(T(Pi)) denotes the length of the
 * minimum spanning 1-tree computed from the transformed distances, and PiSum
 * denotes the sum of Pi-values. If C(i,j) denotes the length of an edge
 * (i,j), then the transformed distance D(i,j) of an edge is
 * C(i,j) + Pi(i) + Pi(j).
 *
 * The Minimum1TreeCost function is used to compute the cost of a minimum
 * 1-tree.The Generatecandidates function is called in order to generate
 * candidate sets. Minimum 1-trees are then computed in the corresponding
 * sparse graph.
 */

GainType Ascent(POpMUSICCandicateSetCreator& popmusic) {
  Node* t;
  GainType BestW, W, W0, Alpha, MaxAlpha;
  int T, Period, P, InitialPhase, BestNorm;

Start:
  // Initialize Pi
  t = context.FirstNode;
  do t->Pi = 0;
  while (Node::MoveSuc(t) != context.FirstNode);

  std::map<Node*, int> best_pi;

  if (param.candidate_set_type == POPMUSIC &&
      context.FirstNode->candidates.empty())
    popmusic.CreateCandidateSet(param.ascent_candidates);

  std::map<Node*, int> degree_2;

  // Compute the cost of a minimum 1-tree
  W = Minimum1TreeCost(
      param.candidate_set_type == POPMUSIC || param.max_candidates == 0,
      degree_2);

  // Return this cost if either
  // (1) subgradient optimization is not wanted, or
  // (2) the norm of the tree (its deviation from a tour) is zero
  // (in that case the true optimum has been found).
  if (!context.Norm) return W;

  if (param.max_candidates > 0) {
    // Generate symmetric candidate sets for all nodes
    MaxAlpha = INT_MAX;
    if (context.Optimum != std::numeric_limits<GainType>::min() &&
        (Alpha = context.Optimum - W) >= 0)
      MaxAlpha = Alpha;
    if (param.candidate_set_type != POPMUSIC)
      GenerateCandidates(param.ascent_candidates, MaxAlpha, 1);
    else {
      OrderCandidateSet(param.ascent_candidates, MaxAlpha, 1);
      W = Minimum1TreeCost(1, degree_2);
      if (!context.Norm || W == context.Optimum) return W;
    }
  }
  PLOGD << CandidateReport(context.FirstNode);
  PLOGD << "Subgradient optimization ...";

  // Set last (d - 2) of every node to V (the node's degree in the 1-tree)
  std::map<Node*, int> last_d2 = degree_2;

  BestW = W0 = W;
  BestNorm = context.Norm;
  InitialPhase = 1;
  // Perform subradient optimization with decreasing period length and
  // decreasing step size
  for (Period = param.initial_period, T = param.initial_step_size;
       Period > 0 && T > 0 && context.Norm != 0; Period /= 2, T /= 2) {
    // Period and step size are halved at each iteration
    PLOGD << "  T = " << T << ", Period = " << Period
          << ", BestW = " << (double)BestW << ", BestNorm = " << BestNorm;
    for (P = 1; T && P <= Period && context.Norm != 0; P++) {
      // Adjust the Pi-values
      t = context.FirstNode;
      do {
        if (degree_2[t] != 0) {
          t->Pi += T * (7 * degree_2[t] + 3 * last_d2[t]) / 10;
          if (t->Pi > INT_MAX / 10)
            t->Pi = INT_MAX / 10;
          else if (t->Pi < INT_MIN / 10)
            t->Pi = INT_MIN / 10;
        }
        last_d2[t] = degree_2[t];
      } while (Node::MoveSuc(t) != context.FirstNode);
      // Compute a minimum 1-tree in the sparse graph
      W = Minimum1TreeCost(1, degree_2);
      // Test if an improvement has been found
      if (W > BestW || (W == BestW && context.Norm < BestNorm)) {
        // If the lower bound becomes greater than twice its initial value
        // it is taken as a sign that the graph might be too sparse
        if (W - W0 > (W0 >= 0 ? W0 : -W0) && param.ascent_candidates > 0 &&
            param.ascent_candidates < problem.dimension) {
          W = Minimum1TreeCost(
              param.candidate_set_type == POPMUSIC || param.max_candidates == 0,
              degree_2);
          if (W < W0) {
            // Double the number of candidate edges and start all
            // over again
            PLOGD << "Warning: AscentCandidates doubled";
            if ((param.ascent_candidates *= 2) > problem.dimension)
              param.ascent_candidates = problem.dimension;
            goto Start;
          }
          W0 = W;
        }
        BestW = W;
        BestNorm = context.Norm;
        // Update the BestPi-values
        t = context.FirstNode;
        do {
          best_pi[t] = t->Pi;
        } while (Node::MoveSuc(t) != context.FirstNode);
        PLOGD << "* T = " << T << ", Period = " << Period
              << ", BestW = " << (double)BestW << ", BestNorm = " << BestNorm;
        // If in the initial phase, the step size is doubled
        if (InitialPhase && T * sqrt((double)context.Norm) > 0) T *= 2;
        // If the improvement was found at the last iteration of the
        // current period, then double the period
        if (param.candidate_set_type != POPMUSIC && P == Period &&
            (Period *= 2) > param.initial_period)
          Period = param.initial_period;
      } else {
        PLOGD << "  T = " << T << ", Period = " << Period << ", P = " << P
              << ", W = " << (double)W << ", Norm = " << context.Norm;
        if (InitialPhase && P > Period / 2) {
          // Conclude the initial phase
          InitialPhase = 0;
          P = 0;
          T = 3 * T / 4;
        }
      }
    }
  }

  t = context.FirstNode;
  do {
    t->Pi = best_pi[t];
  } while (Node::MoveSuc(t) != context.FirstNode);

  // Compute a minimum 1-tree
  W = BestW = Minimum1TreeCost(
      param.candidate_set_type == POPMUSIC || param.max_candidates == 0,
      degree_2);

  if (param.max_candidates > 0 && param.candidate_set_type != POPMUSIC) {
    for (auto& node : context.node_set.data) node.candidates.clear();
  } else {
    t = context.FirstNode;
    do {
      for (const auto& c : t->candidates) c->Cost += t->Pi + c->To->Pi;
    } while (Node::MoveSuc(t) != context.FirstNode);
  }
  PLOGD << "Ascent: BestW = " << (double)BestW << ", Norm = " << context.Norm;
  return W;
}
