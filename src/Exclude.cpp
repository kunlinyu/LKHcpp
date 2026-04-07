#include "data/Node.h"

/*
 * The Exclude function is used to register that an edge, (ta,tb),
 * of the original tour has been excluded in a move. This is done by
 * setting the appropriate flag, OldPredExcluded or OldSucExcluded,
 * for each of the two end nodes.
 */

void Exclude(Node* ta, Node* tb) {
  if (ta == tb->Prd || ta == tb->Suc) return;
  if (ta == tb->OldPrd)
    tb->OldPredExcluded = true;
  else if (ta == tb->OldSuc)
    tb->OldSucExcluded = true;
  if (tb == ta->OldPrd)
    ta->OldPredExcluded = true;
  else if (tb == ta->OldSuc)
    ta->OldSucExcluded = true;
}
