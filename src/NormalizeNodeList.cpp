#include "Tree.h"
#include "data/Context.h"

/*
 * The NormalizeNodeList function is used to swap the Suc and Pred fields
 * of nodes in such a way that the list of nodes constitutes a cyclic
 * two-way list.
 *
 * A call of the function corrupts the segment list representation.
 */

void NormalizeNodeList() {
  Node *node = context.FirstNode;
  do {
    Node *t = SUC(node);
    node->Prd = PRD(node);
    node->Suc = t;
    node->Parent = nullptr;
  } while ((node = node->SucNode()) != context.FirstNode);
}
