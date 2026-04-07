#pragma once

#include "data/Context.h"
#include "data/Param.h"

/*
 * This header specifies the interface for accessing and manipulating a
 * tour.
 *
 * If tree_type set to 3 the three-level tree representation is used
 * for a tour. If tree_type set to 2 the two-level tree representation is used.
 * Otherwise, the linked list representation is used.
 *
 * All representations support the following primitive operations:
 *
 *     (1) find the predecessor of a node in the tour with respect
 *         to a chosen orientation (PRED);
 *
 *     (2) find the successor of a node in the tour with respect to
 *         a chosen orientation (SUC);
 *
 *     (3) determine whether a given node is between two other nodes
 *         in the tour with respect to a chosen orientation (BETWEEN);
 *
 *     (4) make a 2-opt move (FLIP).
 *
 * The default representation is the two-level tree representation.
 */

#define RG context.Reversed
#define RGP(n) (RG ^ (n)->Parent->Reversed)
#define RGPP(n) (RGP(n) ^ (n)->Parent->Parent->Reversed)

// clang-format off
static Node* PRD(const Node* n) {
  switch (param.tree_type) {
    case 1: return RG ? n->SucNode() : n->PrdNode();
    default:
    case 2: return RGP(n) ? n->SucNode() : n->PrdNode();
    case 3: return RGPP(n) ? n->SucNode() : n->PrdNode();
  }
}
static Node* SUC(const Node* n) {
  switch (param.tree_type) {
    case 1: return RG ? n->PrdNode() : n->SucNode();
    default:
    case 2: return RGP(n) ? n->PrdNode() : n->SucNode();
    case 3: return RGPP(n) ? n->PrdNode() : n->SucNode();
  }
}
#undef RGPP
#undef RGP
#undef RG

bool Between(const Node* ta, const Node* tb, const Node* tc);
bool Between_SL(const Node* ta, const Node* tb, const Node* tc);
bool Between_SSL(const Node* ta, const Node* tb, const Node* tc);

static bool BETWEEN(const Node* a, const Node* b, const Node* c) {
  switch (param.tree_type) {
    case 1: return Between(a, b, c);
    default:
    case 2: return Between_SL(a, b, c);
    case 3: return Between_SSL(a, b, c);
  }
}

void Flip(Node* t1, Node* t2, Node* t3);
void Flip_SL(Node* t1, Node* t2, Node* t3);
void Flip_SSL(Node* t1, Node* t2, Node* t3);

static void FLIP(Node* a, Node* b, Node* c, Node*) {
  switch (param.tree_type) {
    case 1: return Flip(a, b, c);
    default:
    case 2: return Flip_SL(a, b, c);
    case 3: return Flip_SSL(a, b, c);
  }
}
// clang-format on

inline void Swap1(Node* a1, Node* a2, Node* a3) { FLIP(a1, a2, a3, nullptr); }
inline void Swap2(Node* a1, Node* a2, Node* a3, Node* b1, Node* b2, Node* b3) {
  Swap1(a1, a2, a3);
  Swap1(b1, b2, b3);
}
inline void Swap3(Node* a1, Node* a2, Node* a3, Node* b1, Node* b2, Node* b3,
                  Node* c1, Node* c2, Node* c3) {
  Swap2(a1, a2, a3, b1, b2, b3);
  Swap1(c1, c2, c3);
}
inline void Swap4(Node* a1, Node* a2, Node* a3, Node* b1, Node* b2, Node* b3,
                  Node* c1, Node* c2, Node* c3, Node* d1, Node* d2, Node* d3) {
  Swap3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
  Swap1(d1, d2, d3);
}
inline void Swap5(Node* a1, Node* a2, Node* a3, Node* b1, Node* b2, Node* b3,
                  Node* c1, Node* c2, Node* c3, Node* d1, Node* d2, Node* d3,
                  Node* e1, Node* e2, Node* e3) {
  Swap4(a1, a2, a3, b1, b2, b3, c1, c2, c3, d1, d2, d3);
  Swap1(e1, e2, e3);
}
