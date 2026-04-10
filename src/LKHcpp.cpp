// @file LKHcpp.cpp
// @brief
// @author Kunlin Yu
// @date 2026/4/7

#include "LKHcpp.h"

#include "Initializer.h"
#include "LKH.h"
#include "data/Context.h"
#include "data/ParamReader.h"
#include "data/TSPLIBReader.h"
#include "utils/GetTime.h"
#include "variant/VariantFactory.h"

Tour LKHcpp::Solve(const Param& pr, const Problem& pb) {
  // set to global variables
  param = pr;

  double LastTime;
  context.StartTime = LastTime = GetTime();

  PLOGI << "Initialize context by parameters and problem";
  Initializer::Init(param, context, pb);

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

TourFile LKHcpp::Solve(const Param& pr, const TSPLIB& tsplib) {
  Param param = pr;
  param.Adjust(tsplib.dimension);

  std::unique_ptr<VariantBase> variant = VariantFactory::Create(tsplib);
  PLOGI << "Encode problem with variant: " << variant->chain();
  Problem problem = variant->Encode(tsplib);

  Tour tour = Solve(param, problem);

  tour = variant->Decode(tour);

  TourFile tour_file;
  tour_file.name = tsplib.name;
  tour_file.type = "TOUR";
  tour_file.comments.emplace_back("Length = " + std::to_string(tour.cost));
  tour_file.comments.emplace_back("Found by LKH-3 [Keld Helsgaun]");
  tour_file.dimension = context.dimension;
  tour_file.tour = tour;

  return tour_file;
}

void LKHcpp::Solve(std::istream& param_json, std::istream& tsplib_text,
                   std::ostream& tour_text) {
  Param pr = ParamReader::ReadStream(param_json);
  TSPLIB tsplib = TSPLIBReader::Read(tsplib_text);
  TourFile tour_file = Solve(pr, tsplib);
  tour_file.write(tour_text);
}