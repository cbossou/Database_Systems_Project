#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "ExtraUtil.h"

vector<tuple<string, double>> *seq_hit_read(uint64_t size, double average);
vector<tuple<string, double>> *seq_hit_write(uint64_t size, double average);
vector<tuple<string, double>> *seq_hit_update(uint64_t size, double average);
vector<tuple<string, double>> *seq_hit_delete(uint64_t size, double average);
vector<tuple<string, double>> *seq_miss_read(uint64_t size, double average);
vector<tuple<string, double>> *seq_miss_write(uint64_t size, double average);
vector<tuple<string, double>> *seq_miss_update(uint64_t size, double average);
vector<tuple<string, double>> *seq_miss_delete(uint64_t size, double average);
vector<tuple<string, double>> *rnd_hit_read(uint64_t size, double average);
vector<tuple<string, double>> *rnd_hit_write(uint64_t size, double average);
vector<tuple<string, double>> *rnd_hit_update(uint64_t size, double average);
vector<tuple<string, double>> *rnd_hit_delete(uint64_t size, double average);
vector<tuple<string, double>> *rnd_miss_read(uint64_t size, double average);
vector<tuple<string, double>> *rnd_miss_write(uint64_t size, double average);
vector<tuple<string, double>> *rnd_miss_update(uint64_t size, double average);
vector<tuple<string, double>> *rnd_miss_delete(uint64_t size, double average);

#endif
