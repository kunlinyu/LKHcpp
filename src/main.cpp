#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <CLI11/CLI11.hpp>
#include <csignal>
#include <thread>

#include "FatalExitAppender.h"
#include "FileLineFormatter.h"
#include "LKHcpp.h"
#include "data/Param.h"
#include "data/ParamReader.h"
#include "data/TSPLIB.h"
#include "data/TSPLIBReader.h"
#include "data/Tour.h"

std::atomic<bool> g_stop{false};
void HandleSigInt(int) { g_stop.store(true, std::memory_order_relaxed); }

int main(int argc, char* argv[]) {
  g_stop.store(false, std::memory_order_relaxed);
  std::signal(SIGINT, HandleSigInt);

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

  app.add_option("-j, --threads", param_cli.threads,
                 "number of threads to run in parallel")
      ->check(CLI::PositiveNumber);

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

  LOGD << "Command line arguments parsed.";

  // ******** Read parameters ********
  std::ifstream fparam(param_filename);
  if (!fparam.is_open()) {
    LOGE << "param file " << param_filename << " not found ";
    return EXIT_FAILURE;
  }
  Param pr = ParamReader::ReadStream(fparam);
  // patch by command line arguments
  pr.Patch(param_cli);
  if (not problem_filename.empty()) pr.tsplib_filename = problem_filename;

  // ******** Read TSPLIB ********
  std::ifstream fproblem(pr.tsplib_filename);
  if (!fproblem.is_open()) {
    LOGE << "TSPLIB file " << pr.tsplib_filename << " not found ";
    return EXIT_FAILURE;
  }
  const TSPLIB tsplib = TSPLIBReader::Read(fproblem);

  // ******** solve the problem ********
  LKHcpp lkh;
  TourFile tour_file;
  std::atomic<bool> solve_done{false};

  std::thread solver_thread([&]() {
    tour_file = lkh.Solve(pr, tsplib);
    solve_done.store(true, std::memory_order_release);
  });

  // ******** wait for result ********
  while (!solve_done.load(std::memory_order_acquire)) {
    if (g_stop.load(std::memory_order_relaxed)) {
      LOGW << "SIGINT received, requesting solver stop...";
      lkh.stop();
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  if (solver_thread.joinable()) solver_thread.join();

  if (tour_file.tour.empty())  {
    LOGW << "nothing found";
    return EXIT_SUCCESS;
  }

  // ******** write to tour file ********
  std::string file_path_cost = pr.TourFilenameWithCost(tour_file.tour.cost);
  LOGI << "Writing: " << file_path_cost;
  std::ofstream ofs(file_path_cost, std::ios::binary);
  if (ofs.is_open()) {
    tour_file.write(ofs);
    ofs.close();
  } else {
    LOGE << "Cannot open " << file_path_cost;
  }

  return EXIT_SUCCESS;
}
