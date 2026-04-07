#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Problem.h"

class ProblemReader {
  typedef std::function<bool(const std::string& value, TSPLIB& problem)>
      FieldFunc;
  typedef std::function<bool(const std::string& line, TSPLIB& problem)>
      SectionFunc;

 public:
  static TSPLIB Read(std::istream& is);

 private:
  static std::string trim(const std::string& str);

  static std::vector<std::string> split(const std::string& str,
                                        const std::string& delimiters);

  static const std::map<std::string, FieldFunc>& FieldFuncs();

  static const std::map<std::string, SectionFunc>& SectionFuncs();

  static void Check(TSPLIB& pb);
};
