#include "data/Context.h"
#include "data/TreeNode.h"

/*
   The Between_SSL function is used to determine whether a segment is
   between two other segments with respect to the current orientation.
   The function is only used if the three-level tree representation
   is used for a tour.

   Between_SSL(a,b,c) returns 1 if segment b is between segment a and c.
   Otherwise, 0 is returned.

   The function is called from the functions BestMove, Gain23,
   BridgeGain, Make4OptMove and Make5OptMove.
*/

bool Between_SSL(const Node *ta, const Node *tb, const Node *tc) {
  if (tb == ta || tb == tc) return true;
  if (ta == tc) return false;
  const TreeNode *Pa = ta->Parent;
  const TreeNode *Pb = tb->Parent;
  const TreeNode *Pc = tc->Parent;
  const TreeNode *PPa = Pa->Parent;
  const TreeNode *PPb = Pb->Parent;
  const TreeNode *PPc = Pc->Parent;
  if (Pa == Pc) {
    if (Pb == Pa)
      return (context.Reversed == (Pa->Reversed != PPa->Reversed)) ==
             (ta->Rank < tc->Rank ? tb->Rank > ta->Rank && tb->Rank < tc->Rank
                                  : tb->Rank > ta->Rank || tb->Rank < tc->Rank);
    return (context.Reversed == (Pa->Reversed != PPa->Reversed)) ==
           (ta->Rank > tc->Rank);
  }
  if (Pb == Pc)
    return (context.Reversed == (Pb->Reversed != PPb->Reversed)) ==
           (tb->Rank < tc->Rank);
  if (Pa == Pb)
    return (context.Reversed == (Pa->Reversed != PPa->Reversed)) ==
           (ta->Rank < tb->Rank);
  if (PPa == PPc) {
    if (PPb == PPa)
      return (context.Reversed == PPa->Reversed) ==
             (Pa->Rank < Pc->Rank ? Pb->Rank > Pa->Rank && Pb->Rank < Pc->Rank
                                  : Pb->Rank > Pa->Rank || Pb->Rank < Pc->Rank);
    return (context.Reversed == PPa->Reversed) == (Pa->Rank > Pc->Rank);
  }
  if (PPb == PPc)
    return (context.Reversed == PPb->Reversed) == (Pb->Rank < Pc->Rank);
  if (PPa == PPb)
    return (context.Reversed == PPa->Reversed) == (Pa->Rank < Pb->Rank);
  return context.Reversed !=
         (PPa->Rank < PPc->Rank
              ? PPb->Rank > PPa->Rank && PPb->Rank < PPc->Rank
              : PPb->Rank > PPa->Rank || PPb->Rank < PPc->Rank);
}
