#include <cstdlib>
#include <fstream>

#include "data/Context.h"
#include "data/Problem.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"
#include "plog/Log.h"

Tour ExtractFinalTour(const std::vector<NodeIdType>& ids) {
  int i;
  assert(context.dimension == ids.size() - 1);
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
