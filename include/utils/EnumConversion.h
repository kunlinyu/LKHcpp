#pragma once
#include <string>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#define MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#define MAYBE_UNUSED __pragma(warning(suppress : 4505))
#else
#define MAYBE_UNUSED
#endif

#define DECLARE_ENUM_CONVERSION(T, ...)                                       \
  MAYBE_UNUSED static const std::vector<std::pair<T, std::string>> T##Map() { \
    return __VA_ARGS__;                                                       \
  }                                                                           \
                                                                              \
  MAYBE_UNUSED static std::string to_string(T value) {                        \
    for (const auto& entry : T##Map())                                        \
      if (entry.first == value) return entry.second;                          \
    return "UNKNOWN_" + std::to_string(static_cast<int>(value));              \
  }                                                                           \
                                                                              \
  MAYBE_UNUSED static bool from_string(const std::string& str, T* out) {      \
    for (const auto& entry : T##Map()) {                                      \
      if (entry.second == str) {                                              \
        if (out) *out = entry.first;                                          \
        return true;                                                          \
      }                                                                       \
    }                                                                         \
    return false;                                                             \
  }

// EXAMPLE:
//
// enum Color { Red, Green, Blue };

// DECLARE_ENUM_CONVERSION(Color, {{Red, "Red"},
//                                 {Red, "RED"},
//                                 {Green, "Green"},
//                                 {Green, "GREEN"},
//                                 {Blue, "Blue"},
//                                 {Blue, "BLUE"}});

// #include <iostream>
// inline void func() {
//   Color c;
//   if (from_string("RED", &c)) std::cout << to_string(c);
// }
