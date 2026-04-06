// @file LKHcpp.h
// @brief
// @author Kunlin Yu
// @date 2026-04-06

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <CLI11/CLI11.hpp>

#include "LKHcpp.h"
#include "data/Param.h"
#include "FatalExitAppender.h"
#include "FileLineFormatter.h"

int main(int argc, char* argv[]) {
  CLI::App app{"LKHcpp A C++11 implementation of the LKH algorithm"};
  app.name("LKHcpp");

  std::string param_filename;
  app.add_option("--param,--parameter-file", param_filename,
                 "Path to the parameter file")
      ->required()
      ->check(CLI::ExistingFile);

  std::string problem_filename;
  app.add_option("--problem,--problem-file", problem_filename,
                 "Path to the problem file")
      ->check(CLI::ExistingFile);

  Param param_cli{};
  app.add_option("--tour,--tour-file", param_cli.tour_filename,
                 "Path to the output tour file");

  // max trials
  app.add_option("-t, --max-trials", param_cli.max_trials,
                 "Maximum number of trials in each run")
      ->check(CLI::NonNegativeNumber);

  // runs
  app.add_option("-r ,--runs", param_cli.runs, "Total number of runs")
      ->check(CLI::NonNegativeNumber);

  // optimum
  app.add_option("-o, --optimum", param_cli.known_optimum,
                 "Known optimum tour length")
      ->check(CLI::NonNegativeNumber);

  app.add_option("--tree, --tree_type", param_cli.tree_type,
                 "level of tree representation")
      ->check(CLI::IsMember({"1", "2", "3"}));

  app.add_option("--move, --move_type", param_cli.move_type,
                 "sequential move type to be used in local search")
      ->check(CLI::IsMember({"2", "3", "4", "5"}));

  plog::Severity severity_level = plog::info;
  app.add_option(
         "-l, --trace-level", severity_level,
         "Specifies the level[0-6] of detail of the output given during the "
         "solution process. The value 0 signifies a minimum amount of "
         "output. The higher the value is the more information is given.")
      ->default_val(plog::info)
      ->check(CLI::NonNegativeNumber);
  bool verbose = false;
  app.add_flag("-v, --verbose", verbose, "print verbose log");
  bool color = false;
  app.add_flag("-c, --color", color, "print colorfully log");

  CLI11_PARSE(app, argc, argv);

  if (verbose) severity_level = plog::verbose;
  static plog::ConsoleAppender<FileLineFormatter<true, true>> appender;
  static plog::ColorConsoleAppender<FileLineFormatter<true, true>> c_appender;
  plog::IAppender& i_appender = color
                                    ? static_cast<plog::IAppender&>(c_appender)
                                    : static_cast<plog::IAppender&>(appender);
  FatalExitAppender exit_appender(i_appender);
  plog::init(severity_level, &exit_appender);

  PLOGD << "Command line arguments parsed.";

  return EXIT_SUCCESS;
}