#pragma once
#include <cstdlib>

#include <plog/Appenders/ConsoleAppender.h>

class FatalExitAppender : public plog::IAppender {
private:
  IAppender& appender_;
public:
  explicit FatalExitAppender(IAppender& appender) : appender_(appender) {}
  void write(const plog::Record& record) override {
    appender_.write(record);
    if (record.getSeverity() == plog::fatal)
      std::exit(1);
  }
};