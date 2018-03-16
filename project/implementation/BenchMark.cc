#include "BenchMark.h"
#include "RocksUtil.h"

#include <sys/time.h>
#include <random>
#include <vector>
#include <tuple>

#define time_func(func, ...) { \
    gettimeofday(&start_time, NULL); \
    s = func(__VA_ARGS__); \
    gettimeofday(&end_time, NULL); \
    time_acc += (end_time.tv_sec * 1e6 + end_time.tv_usec) - \
        (start_time.tv_sec * 1e6 + start_time.tv_usec); \
}

using namespace std;

static uint64_t time_acc;
static struct timeval start_time, end_time;
static Status s;

vector<double> *make_seq_data(int size, int start) {
  vector<double> *v = new vector<double>();
  for (double i = start; i < size + start; i++) {
    v->push_back(i);
  }
  return v;
}

vector<double> *make_rnd_data(int size, double average) {
  poisson_distribution<int> distribution(average);
  default_random_engine generator;
  vector<double> *v = new vector<double>();
  for (int i = 0; i < size; i++) {
    v->push_back(distribution(generator));
  }
  return v;
}

// sequential, high hit rate, read heavy
vector<tuple<string, double>> *seq_hit_read(int size, double average) {

  vector<double> *v = make_seq_data(size, average - size/2);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  time_acc = 0;
  // these are strict write because the DB is emptied before this funcion call
  for(int i = 0; i < (int) v->size(); i++) {
    double data = v->at(i);
    time_func(rocks_put, to_string(data), to_string(data));
    if (!s.ok()) {
      eexit("%s failed put for (%s,%s)\n",
          __FUNCTION__, to_string(data), to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("put_time [ms]"), time_acc / 1e3));

  time_acc = 0;
  // need 100 reads for every 1 writes
  for (int read_count = 0; read_count < 100; read_count++) {
    for (int i = 0; i < (int) v->size(); i++) {
      double data = v->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
      if (!s.ok() || comp.compare(to_string(data)) != 0) {
        eexit("%s failed get: expected=%s : actual=%s\n",
            __FUNCTION__, to_string(data).c_str(), comp.c_str());
      }
    }
  }
  run_results->push_back(make_tuple(
        string("get_time [ms]"), time_acc / 1e3 / 100));

  // need 1/2 delete for every 1 write
  time_acc = 0;
  for (int i = 0; i < size / 2; i++) {
    double data = v->at(i);
    time_func(rocks_delete, to_string(data));
    if (!s.ok()) {
      eexit("%s failed delete on key %s\n",
          __FUNCTION__, to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("delete_time [ms]"), 2 * time_acc / 1e3));

  // need 1/2 updates for every 1 write
  // these are updates because we inserted these entries beforehand
  time_acc = 0;
  for (int i = 0; i < size / 2; i++) {
    double data = v->at(i);
    time_func(rocks_put, to_string(data), to_string(data));
    if (!s.ok()) {
      eexit("%s failed update for (%s,%d)\n",
          __FUNCTION__, to_string(data), to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("update_time [ms]"), 2 * time_acc / 1e3));

  delete(v);
  return run_results;
}

// sequential, high hit rate, write heavy
vector<tuple<string, double>> *seq_hit_write(int size, double average) {
  return NULL;
}

// sequential, high hit rate, update heavy
vector<tuple<string, double>> *seq_hit_update(int size, double average) {
  return NULL;

}

// sequential, high hit rate, delete heavy
vector<tuple<string, double>> *seq_hit_delete(int size, double average) {
  return NULL;

}

// sequential, high miss rate, read heavy
vector<tuple<string, double>> *seq_miss_read(int size, double average) {
  return NULL;

}

// sequential, high miss rate, write heavy
vector<tuple<string, double>> *seq_miss_write(int size, double average) {
  return NULL;

}

// sequential, high miss rate, update heavy
vector<tuple<string, double>> *seq_miss_update(int size, double average) {
  return NULL;

}

// sequential, high miss rate, delete heavy
vector<tuple<string, double>> *seq_miss_delete(int size, double average) {
  return NULL;

}

// random, high hit rate, read heavy
vector<tuple<string, double>> *rnd_hit_read(int size, double average) {
  vector<double> *v = make_rnd_data(size, average);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  time_acc = 0;
  // these are strict write because the DB is emptied before this funcion call
  for(int i = 0; i < (int) v->size(); i++) {
    double data = v->at(i);
    time_func(rocks_put, to_string(data), to_string(data));
    if (!s.ok()) {
      eexit("%s failed put for (%s,%s)\n",
          __FUNCTION__, to_string(data), to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("put_time [ms]"), time_acc / 1e3));

  time_acc = 0;
  // need 100 reads for every 1 writes
  for (int read_count = 0; read_count < 100; read_count++) {
    for (int i = 0; i < (int) v->size(); i++) {
      double data = v->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
      if (!s.ok() || comp.compare(to_string(data)) != 0) {
        eexit("%s failed get: expected=%s : actual=%s\n",
            __FUNCTION__, to_string(data).c_str(), comp.c_str());
      }
    }
  }
  run_results->push_back(make_tuple(
        string("get_time [ms]"), time_acc / 1e3 / 100));

  // need 1/2 delete for every 1 write
  time_acc = 0;
  for (int i = 0; i < size / 2; i++) {
    double data = v->at(i);
    time_func(rocks_delete, to_string(data));
    if (!s.ok()) {
      eexit("%s failed delete on key %s\n",
          __FUNCTION__, to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("delete_time [ms]"), 2 * time_acc / 1e3));

  // need 1/2 update for every 1 write
  // these are updates because we inserted these entries beforehand
  time_acc = 0;
  for (int i = 0; i < size / 2; i++) {
    double data = v->at(i);
    time_func(rocks_put, to_string(data), to_string(data));
    if (!s.ok()) {
      eexit("%s failed update for (%s,%d)\n",
          __FUNCTION__, to_string(data), to_string(data));
    }
  }
  run_results->push_back(make_tuple(
        string("update_time [ms]"), 2 * time_acc / 1e3));

  delete(v);
  return run_results;

}

// random, high hit rate, write heavy
vector<tuple<string, double>> *rnd_hit_write(int size, double average) {
  return NULL;

}

// random, high hit rate, update heavy
vector<tuple<string, double>> *rnd_hit_update(int size, double average) {
  return NULL;

}

// random, high hit rate, delete heavy
vector<tuple<string, double>> *rnd_hit_delete(int size, double average) {
  return NULL;

}

// random, high miss rate, read heavy
vector<tuple<string, double>> *rnd_miss_read(int size, double average) {
  return NULL;

}

// random, high miss rate, write heavy
vector<tuple<string, double>> *rnd_miss_write(int size, double average) {
  return NULL;

}

// random, high miss rate, update heavy
vector<tuple<string, double>> *rnd_miss_update(int size, double average) {
  return NULL;

}

// random, high miss rate, delete heavy
vector<tuple<string, double>> *rnd_miss_delete(int size, double average) {
  return NULL;

}

