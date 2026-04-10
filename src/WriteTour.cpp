#include <plog/Log.h>

#include <fstream>

#include "data/Context.h"
#include "data/TSPLIB.h"
#include "data/Tour.h"

Tour ExtractFinalTour(const std::vector<NodeIdType>& ids) {
  size_t dimension = ids.size() - 1;
  int i;
  for (i = 1; i < dimension && ids[i] != DepotIndex; i++);
  PLOGI << "i value : " << i;
  bool Forward =
      ids[i < dimension ? i + 1 : 1] < ids[i > 1 ? i - 1 : dimension];
  if (Forward) {
    LOGI << "Tour direction: forward";
  } else {
    LOGI << "Tour direction: backward";
  }

  Tour tour;
  i = 1;
  for (int j = 1; j <= dimension; j++) {
    NodeIdType a = ids[i];
    if (a <= dimension)
      tour.node_ids.push_back(a);
    else
      LOGE << "Warning: Node " << a
           << " is out of range and will be skipped in the tour file.";
    i += Forward ? 1 : -1;
    if (i > dimension) i = 1;
    if (i < 1) i = dimension;
  }
  return tour;
}
