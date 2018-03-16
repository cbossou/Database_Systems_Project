#include "ExtraUtil.h"
#include "RocksUtil.h"
#include "BenchMark.h"

#include <string>
#include <vector>
#include <tuple>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

#define RESULT_PATH (results_name + "_" + "results").c_str()
#define RESULT_PATH_STR (string(RESULT_PATH))
#define TEST_PATH_STR(test) (RESULT_PATH_STR + string("/") + test + string("/"))
#define TEST_PATH(test) ((TEST_PATH_STR(test)).c_str())

#define TEST_STR(test) (#test)
#define TEST_ADDR(test) (&test)

#define RESULT_NAME 0
#define RESULT_VALUE 1

#ifndef DEFAULT_BEGIN_SIZE
#define DEFAULT_BEGIN_SIZE 1000
#endif

#ifndef MAX_BASE_SIZE
#define MAX_BASE_SIZE 1000000
#endif

#ifndef MAX_NUM_TRIALS
#define MAX_NUM_TRIALS 100
#endif

#ifndef DEFAULT_RESULTS_NAME
#define DEFAULT_RESULTS_NAME "default"
#endif

using namespace std;

int begin_size, max_size, num_trials;
string results_name;

int benchmark_wrapper(int size, double average, string bench_name,
    vector<tuple<string, double>> *(*bench_func)(int size, double average)) {
  // create a folder for these tests in results
  boost::filesystem::path dir(TEST_PATH(bench_name));
  boost::filesystem::create_directory(dir);
  FILE *res_file = fopen((TEST_PATH_STR(bench_name) + bench_name +
        to_string(size) + string(".csv")).c_str(), "w");
  normal_printf("Running %d trials for %s(size=%d,average=%.3lf):\n",
      num_trials, bench_name.c_str(), size, average);
  for (int i = 0; i < num_trials; i++) {
    Status s = rocks_init();
    if (!s.ok()) {
      eexit("Database creation failed\n");
    }
    vector<tuple<string, double>> *run_results = bench_func(size, average);
    if(i == 0) { // insert header line of csv
      for (uint32_t iter = 0; iter < run_results->size(); iter++) {
        fprintf(res_file, "%s%s",
            get<RESULT_NAME>(run_results->at(iter)).c_str(),
            (iter == run_results->size() - 1 ? ",\n" : ","));
        normal_printf("%s%s",
            get<RESULT_NAME>(run_results->at(iter)).c_str(),
            (iter == run_results->size() - 1 ? ",\n" : ","));
      }
    }
    for (uint32_t iter = 0; iter < run_results->size(); iter++) {
      fprintf(res_file, "%.3lf%s", get<RESULT_VALUE>(run_results->at(iter)),
            (iter == run_results->size() - 1 ? ",\n" : ","));
    }
    delete(run_results);
    s = rocks_destroy();
    if (!s.ok()) {
      eexit("Can't destroy database\n");
    }
  }
  normal_printf("\n========\n");
  fclose(res_file);
  return 1;
}

int main(int argc, char **argv) {

  begin_size = DEFAULT_BEGIN_SIZE;
  max_size = MAX_BASE_SIZE;
  num_trials = MAX_NUM_TRIALS;
  results_name = DEFAULT_RESULTS_NAME;

  int c;
  while((c = getopt(argc, argv, "n:b:s:t:")) != -1) {
    if (c == 'b' || c == 's' || c == 't') {
      if (!isdigit(optarg[0])) eexit("Entered non-int for int argument\n");
    }
    switch(c) {
      case 'b':
        begin_size = atoi(optarg);
        break;
      case 's':
        max_size = atoi(optarg) < max_size ? atoi(optarg) : max_size;
        break;
      case 't':
        num_trials = atoi(optarg) < num_trials ? atoi(optarg) : num_trials;
        break;
      case 'n':
        results_name = string(optarg);
        if (results_name.compare("") == 0) {
          eexit("Must have prefix for results folder name\n");
        }
        break;
    }
  }

  // create the results directory
  boost::filesystem::path dir(RESULT_PATH);
  boost::filesystem::create_directory(dir);

  rocks_destroy(); // just in case it survived from a midway cutoff

  // run the benchmarks
  for (int i = begin_size; i <= max_size; i *= 10) {
    benchmark_wrapper(i, i / (double) 2,
        TEST_STR(seq_hit_read), TEST_ADDR(seq_hit_read));

    benchmark_wrapper(i, i / (double) 2,
        TEST_STR(rnd_hit_read), TEST_ADDR(rnd_hit_read));
  }
  return 0;
}
