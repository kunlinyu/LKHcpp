#pragma once

#include "data/Node.h"
#include "type.h"

class POpMUSICThreeOpt {
 private:
  int n = 0;
  int **dist = nullptr;
  int *tour = nullptr;
  int *pos = nullptr;
  int **neighbor = nullptr;
  int *neighbors = nullptr;
  int reversed = 0;
  char *dontLook = nullptr;
  GainType tourLength = 0;

  Node **&node_path;

  int &trials;
  int &max_neighbors;

 public:
  POpMUSICThreeOpt(Node **&node_path, int &trials, int &max_neighbors);

  void path_threeOpt(int N, int **D, int *best_sol, GainType *best_cost);

 private:
  int fixed(int a, int b);
  int prev(int v);
  int next(int v);
  int between(int v1, int v2, int v3);
  int PREV(int v);
  int NEXT(int v);
  int BETWEEN(int v1, int v2, int v3);
  void flip(int from, int to);
  void threeOpt();
  void createNeighbors();
  int legal(int r, int i, int t[4]);
  int select_t(int i, int t[4]);
  void doubleBridgeKick();
};

/*
 * The Create_POPMUSIC_CandidateSet function creates for each node
 * a candidate set of K edges using the POPMUSIC algorithm.
 *
 * The function is called from the CreateCandidateSet function.
 *
 * Programmed by Keld Helsgaun and Eric Taillard, 2018.
 *
 * References:
 *
 *     É. D. Taillard and K. Helsgaun,
 *     POPMUSIC for the Travelling Salesman Problem.
 *     European Journal of Operational Research, 272(2):420-429 (2019).
 *
 *     K. Helsgaun,
 *     Using POPMUSIC for Candidate Set Generation in the
 *     Lin-Kernighan-Helsgaun TSP Solver.
 *     Technical Report, Computer Science, Roskilde University, 2018.
 */
class POpMUSICCandicateSetCreator {
 private:
  Node **node = nullptr;
  Node **node_path = nullptr;
  POpMUSICThreeOpt three_opt;

  bool initial_tour_ = false;  // Specifies whether the first POPMUSIC tour is
                               // used as initial tour for LK
  int max_neighbors_ = 5;  // Maximum number of nearest neighbors used as
                           // candidates in iterated 3-opt
  int sample_size_ = 10;   // The sample size
  int solutions_ = 50;     // Number of solutions to generate
  int trials_ = 1;         // Maximum trials used for iterated 3-opt
 public:
  POpMUSICCandicateSetCreator();

  bool initial_tour() const { return initial_tour_; }
  int max_neighbors() const { return max_neighbors_; }
  int sample_size() const { return sample_size_; }
  int solutions() const { return solutions_; }
  int trials() const { return trials_; }

  void set_initial_tour(int initial_tour) { initial_tour_ = initial_tour; }
  void set_sample_size(int sample_size) { sample_size_ = sample_size; }
  void set_solutions(int solutions) { solutions_ = solutions; }
  void set_trails(int trials) { trials_ = trials; }
  void set_max_neighbors(int max_neighbors) { max_neighbors_ = max_neighbors; }

  void CreateCandidateSet(int K);

 private:
  GainType length_path(int n, int *path);
  void optimize_path(int n, int *path);
  void build_path(int n, int *path, int nb_clust);
  void reverse(int *path, int i, int j);
  void circular_right_shift(int n, int *path, int positions);
  void fast_POPMUSIC(int n, int *path, int R);
};

// extern POpMUSICCandicateSetCreator popmusic;