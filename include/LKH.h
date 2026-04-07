#pragma once

#include "candidate/POpMUSICCandidateSetCreator.h"
#include "data/Problem.h"
#include "type.h"

GainType Ascent(POpMUSICCandicateSetCreator& popmusic);
GainType CalcOrdinalTourCost(const std::vector<Node>& NodeSet);
GainType FindTour(GainType);

void ChooseInitialTour(Node* FirstNode);
GainType LinKernighan();
void RecordBetterTour(std::vector<NodeIdType>& BetterTour, Node* FirstNode);
GainType Gain23();
int Improvement(GainType* Gain, Node* t1, Node* SUCt1);

void RestoreTour();
void WriteTour(const std::string& FileName, const std::vector<NodeIdType>& tour,
               GainType Cost);

bool Excludable(Node* ta, Node* tb);
void Exclude(Node* ta, Node* tb);

std::string StatusReport(GainType Cost, double EntryTime, const char* Suffix);

void NormalizeNodeList();
void NormalizeSegmentList();

int SegmentSize(Node* ta, Node* tb);
