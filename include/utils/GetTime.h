#pragma once

#include <ctime>

inline double GetTime() { return (double) clock() / CLOCKS_PER_SEC; }