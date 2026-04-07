#include <algorithm>
#include <cmath>

#include "data/Coordinate.h"

int Distance_ATT(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y;
  return static_cast<int>(ceil(sqrt((xd * xd + yd * yd) / 10.0)));
}

int Distance_CEIL_2D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y;
  return (int)ceil(sqrt(xd * xd + yd * yd));
}

int Distance_CEIL_3D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y, zd = a->Z - b->Z;
  return (int)ceil(sqrt(xd * xd + yd * yd + zd * zd));
}

int Distance_EUC_2D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y;
  return lround(sqrt(xd * xd + yd * yd));
}

int Distance_EUC_3D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y, zd = a->Z - b->Z;
  return lround(sqrt(xd * xd + yd * yd + zd * zd));
}

int Distance_FLOOR_2D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y;
  return (int)floor(sqrt(xd * xd + yd * yd));
}

int Distance_FLOOR_3D(const Coordinate* a, const Coordinate* b) {
  double xd = a->X - b->X, yd = a->Y - b->Y, zd = a->Z - b->Z;
  return (int)floor(sqrt(xd * xd + yd * yd + zd * zd));
}

#define PI 3.141592
#define RRR 6378.388

int Distance_GEO(const Coordinate* a, const Coordinate* b) {
  int deg = (int)a->X;
  double min = a->X - deg;
  double aLatitude = PI * (deg + 5.0 * min / 3.0) / 180.0;
  deg = (int)a->Y;
  min = a->Y - deg;
  double aLongitude = PI * (deg + 5.0 * min / 3.0) / 180.0;
  deg = (int)b->X;
  min = b->X - deg;
  double bLatitude = PI * (deg + 5.0 * min / 3.0) / 180.0;
  deg = (int)b->Y;
  min = b->Y - deg;
  double bLongitude = PI * (deg + 5.0 * min / 3.0) / 180.0;
  double q1 = cos(aLongitude - bLongitude);
  double q2 = cos(aLatitude - bLatitude);
  double q3 = cos(aLatitude + bLatitude);
  return (int)(RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
}

#undef M_PI
#define M_PI 3.14159265358979323846264
#define M_RRR 6378388.0

int Distance_GEOM(const Coordinate* a, const Coordinate* b) {
  double lati = M_PI * (a->X / 180.0);
  double latj = M_PI * (b->X / 180.0);
  double longi = M_PI * (a->Y / 180.0);
  double longj = M_PI * (b->Y / 180.0);
  double q1 = cos(latj) * sin(longi - longj);
  double q3 = sin((longi - longj) / 2.0);
  double q4 = cos((longi - longj) / 2.0);
  double q2 = sin(lati + latj) * q3 * q3 - sin(lati - latj) * q4 * q4;
  double q5 = cos(lati - latj) * q4 * q4 - cos(lati + latj) * q3 * q3;
  return (int)(M_RRR * atan2(sqrt(q1 * q1 + q2 * q2), q5) + 1.0);
}

int Distance_MAN_2D(const Coordinate* a, const Coordinate* b) {
  return lround(fabs(a->X - b->X) + fabs(a->Y - b->Y));
}

int Distance_MAN_3D(const Coordinate* a, const Coordinate* b) {
  return lround(fabs(a->X - b->X) + fabs(a->Y - b->Y) + fabs(a->Z - b->Z));
}

int Distance_MAX_2D(const Coordinate* a, const Coordinate* b) {
  int dx = lround(fabs(a->X - b->X));
  int dy = lround(fabs(a->Y - b->Y));
  return dx > dy ? dx : dy;
}

int Distance_MAX_3D(const Coordinate* a, const Coordinate* b) {
  int dx = lround(fabs(a->X - b->X));
  int dy = lround(fabs(a->Y - b->Y));
  int dz = lround(fabs(a->Z - b->Z));
  if (dy > dx) dx = dy;
  return dx > dz ? dx : dz;
}

/* Function for computing the distance in kilometers between two points on
   the Earth's surface, based on the high accuracy method by H. Andoyer,
   as described in

       "Astronomical Algorithms (2nd Ed.)", pg. 85, Jean Meeus (2000).
*/

static double Meeus(double lat1, double lon1, double lat2, double lon2) {
  constexpr double a = 6378.137;      // equator earth radius
  constexpr double fl = 1 / 298.257;  // earth flattening
  if (lat1 == lat2 && lon1 == lon2) return 0;
  double f = (lat1 + lat2) / 2;
  double g = (lat1 - lat2) / 2;
  double l = (lon1 - lon2) / 2;
  double sg = sin(g) * sin(g);
  double sl = sin(l) * sin(l);
  double sf = sin(f) * sin(f);
  double s = sg * (1 - sl) + (1 - sf) * sl;
  double c = (1 - sg) * (1 - sl) + sf * sl;
  double w = atan(sqrt(s / c));
  double r = sqrt(s * c) / w;
  double d = 2 * w * a;
  double h1 = (3 * r - 1) / 2 / c;
  double h2 = (3 * r + 1) / 2 / s;
  return d * (1 + fl * (h1 * sf * (1 - sg) - h2 * (1 - sf) * sg));
}

int Distance_GEO_MEEUS(const Coordinate* a, const Coordinate* b) {
  double lat1 = M_PI * ((int)a->X + 5 * (a->X - (int)a->X) / 3) / 180;
  double lon1 = M_PI * ((int)a->Y + 5 * (a->Y - (int)a->Y) / 3) / 180;
  double lat2 = M_PI * ((int)b->X + 5 * (b->X - (int)b->X) / 3) / 180;
  double lon2 = M_PI * ((int)b->Y + 5 * (b->Y - (int)b->Y) / 3) / 180;
  return lround(Meeus(lat1, lon1, lat2, lon2));
}

int Distance_GEOM_MEEUS(const Coordinate* a, const Coordinate* b) {
  double lat1 = M_PI * (a->X / 180);
  double lon1 = M_PI * (a->Y / 180);
  double lat2 = M_PI * (b->X / 180);
  double lon2 = M_PI * (b->Y / 180);
  return lround(1000 * Meeus(lat1, lon1, lat2, lon2));
}

int Distance_XRAY1(const Coordinate* a, const Coordinate* b) {
  double distp = std::min(fabs(a->X - b->X), fabs(fabs(a->X - b->X) - 360));
  double distc = fabs(a->Y - b->Y);
  double distt = fabs(a->Z - b->Z);
  double cost = std::max(distp, std::max(distc, distt));
  return lround(100 * cost);
}

int Distance_XRAY2(const Coordinate* a, const Coordinate* b) {
  double distp = std::min(fabs(a->X - b->X), fabs(fabs(a->X - b->X) - 360));
  double distc = fabs(a->Y - b->Y);
  double distt = fabs(a->Z - b->Z);
  double cost = std::max(distp / 1.25, std::max(distc / 1.5, distt / 1.15));
  return lround(100 * cost);
}
