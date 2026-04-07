#include <cstdlib>
#include <fstream>

#include "data/Context.h"
#include "data/Problem.h"
#include "plog/Log.h"

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
std::string FullName(const char* Name, GainType Cost);

struct TourFile {
  std::string name;
  std::string type;
  std::vector<std::string> comments;
  int dimension;
  std::vector<int> tour_section;
};

void WriteTourFile(std::ostream& os, const TourFile& tour_file) {
  os << "NAME : " << tour_file.name << "\n";
  for (const auto& comment : tour_file.comments) {
    os << "COMMENT : " << comment << "\n";
  }
  os << "TYPE : " << tour_file.type << "\n";
  os << "DIMENSION : " << tour_file.dimension << "\n";
  os << "TOUR_SECTION\n";
  for (const auto& node : tour_file.tour_section) {
    os << node << "\n";
  }
  os << "-1\nEOF\n";
}

TourFile TourFileSTTSP(const TourFile& tour_file) {
  TourFile result = tour_file;
  result.tour_section.clear();
  for (size_t i = 0; i < tour_file.tour_section.size(); i++) {
    size_t next_i = i + 1;
    if (next_i >= tour_file.tour_section.size()) next_i = 0;

    NodeIdType a = tour_file.tour_section[i];
    NodeIdType b = tour_file.tour_section[next_i];

    result.tour_section.push_back(context.NodeSet[a].OriginalId);
    for (int k = 0; k < context.NodeSet[a].Paths[b].size(); k++)
      result.tour_section.push_back(context.NodeSet[a].Paths[b][k]);
  }
  return result;
}

void WriteTour(const std::string& FileName, const std::vector<NodeIdType>& tour,
               GainType Cost) {
  if (FileName.empty()) return;
  std::string FullFileName = FullName(FileName.c_str(), Cost);
  PLOGI << "Writing: " << FullFileName;
  std::ofstream ofs(FullFileName, std::ios::binary);
  if (!ofs.is_open()) {
    PLOGF << "WriteTour: Cannot open " << FullFileName;
  }

  TourFile tour_file;
  tour_file.name = problem.name + "." + std::to_string(Cost) + ".tour";
  tour_file.type = "TOUR";
  tour_file.comments.push_back("Length = " + std::to_string(Cost));
  tour_file.comments.push_back("Found by LKH-3 [Keld Helsgaun]");
  tour_file.dimension = problem.dimension;

  int i;
  for (i = 1; i < problem.dimension && tour[i] != DepotIndex; i++);
  PLOGI << "i value : " << i;
  bool Forward =
      problem.IsAsymmetric() || tour[i < problem.dimension ? i + 1 : 1] <
                                    tour[i > 1 ? i - 1 : problem.dimension];
  if (Forward) {
    LOGI << "Tour direction: forward";
  } else {
    LOGI << "Tour direction: backward";
  }

  i = 1;
  for (int j = 1; j <= problem.dimension; j++) {
    int a = tour[i];
    if (a <= problem.dimension)
      tour_file.tour_section.push_back(a);
    else
      LOGE << "Warning: Node " << a
           << " is out of range and will be skipped in the tour file.";
    i += Forward ? 1 : -1;
    if (i > problem.dimension) i = 1;
    if (i < 1) i = problem.dimension;
  }
  if (problem.type == STTSP) tour_file = TourFileSTTSP(tour_file);
  WriteTourFile(ofs, tour_file);
  ofs.close();
}

/*
 * The FullName function returns a copy of the string Name where all
 * occurrences of the character '$' have been replaced by Cost.
 */

std::string FullName(const char* Name, GainType Cost) {
  std::string result(Name);
  const std::string cost_str = std::to_string(Cost);

  size_t pos = 0;
  while ((pos = result.find('$', pos)) != std::string::npos) {
    result.replace(pos, 1, cost_str);
    pos += cost_str.length();
  }
  return result;
}