#include "data/Context.h"
#include "data/Problem.h"

/*
 * The RecordBetterTour function is called by FindTour each time
 * the LinKernighan function has returned a better tour.
 *
 * The function records the tour in the BetterTour array and in the
 * BestSuc field of each node. Furthermore, for each node the previous
 * value of BestSuc is saved in the NextBestSuc field.
 *
 * Recording a better tour in the BetterTour array when the problem is
 * asymmetric requires special treatment since the number of nodes has
 * been doubled.
 */

void RecordBetterTour(std::vector<NodeIdType> &BetterTour, Node *FirstNode) {
  Node *N = FirstNode;

  int i = 1;
  do BetterTour[i++] = N->Id;
  while ((N = N->SucNode()) != FirstNode);

  BetterTour[0] = BetterTour[problem.dimension];
  N = FirstNode;
  do {
    N->NextBestSuc = N->BestSuc;
    N->BestSuc = N->SucNode();
  } while ((N = N->SucNode()) != FirstNode);
}