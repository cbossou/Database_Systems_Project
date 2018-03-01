#include "BenchMark.h"

#include <random>
#include <vector>
#include <tuple>

using namespace std;

vector<int> *make_seq_data(int size) {
  vector<int> *v = new vector<int>();
  for (int i = 0; i < size; i++) {
    v->push_back(i);
  }
  return v;
}

vector<int> *make_rnd_data(int size, int mean) {
  poisson_distribution<int> distribution(mean);
  default_random_engine generator;
  vector<int> *v = new vector<int>();
  for (int i = 0; i < size; i++) {
    v->push_back(distribution(generator));
  }
  return v;
}

// sequential, high hit rate, read heavy
vector<tuple<string, int>> *seq_hit_read(int size) {
  // FIXME EXAMPLE
  vector<int> *v = make_seq_data(size);
  vector<tuple<string, int>> *run_results = new vector<tuple<string, int>>();
  // do MAXIMUM_BASE_SIZE inserts
  // do MAXIMUM_BASE_SIZE * 100 reads
  // do MAXIMUM_BASE_SIZE / 2 delete
  // free it all
  run_results->push_back(make_tuple(string("test1"), 1));
  run_results->push_back(make_tuple(string("test2"), 2));

  delete(v);
  return run_results;
}

// sequential, high hit rate, write heavy
vector<tuple<string, int>> *seq_hit_write(int size) {
  return NULL;
}

// sequential, high hit rate, update heavy
vector<tuple<string, int>> *seq_hit_update(int size) {
  return NULL;

}

// sequential, high hit rate, delete heavy
vector<tuple<string, int>> *seq_hit_delete(int size) {
  return NULL;

}

// sequential, high miss rate, read heavy
vector<tuple<string, int>> *seq_miss_read(int size) {
  return NULL;

}

// sequential, high miss rate, write heavy
vector<tuple<string, int>> *seq_miss_write(int size) {
  return NULL;

}

// sequential, high miss rate, update heavy
vector<tuple<string, int>> *seq_miss_update(int size) {
  return NULL;

}

// sequential, high miss rate, delete heavy
vector<tuple<string, int>> *seq_miss_delete(int size) {
  return NULL;

}

// random, high hit rate, read heavy
vector<tuple<string, int>> *rnd_hit_read(int size) {
  return NULL;

}

// random, high hit rate, write heavy
vector<tuple<string, int>> *rnd_hit_write(int size) {
  return NULL;

}

// random, high hit rate, update heavy
vector<tuple<string, int>> *rnd_hit_update(int size) {
  return NULL;

}

// random, high hit rate, delete heavy
vector<tuple<string, int>> *rnd_hit_delete(int size) {
  return NULL;

}

// random, high miss rate, read heavy
vector<tuple<string, int>> *rnd_miss_read(int size) {
  return NULL;

}

// random, high miss rate, write heavy
vector<tuple<string, int>> *rnd_miss_write(int size) {
  return NULL;

}

// random, high miss rate, update heavy
vector<tuple<string, int>> *rnd_miss_update(int size) {
  return NULL;

}

// random, high miss rate, delete heavy
vector<tuple<string, int>> *rnd_miss_delete(int size) {
  return NULL;

}

