#pragma once
#include <plog/Log.h>

#include <cmath>

#include "Move.h"
#include "candidate/CandidateFuncs.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/TSPLIB.h"
#include "data/TreeNode.h"
#include "utils/Random.h"
#include "utils/RingPair.h"

class Initializer {
 public:
  static std::vector<Node> CreateNodes(int Dimension) {
    PLOGF_IF(Dimension <= 0) << "DIMENSION is not positive (or not specified)";
    std::vector<Node> nodes;
    nodes.resize(Dimension);
    for (size_t index = 0; index < nodes.size(); ++index) {
      nodes[index].index = index;
      nodes[index].Id = nodes[index].OriginalId = index + 1;
    }
    RingPair<Node>(nodes, [](Node& a, Node& b) { Link(a, b); });
    return nodes;
  }

  static void AdjustParameters(Param& pr, int dimension) {
    if (pr.seed == 0) pr.seed = (unsigned)(time(0) * (size_t)(&pr.seed));
    if (pr.max_swaps == 0) pr.max_swaps = dimension;
    if (pr.max_candidates > dimension - 1)
      pr.max_candidates = dimension - 1;
    else {
      if (pr.ascent_candidates > dimension - 1)
        pr.ascent_candidates = dimension - 1;
      if (pr.initial_period == 0) {
        pr.initial_period = dimension / 2;
        if (pr.initial_period < 100) pr.initial_period = 100;
      }
      if (pr.excess == 0) pr.excess = 1.0 / dimension * pr.salesmen;
      if (pr.max_trials == 0) pr.max_trials = dimension;
    }
    if (pr.popmusic_max_neighbors > dimension - 1)
      pr.popmusic_max_neighbors = dimension - 1;
    if (pr.popmusic_sample_size > dimension)
      pr.popmusic_sample_size = dimension;
    PLOGF_IF(pr.salesmen > 1 and pr.salesmen < dimension)
        << "Too many salesmen/vehicles (>= DIMENSION)";

    if (pr.subsequent_move_type == 0) {
      pr.subsequent_move_type = pr.move_type;
    }
    int K = pr.move_type >= pr.subsequent_move_type ? pr.move_type
                                                    : pr.subsequent_move_type;
    if (pr.nonsequential_move_type == -1 || pr.nonsequential_move_type > K)
      pr.nonsequential_move_type = K;
  }

  static void Init(const Param& pr, Context& ctx, const Problem& problem) {
    ctx.dimension = problem.dimension;
    ctx.node_set = CreateNodes(problem.dimension);
    ctx.FirstNode = &ctx.node_set.front();

    ctx.BetterTour.resize(problem.dimension + 1);
    ctx.hash_table.init_rand(problem.dimension + 1);

    ctx.CostMatrix = problem.costs();

    SRandom(pr.seed);
    ctx.Optimum = pr.known_optimum;
    MoveFunction BestOptMove[] = {
        nullptr, nullptr, Best2OptMove, Best3OptMove, Best4OptMove, Best5OptMove};
    ctx.BestMove = BestOptMove[pr.move_type];
    ctx.BestSubsequentMove = BestOptMove[pr.subsequent_move_type];
    int K = pr.move_type;
    if (pr.subsequent_move_type > K) K = pr.subsequent_move_type;
    AllocateSegments(pr.tree_type, problem.dimension, ctx);
  }

  // The AllocateSegments function allocates the segments of the two-level tree.
  static void AllocateSegments(short tree_type, int dimension, Context& ctx) {
    ctx.segments.clear();
    ctx.super_segments.clear();

    if (tree_type == 3)
      ctx.SegmentSize = static_cast<int>(pow(dimension, 1.0 / 3.0));
    else if (tree_type == 2)
      ctx.SegmentSize = static_cast<int>(sqrt(dimension));
    else
      ctx.SegmentSize = dimension;

    int SegmentCount = 0;
    for (int i = dimension; i > 0; i -= ctx.SegmentSize) {
      TreeNode s{};
      s.Rank = ++SegmentCount;
      ctx.segments.emplace_back(s);
    }

    for (int i = 0; i < ctx.segments.size() - 1; i++)
      SLink(ctx.segments[i], ctx.segments[i + 1]);
    SLink(ctx.segments.back(), ctx.segments.front());

    if (tree_type == 3)
      ctx.SuperSegmentSize = static_cast<int>(sqrt(SegmentCount));
    else
      ctx.SuperSegmentSize = dimension;

    int SuperSegmentCount = 0;
    for (int i = SegmentCount; i > 0; i -= ctx.SuperSegmentSize) {
      TreeNode ss{};
      ss.Rank = ++SuperSegmentCount;
      ctx.super_segments.emplace_back(ss);
    }

    for (int i = 0; i < ctx.super_segments.size() - 1; i++)
      SLink(ctx.super_segments[i], ctx.super_segments[i + 1]);
    SLink(ctx.super_segments.back(), ctx.super_segments.front());
  }
};