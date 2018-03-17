#include "BenchMark.h"
#include "RocksUtil.h"

#include <sys/time.h>
#include <random>
#include <vector>
#include <tuple>

#define us_2_ms(val_us) (val_us / 1e3)

#define time_func(func, ...) { \
  gettimeofday(&start_time, NULL); \
  s = func(__VA_ARGS__); \
  gettimeofday(&end_time, NULL); \
  time_acc += (end_time.tv_sec * 1e6 + end_time.tv_usec) - \
  (start_time.tv_sec * 1e6 + start_time.tv_usec); \
}

#define average_time(time_acc, num_ops) ((time_acc) / (num_ops))

#define BASE_READ_HEAVY_WRITES 10
#define BASE_READ_HEAVY_READS 1000
#define BASE_READ_HEAVY_UPDATES 5
#define BASE_READ_HEAVY_DELETES 5

using namespace std;

static uint64_t time_acc;
static struct timeval start_time, end_time;
static Status s;

vector<double> *make_seq_data(uint64_t size, double start) {
  vector<double> *v = new vector<double>();
  for (double i = start; i < size + start; i++) {
    v->push_back(i);
  }
  return v;
}

vector<double> *make_rnd_data(uint64_t size, double average) {
  poisson_distribution<int> distribution(average);
  default_random_engine generator;
  vector<double> *v = new vector<double>();
  for (uint64_t i = 0; i < size; i++) {
    v->push_back(distribution(generator));
  }
  return v;
}

// sequential, high hit rate, read heavy
// write:read:update:delete :: 10:1000:5:5
vector<tuple<string, double>> *seq_hit_read(uint64_t size, double average) {

  vector<double> *v = make_seq_data(size, average - size/2);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bu = BASE_READ_HEAVY_UPDATES;
  int16_t bd = BASE_READ_HEAVY_DELETES;

  // perform writes
  time_acc = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_put, to_string(data), to_string(data));
      if (!s.ok()) {
        eexit("%s failed put for (%s,%s)\n",
            __FUNCTION__, to_string(data), to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("put_time [ms]",
        us_2_ms(average_time(time_acc, bw))));

  // perform reads
  time_acc = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
      if (!s.ok() || comp.compare(to_string(data)) != 0) {
        eexit("%s failed get: expected=%s : actual=%s\n",
            __FUNCTION__, to_string(data).c_str(), comp.c_str());
      }
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform updates
  time_acc = 0;
  for (uint16_t update_count = 0; update_count < bu; update_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_put, to_string(data), to_string(data));
      if (!s.ok()) {
        eexit("%s failed update for (%s,%d)\n",
            __FUNCTION__, to_string(data), to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("update_time [ms]",
        us_2_ms(average_time(time_acc, bu))));

  // perform deletes
  time_acc = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_delete, to_string(data));
      if (!s.ok()) {
        eexit("%s failed delete on key %s\n",
            __FUNCTION__, to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("delete_time [ms]",
        us_2_ms(average_time(time_acc, bd))));

  delete(v);
  return run_results;
}

// sequential, high hit rate, write heavy
vector<tuple<string, double>> *seq_hit_write(uint64_t size, double average) {
  return NULL;
}

// sequential, high hit rate, update heavy
vector<tuple<string, double>> *seq_hit_update(uint64_t size, double average) {
  return NULL;

}

// sequential, high hit rate, delete heavy
vector<tuple<string, double>> *seq_hit_delete(uint64_t size, double average) {
  return NULL;

}

// sequential, high miss rate, read heavy
vector<tuple<string, double>> *seq_miss_read(uint64_t size, double average) {
  return NULL;

}

// sequential, high miss rate, write heavy
vector<tuple<string, double>> *seq_miss_write(uint64_t size, double average) {
  return NULL;

}

// sequential, high miss rate, update heavy
vector<tuple<string, double>> *seq_miss_update(uint64_t size, double average) {
  return NULL;

}

// sequential, high miss rate, delete heavy
vector<tuple<string, double>> *seq_miss_delete(uint64_t size, double average) {
  return NULL;

}

// random, high hit rate, read heavy
// write:read:update:delete :: 10:1000:5:5
vector<tuple<string, double>> *rnd_hit_read(uint64_t size, double average) {
  vector<double> *v = make_rnd_data(size, average);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bu = BASE_READ_HEAVY_UPDATES;
  int16_t bd = BASE_READ_HEAVY_DELETES;

  // perform writes
  time_acc = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_put, to_string(data), to_string(data));
      if (!s.ok()) {
        eexit("%s failed put for (%s,%s)\n",
            __FUNCTION__, to_string(data), to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("put_time [ms]",
        us_2_ms(average_time(time_acc, bw))));

  // perform reads
  time_acc = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
      if (!s.ok() || comp.compare(to_string(data)) != 0) {
        eexit("%s failed get: expected=%s : actual=%s\n",
            __FUNCTION__, to_string(data).c_str(), comp.c_str());
      }
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform updates
  time_acc = 0;
  for (uint16_t update_count = 0; update_count < bu; update_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_put, to_string(data), to_string(data));
      if (!s.ok()) {
        eexit("%s failed update for (%s,%d)\n",
            __FUNCTION__, to_string(data), to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("update_time [ms]",
        us_2_ms(average_time(time_acc, bu))));

  // perform deletes
  time_acc = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (uint64_t i = 0; i < size; i++) {
      double data = v->at(i);
      time_func(rocks_delete, to_string(data));
      if (!s.ok()) {
        eexit("%s failed delete on key %s\n",
            __FUNCTION__, to_string(data));
      }
    }
  }
  run_results->push_back(make_tuple("delete_time [ms]",
        us_2_ms(average_time(time_acc, bd))));

  delete(v);
  return run_results;
}

// random, high hit rate, write heavy
vector<tuple<string, double>> *rnd_hit_write(uint64_t size, double average) {
  return NULL;

}

// random, high hit rate, update heavy
vector<tuple<string, double>> *rnd_hit_update(uint64_t size, double average) {
  return NULL;

}

// random, high hit rate, delete heavy
vector<tuple<string, double>> *rnd_hit_delete(uint64_t size, double average) {
  return NULL;

}

// random, high miss rate, read heavy
vector<tuple<string, double>> *rnd_miss_read(uint64_t size, double average) {
  return NULL;

}

// random, high miss rate, write heavy
vector<tuple<string, double>> *rnd_miss_write(uint64_t size, double average) {
  return NULL;

}

// random, high miss rate, update heavy
vector<tuple<string, double>> *rnd_miss_update(uint64_t size, double average) {
  return NULL;

}

// random, high miss rate, delete heavy
vector<tuple<string, double>> *rnd_miss_delete(uint64_t size, double average) {
  return NULL;

}

