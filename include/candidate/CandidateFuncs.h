#pragma once
#include "POpMUSICCandidateSetCreator.h"

int AddCandidate(Node* From, Node* To, WeightType Cost, WeightType Alpha);
void AddExtraDepotCandidates();
void AdjustCandidateSet(Node* FirstNode);
void GenerateCandidates(size_t MaxCandidates, GainType MaxAlpha, int Symmetric);
std::string CandidateReport(Node* FirstNode);
int NotEqual(Node * From, Node * To);
void OrderCandidateSet(size_t MaxCandidates,
                       GainType MaxAlpha, int Symmetric);
void ResetCandidateSet();
void SymmetrizeCandidateSet();
void TrimCandidateSet(int MaxCandidates);
double CreateCandidateSet(POpMUSICCandicateSetCreator &popmusic);