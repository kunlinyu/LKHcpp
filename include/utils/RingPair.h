// @file RingPair.h
// @brief
// @author Kunlin Yu
// @date 2026/4/8

#pragma once
#include <cstddef>
#include <functional>
#include <vector>

template <typename ElementType>
void RingPair(std::vector<ElementType> &data,
              const std::function<void(ElementType &, ElementType &)> &func) {
  size_t n = data.size();
  if (n < 2) return;
  for (size_t i = 0; i < n; ++i) func(data[i], data[(i + 1) % n]);
}