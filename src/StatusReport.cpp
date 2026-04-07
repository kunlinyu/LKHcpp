#include <cmath>
#include <iomanip>
#include <sstream>

#include "data/Context.h"
#include "type.h"
#include "utils/GetTime.h"

std::string StatusReport(GainType Cost, double EntryTime, const char *Suffix) {
  std::stringstream ss;
  ss << "Cost = " << Cost;
  if (context.Optimum != std::numeric_limits<GainType>::min() &&
      context.Optimum != 0)
    ss << ", Gap = " << std::fixed << std::setprecision(4)
       << 100.0 * (Cost - context.Optimum) / context.Optimum << "%";
  ss << ", Time = " << std::fixed << std::setprecision(2)
     << fabs(GetTime() - EntryTime) << " sec." << Suffix
     << (Cost < context.Optimum    ? " <"
         : Cost == context.Optimum ? " ="
                                   : "");
  return ss.str();
}
