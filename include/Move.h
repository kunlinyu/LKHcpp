#pragma once
#include "type.h"

class Node;

Node *Best2OptMove(Node * t1, Node * t2, GainType * G0, GainType * Gain);
Node *Best3OptMove(Node * t1, Node * t2, GainType * G0, GainType * Gain);
Node *Best4OptMove(Node * t1, Node * t2, GainType * G0, GainType * Gain);
Node *Best5OptMove(Node * t1, Node * t2, GainType * G0, GainType * Gain);

void Make2OptMove(Node * t1, Node * t2, Node * t3, Node * t4);
void Make3OptMove(Node * t1, Node * t2, Node * t3, Node * t4,
                  Node * t5, Node * t6, int Case);
void Make4OptMove(Node * t1, Node * t2, Node * t3, Node * t4,
                  Node * t5, Node * t6, Node * t7, Node * t8,
                  int Case);
void Make5OptMove(Node * t1, Node * t2, Node * t3, Node * t4,
                  Node * t5, Node * t6, Node * t7, Node * t8,
                  Node * t9, Node * t10, int Case);

int SpecialMove(Node * t1, Node * t2, GainType * G0, GainType * Gain);