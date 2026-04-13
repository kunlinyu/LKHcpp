#include <plog/Log.h>

#include <climits>
#include <unordered_map>

#include "LKH.h"
#include "candidate/CandidateFuncs.h"
#include "data/Candidate.h"
#include "data/Context.h"

/*
 * The OrderCandidateSet function augments the candidate set by using
 * transitive relatations in the following way. If the edges (i,j) and (j,k)
 * are contained the candidate set, then the edge (i,k) is added to the
 * candidate set. The alpha-value of each candidate edge is computed, and
 * the candidate edges associated with each node are ordered according to
 * their Alpha-values.
 *
 * The parameter MaxCandidates specifies the maximum number of candidate
 * edges allowed for each node, and MaxAlpha puts an upper limit on their
 * Alpha-values.
 *
 * A non-zero value of Symmetric specifies that the candidate set is to be
 * complemented such that every candidate edge is associated with both its
 * two end nodes (in this way MaxCandidates may be exceeded).
 */

struct AlphaContext {
  int level;
  bool computed;
  Node *ancestor;
  int beta;
};

static int BetaValue(std::unordered_map<Node *, AlphaContext> &ctx, Node *From,
                     Node *To);

void OrderCandidateSet(size_t MaxCandidates, GainType MaxAlpha, int Symmetric) {
  Node *From;
  Node *To;
  Candidate *NFrom;
  Candidate *NN;

  PLOGD << "Ordering candidates ...";
  if (MaxAlpha > INT_MAX) MaxAlpha = INT_MAX;
  // Add edges from the 1-tree to the candidate set
  if (MaxCandidates > 0) {
    From = context.FirstNode;
    do {
      if ((To = From->Dad)) {
        AddCandidate(From, To, From->Cost, 0);
        AddCandidate(To, From, From->Cost, 0);
      }
    } while ((From = From->SucNode()) != context.FirstNode);
    AddCandidate(context.FirstNode, context.FirstNode->Next,
                 context.FirstNode->NextCost, 0);
    AddCandidate(context.FirstNode->Next, context.FirstNode,
                 context.FirstNode->NextCost, 0);
  }

  std::unordered_map<Node *, AlphaContext> ctx;

  From = context.FirstNode;
  do {
    ctx[From].computed = false;
  } while ((From = From->SucNode()) != context.FirstNode);

  From = context.FirstNode->SucNode();
  ctx[From].level = 0;
  ctx[From].ancestor = From;
  ctx[From].beta = INT_MIN;

  From = From->SucNode();
  do {
    ctx[From].ancestor = To = From->Dad;
    ctx[From].beta = From->Cost;
    ctx[From].level = ctx[To].level + 1;
  } while ((From = From->SucNode()) != context.FirstNode);

  // Compute Alpha-values for candidates
  do {
    for (const auto &c : From->candidates) {
      NFrom = c;
      To = NFrom->To;
      if (From == To)
        NFrom->Alpha = INT_MAX;
      else if (ctx[To].computed && (NN = To->candidates.find(From)))
        NFrom->Alpha = NN->Alpha;
      else {
        WeightType Beta = BetaValue(ctx, From, To);
        NFrom->Alpha =
            Beta != INT_MIN ? std::max((int64_t)(NFrom->Cost) - Beta, (int64_t)0) : INT_MAX;
      }
    }
    ctx[From].computed = true;
  } while ((From = From->SucNode()) != context.FirstNode);

  // Order candidates according to their Alpha-values
  ResetCandidateSet();
  if (MaxCandidates > 0) TrimCandidateSet(MaxCandidates);
  if (Symmetric) SymmetrizeCandidateSet();
  PLOGD << "done";
}

/*
 * The BetaValue function computes the largest edge cost on the path
 * between two given nodes in the minimum spanning tree.
 */

static int BetaValue(std::unordered_map<Node *, AlphaContext> &ctx, Node *From,
                     Node *To) {
  Node *N1 = From, *N2 = To;
  int Beta = INT_MIN;

  if (To == From->Dad) return From->Cost;
  if (From == To->Dad) return To->Cost;
  if (From == context.FirstNode || To == context.FirstNode)
    return context.FirstNode->NextCost;

  // Go upwards in the tree until the least common ancestor is met
  while (ctx[N1].ancestor != ctx[N2].ancestor) {
    if (ctx[N1].level > ctx[N2].level) {
      if (ctx[N1].beta > Beta) Beta = ctx[N1].beta;
      N1 = ctx[N1].ancestor;
    } else {
      if (ctx[N2].beta > Beta) Beta = ctx[N2].beta;
      N2 = ctx[N2].ancestor;
    }
  }
  if (N1 == N2) return Beta;
  return std::max(Beta, std::max(ctx[N1].beta, ctx[N2].beta));
}
