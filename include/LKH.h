#pragma once

#include <atomic>

#include "candidate/POpMUSICCandidateSetCreator.h"
#include "type.h"

GainType Ascent(POpMUSICCandicateSetCreator& popmusic);

GainType CalcOrdinalTourCost();

GainType FindTour(GainType, const std::atomic<bool>& stop);

GainType LinKernighan(const std::atomic<bool>& stop);

void ChooseInitialTour(Node* FirstNode);

GainType Gain23();

int Improvement(GainType* Gain, Node* t1, Node* SUCt1);

std::string StatusReport(GainType Cost, double EntryTime, const char* Suffix);
