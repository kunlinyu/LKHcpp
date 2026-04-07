#include <random>
thread_local static std::mt19937 random_engine;
void SRandom(const unsigned Seed) { random_engine.seed(Seed); }
unsigned Random() { return random_engine(); }