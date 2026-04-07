#include "data/Node.h"

int C_EXPLICIT(const Node* Na, const Node* Nb) {
  return Na->Id < Nb->Id ? Nb->C[Na->Id] : Na->C[Nb->Id];
}

int D_EXPLICIT(const Node* Na, const Node* Nb) {
  return C_EXPLICIT(Na, Nb) + Na->Pi + Nb->Pi;
}
