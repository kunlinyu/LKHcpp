// @file LKH.cpp
// @brief
// @author Kunlin Yu
// @date 2026/4/13

#include "LKH.h"

#include "FileLineFormatter.h"
#include "LKHcpp.h"
#include "plog/Appenders/ConsoleAppender.h"
#include "plog/Init.h"

extern "C" {

LKH_Handle LKH_Create() {
  try {
    return new LKHcpp();
  } catch (...) {
    LOGE << "Create LKH library failed.";
    return nullptr;
  }
}
void LKH_Enable_LOG() {
  static plog::ConsoleAppender<FileLineFormatter<true, true>> appender;
  plog::init(plog::info, &appender);
}

void LKH_Destroy(LKH_Handle hdl) {
  if (hdl) delete static_cast<LKHcpp*>(hdl);
}

const char* LKH_Solve(LKH_Handle hdl, const char* param_json,
                      const char* tsplib_text, size_t* out_len) {
  if (hdl) {
    try {
      auto* obj = static_cast<LKHcpp*>(hdl);
      return obj->CSolve(param_json, tsplib_text, out_len);
    } catch (const std::exception& e) {
      LOGE << "Solve failed: " << e.what();
    } catch (...) {
      LOGE << "Solve failed with unknown error.";
    }
  }
  return nullptr;
}

void LKH_Stop(LKH_Handle h) {
  if (h) static_cast<LKHcpp*>(h)->stop();
}

void LKH_FreeResult(const char* data) {
  if (data) free(const_cast<char*>(data));
}

}  // extern "C"