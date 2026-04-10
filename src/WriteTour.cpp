#include <cstdlib>
#include <fstream>

#include "data/Context.h"
#include "data/Problem.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"
#include "plog/Log.h"
/*
 * The ReplaceCost function returns a copy of the string Name where all
 * occurrences of the character '$' have been replaced by Cost.
 */

std::string ReplaceCost(const char* Name, GainType Cost) {
  std::string result(Name);
  const std::string cost_str = std::to_string(Cost);

  size_t pos = 0;
  while ((pos = result.find('$', pos)) != std::string::npos) {
    result.replace(pos, 1, cost_str);
    pos += cost_str.length();
  }
  return result;
}
/*
 * The WriteTour function writes a tour to file. The tour
 * is written in TSPLIB format to file FileName.
 *
 * The tour is written in "normal form": starting at node 1,
 * and continuing in direction of its lowest numbered
 * neighbor.
 *
 * Nothing happens if FileName is 0.
 */

void WriteTourFile(std::ostream& os, const TourFile& tour_file) {
  os << "NAME : " << tour_file.name << "\n";
  for (const auto& comment : tour_file.comments) {
    os << "COMMENT : " << comment << "\n";
  }
  os << "TYPE : " << tour_file.type << "\n";
  os << "DIMENSION : " << tour_file.dimension << "\n";
  os << "TOUR_SECTION\n";
  for (const auto& node : tour_file.node_ids) {
    os << node << "\n";
  }
  os << "-1\nEOF\n";
}

Tour TourFileSTTSP(const Tour& tour) {
  Tour result;
  for (size_t i = 0; i < tour.node_ids.size(); i++) {
    size_t next_i = i + 1;
    if (next_i >= tour.node_ids.size()) next_i = 0;

    NodeIdType a = tour.node_ids[i];
    NodeIdType b = tour.node_ids[next_i];

    // TODO: convert id to index
    result.node_ids.push_back(context.node_set[a - 1].OriginalId);
    for (const int k : context.node_set[a - 1].Paths[b])
      result.node_ids.push_back(k);
  }
  return result;
}

Tour ExtractFinalTour(const std::vector<NodeIdType>& ids) {
  int i;
  for (i = 1; i < context.dimension && ids[i] != DepotIndex; i++);
  PLOGI << "i value : " << i;
  bool Forward = ids[i < context.dimension ? i + 1 : 1] <
                 ids[i > 1 ? i - 1 : context.dimension];
  if (Forward) {
    LOGI << "Tour direction: forward";
  } else {
    LOGI << "Tour direction: backward";
  }

  Tour tour;
  i = 1;
  for (int j = 1; j <= context.dimension; j++) {
    NodeIdType a = ids[i];
    if (a <= context.dimension)
      tour.node_ids.push_back(a);
    else
      LOGE << "Warning: Node " << a
           << " is out of range and will be skipped in the tour file.";
    i += Forward ? 1 : -1;
    if (i > context.dimension) i = 1;
    if (i < 1) i = context.dimension;
  }
  return tour;
}

TourFile CreateTourFile(const Tour& tour, const std::string& name,
                        GainType cost) {
  TourFile tour_file;
  tour_file.name = name;
  tour_file.type = "TOUR";
  tour_file.comments.emplace_back("Length = " + std::to_string(cost));
  tour_file.comments.emplace_back("Found by LKH-3 [Keld Helsgaun]");
  tour_file.dimension = context.dimension;
  tour_file.node_ids = tour.node_ids;
  return tour_file;
}

inline std::string FileName(const std::string& file_path) {
  size_t found = file_path.find_last_of("/\\");
  if (found == std::string::npos) return file_path;
  return file_path.substr(found + 1);
}

void WriteTour(const std::string& file_path, const Tour& tour,
               GainType Cost) {
  if (file_path.empty()) return;
  std::string file_path_cost = ReplaceCost(file_path.c_str(), Cost);
  PLOGI << "Writing: " << file_path_cost;

  std::string name = FileName(file_path_cost);
  TourFile tour_file = CreateTourFile(tour, name, Cost);

  std::ofstream ofs(file_path_cost, std::ios::binary);
  if (!ofs.is_open()) {
    PLOGF << "WriteTour: Cannot open " << file_path_cost;
  }
  WriteTourFile(ofs, tour_file);
  ofs.close();
}
