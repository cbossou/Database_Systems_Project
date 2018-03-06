#include "ExtraUtil.h"
#include "BenchMark.h"

#include <string>
#include <vector>
#include <tuple>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

#define RESULT_PATH ("results")
#define RESULT_PATH_STR (string(RESULT_PATH))
#define TEST_PATH_STR(test) (RESULT_PATH_STR + string("/") + test + string("/"))
#define TEST_PATH(test) ((TEST_PATH_STR(test)).c_str())

#define TEST_STR(test) (#test)
#define TEST_ADDR(test) (&test)

#define RESULT_NAME 0
#define RESULT_VALUE 1

using namespace std;

int benchmark_wrapper(int size, int average, string bench_name,
    vector<tuple<string, int>> *(*bench_func)(int size, int average)) {
  // create a folder for these tests in results
  boost::filesystem::path dir(TEST_PATH(bench_name));
  boost::filesystem::create_directory(dir);
  FILE *res_file = fopen((TEST_PATH_STR(bench_name) + bench_name +
        to_string(size) + string(".csv")).c_str(), "w");
  for (int i = 0; i < NUMTRIALS; i++) {
    vector<tuple<string, int>> *run_results = bench_func(size, average);
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
      fprintf(res_file, "%d%s", get<RESULT_VALUE>(run_results->at(iter)),
            (iter == run_results->size() - 1 ? ",\n" : ","));
      normal_printf("%d%s", get<RESULT_VALUE>(run_results->at(iter)),
            (iter == run_results->size() - 1 ? ",\n" : ","));
    }
    normal_printf("\n========\n");
    delete(run_results);
  }
  fclose(res_file);
  return 1;
}

int main(int argc, char **argv) {
  // create the results directory
  boost::filesystem::path dir(RESULT_PATH);
  boost::filesystem::create_directory(dir);

  // run the benchmarks
  for (int i = 1; i <= MAXIMUM_BASE_SIZE; i++) {
    benchmark_wrapper(i, i/2, TEST_STR(seq_hit_read), TEST_ADDR(seq_hit_read));
    benchmark_wrapper(i, i/2, TEST_STR(rnd_hit_read), TEST_ADDR(rnd_hit_read));
  }

  return 0;
}
