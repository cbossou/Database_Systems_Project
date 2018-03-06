#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "ExtraUtil.h"

vector<tuple<string, int>> *seq_hit_read(int size, int average);
vector<tuple<string, int>> *rnd_hit_read(int size, int average);

#endif
