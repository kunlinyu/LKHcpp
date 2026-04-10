#include <plog/Log.h>

#include <CLI11/CLI11.hpp>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>

#include "Initializer.h"
#include "LKH.h"
#include "candidate/CandidateFuncs.h"
#include "candidate/POpMUSICCandidateSetCreator.h"
#include "data/Context.h"
#include "data/Param.h"
#include "data/Problem.h"
#include "data/TSPLIBReader.h"
#include "utils/GetTime.h"
#include "variant/VariantFactory.h"

Tour Solve(Param& pr, const Problem& problem) {
  // set to global variables
  param = pr;

  double LastTime;
  context.StartTime = LastTime = GetTime();

  PLOGI << "Initialize context by parameters and problem";
  Initializer::Init(param, context, problem);

  POpMUSICCandicateSetCreator popmusic;
  popmusic.set_initial_tour(param.popmusic_initial_tour);
  popmusic.set_max_neighbors(param.popmusic_max_neighbors);
  popmusic.set_sample_size(param.popmusic_sample_size);
  popmusic.set_solutions(param.popmusic_solutions);
  popmusic.set_trails(param.popmusic_trials);

  double LowerBound = CreateCandidateSet(popmusic);
  context.SwitchCostToD();

  std::vector<NodeIdType> BestTour(context.dimension + 1);

  GainType BestCost = std::numeric_limits<GainType>::max();
  if (context.Norm != 0) {
    context.Norm = std::numeric_limits<int>::max();
  } else {
    // The ascent has solved the problem!
    context.Optimum = BestCost = (GainType)LowerBound;
    RecordBetterTour(context.BetterTour, context.FirstNode);
    BestTour = context.BetterTour;
    Tour tour = ExtractFinalTour(BestTour);
    tour.cost = BestCost;
    return tour;
  }

  GainType OrdinalTourCost = 0;
  OrdinalTourCost = CalcOrdinalTourCost();
  Tour best_tour;
  for (int Run = 1; Run <= param.runs; Run++) {
    LastTime = GetTime();
    GainType Cost =
        FindTour(OrdinalTourCost);  // using the Lin-Kernighan heuristic
    if (Cost < BestCost) {
      BestCost = Cost;
      RecordBetterTour(context.BetterTour, context.FirstNode);
      BestTour = context.BetterTour;
      Tour tour = ExtractFinalTour(BestTour);
      tour.cost = BestCost;
      best_tour = tour;
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

  return best_tour;
}

void LKHmain(Param& pr, const TSPLIB& tsplib) {
  pr.adjust(tsplib.dimension);

  std::unique_ptr<VariantBase> variant = VariantFactory::Create(tsplib);
  PLOGI << "Encode problem with variant: " << variant->chain();
  Problem problem = variant->Encode(tsplib);

  Tour tour = Solve(pr, problem);

  tour = variant->Decode(tour);

  WriteTour(param.tour_filename, tour, tour.cost);
}