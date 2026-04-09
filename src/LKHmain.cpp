#include <plog/Log.h>

#include <CLI11/CLI11.hpp>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>

#include "Initializer.h"
#include "LKH.h"
#include "STTSP2TSP.h"
#include "candidate/CandidateFuncs.h"
#include "candidate/POpMUSICCandidateSetCreator.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "data/TSPLIBReader.h"
#include "utils/GetTime.h"
#include "variant/VariantFactory.h"

int LKHmain(Param& pr) {
  std::ifstream fproblem(pr.problem_filename.c_str());
  if (!fproblem.is_open()) {
    PLOGE << "Problem file %s not found: " << pr.problem_filename;
    return EXIT_FAILURE;
  }

  const TSPLIB tsplib = TSPLIBReader::Read(fproblem);
  Initializer::AdjustParameters(pr, tsplib.dimension);

  // set to global variables
  param = pr;
  problem.dimension = tsplib.dimension;
  problem.type = tsplib.type;

  double LastTime;
  context.StartTime = LastTime = GetTime();

  std::unique_ptr<VariantBase> variant = VariantFactory::CreateVariant(tsplib);
  PLOGI << variant->chain();
  context.problem = variant->Encode(tsplib);

  std::vector<Node> node_set;
  if (problem.type == STTSP) {
    context.problem = Problem(tsplib.required_nodes_section.size(),
                              STTSP2TSP(tsplib, node_set));
  }

  Initializer::Init(param, context, context.problem);

  if (problem.type == STTSP) {
    for (size_t i = 0; i < context.node_set.size(); i++) {
      context.node_set[i].Paths = node_set[i].Paths;
      context.node_set[i].OriginalId = node_set[i].OriginalId;
    }
  }

  POpMUSICCandicateSetCreator popmusic;
  popmusic.set_initial_tour(param.popmusic_initial_tour);
  popmusic.set_max_neighbors(param.popmusic_max_neighbors);
  popmusic.set_sample_size(param.popmusic_sample_size);
  popmusic.set_solutions(param.popmusic_solutions);
  popmusic.set_trails(param.popmusic_trials);

  double LowerBound = CreateCandidateSet(popmusic);
  context.SwitchCostToD();

  std::vector<NodeIdType> BestTour(problem.dimension + 1);

  GainType BestCost = std::numeric_limits<GainType>::max();
  if (context.Norm != 0) {
    context.Norm = std::numeric_limits<int>::max();
  } else {
    // The ascent has solved the problem!
    context.Optimum = BestCost = (GainType)LowerBound;
    RecordBetterTour(context.BetterTour, context.FirstNode);
    BestTour = context.BetterTour;
    WriteTour(param.tour_filename, BestTour, BestCost);
    param.runs = 0;
    return EXIT_SUCCESS;
  }

  GainType OrdinalTourCost = 0;
  OrdinalTourCost = CalcOrdinalTourCost();
  for (int Run = 1; Run <= param.runs; Run++) {
    LastTime = GetTime();
    GainType Cost =
        FindTour(OrdinalTourCost);  // using the Lin-Kernighan heuristic
    if (Cost < BestCost) {
      BestCost = Cost;
      RecordBetterTour(context.BetterTour, context.FirstNode);
      BestTour = context.BetterTour;
      WriteTour(param.tour_filename, BestTour, BestCost);
    }

    GainType OldOptimum = context.Optimum;
    if (Cost < context.Optimum) context.Optimum = Cost;
    if (context.Optimum < OldOptimum) {
      PLOGI << "*** New OPTIMUM = " << context.Optimum << " ***";
    }

    PLOGI_IF(Cost != std::numeric_limits<GainType>::max())
        << "Run " << Run << ": " << StatusReport(Cost, LastTime, "");

    if (param.stop_at_optimum and Cost == context.Optimum) {
      param.runs = Run;
      break;
    }
  }

  return EXIT_SUCCESS;
}