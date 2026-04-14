// @file ParamReader.cpp
// @brief
// @author Kunlin Yu
// @date 2026/4/10

#include "data/ParamReader.h"

Param ParamReader::Read(const std::function<nlohmann::json()>& parse) {
  try {
    nlohmann::json j = parse();
    Param p = j;
    LOGI << "Json parameter read:\n" << j.dump(2);
    return p;
  } catch (const nlohmann::json::parse_error& e) {
    LOGE << "parse parameter error: " << e.what();
    throw std::invalid_argument("Error parsing parameter file: " +
                                std::string(e.what()));
  } catch (const nlohmann::json::type_error& e) {
    LOGE << "parse parameter error: " << e.what();
    throw std::invalid_argument("Error in parameter types: " +
                                std::string(e.what()));
  }
}
