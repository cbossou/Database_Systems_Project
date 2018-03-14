#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "ExtraUtil.h"

vector<tuple<string, double>> *seq_hit_read(int size, double average);
vector<tuple<string, double>> *seq_hit_write(int size, double average);
vector<tuple<string, double>> *seq_hit_update(int size, double average);
vector<tuple<string, double>> *seq_hit_delete(int size, double average);
vector<tuple<string, double>> *seq_miss_read(int size, double average);
vector<tuple<string, double>> *seq_miss_write(int size, double average);
vector<tuple<string, double>> *seq_miss_update(int size, double average);
vector<tuple<string, double>> *seq_miss_delete(int size, double average);
vector<tuple<string, double>> *rnd_hit_read(int size, double average);
vector<tuple<string, double>> *rnd_hit_write(int size, double average);
vector<tuple<string, double>> *rnd_hit_update(int size, double average);
vector<tuple<string, double>> *rnd_hit_delete(int size, double average);
vector<tuple<string, double>> *rnd_miss_read(int size, double average);
vector<tuple<string, double>> *rnd_miss_write(int size, double average);
vector<tuple<string, double>> *rnd_miss_update(int size, double average);
vector<tuple<string, double>> *rnd_miss_delete(int size, double average);

#endif
