#pragma once

struct Coordinate;

int Distance_ATT(const Coordinate* a, const Coordinate* b);

int Distance_CEIL_2D(const Coordinate* a, const Coordinate* b);
int Distance_CEIL_3D(const Coordinate* a, const Coordinate* b);

int Distance_EUC_2D(const Coordinate* a, const Coordinate* b);
int Distance_EUC_3D(const Coordinate* a, const Coordinate* b);

int Distance_FLOOR_2D(const Coordinate* a, const Coordinate* b);
int Distance_FLOOR_3D(const Coordinate* a, const Coordinate* b);

int Distance_MAN_2D(const Coordinate* a, const Coordinate* b);
int Distance_MAN_3D(const Coordinate* a, const Coordinate* b);

int Distance_MAX_2D(const Coordinate* a, const Coordinate* b);
int Distance_MAX_3D(const Coordinate* a, const Coordinate* b);

int Distance_GEO(const Coordinate* a, const Coordinate* b);
int Distance_GEOM(const Coordinate* a, const Coordinate* b);

int Distance_GEO_MEEUS(const Coordinate* a, const Coordinate* b);
int Distance_GEOM_MEEUS(const Coordinate* a, const Coordinate* b);

int Distance_XRAY1(const Coordinate* a, const Coordinate* b);
int Distance_XRAY2(const Coordinate* a, const Coordinate* b);