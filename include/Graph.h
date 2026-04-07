#pragma once
#include <map>

#include "data/Node.h"
#include "type.h"

GainType Minimum1TreeCost(bool Sparse, std::map<Node *, int> & degree_2);
void MinimumSpanningTree(bool Sparse);
void Connect(Node *N1, int Max, bool Sparse);