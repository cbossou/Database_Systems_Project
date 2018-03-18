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
#define BASE_READ_HEAVY_DELETES 5

#define BASE_WRITE_HEAVY_WRITES 1000
#define BASE_WRITE_HEAVY_READS 10
#define BASE_WRITE_HEAVY_DELETES 5

#define BASE_DELETE_HEAVY_WRITES 100
#define BASE_DELETE_HEAVY_READS 10
#define BASE_DELETE_HEAVY_DELETES 50

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
// write:read:delete :: 10:1000:5
vector<tuple<string, double>> *seq_hit_read(uint64_t size, double average) {

  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bd = BASE_READ_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (i = 0; i < size; i++) { // reset i because we want everything to hit
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  int16_t bw = BASE_WRITE_HEAVY_WRITES;
  int16_t br = BASE_WRITE_HEAVY_READS;
  int16_t bd = BASE_WRITE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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

// sequential, high hit rate, delete heavy
vector<tuple<string, double>> *seq_hit_delete(uint64_t size, double average) {
  int16_t bw = BASE_DELETE_HEAVY_WRITES;
  int16_t br = BASE_DELETE_HEAVY_READS;
  int16_t bd = BASE_DELETE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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

// sequential, high miss rate, read heavy
vector<tuple<string, double>> *seq_miss_read(uint64_t size, double average) {
  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bd = BASE_READ_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  // have different data for read than write if high miss rate
  vector<double> *v_r = make_seq_data(size * br, 100 * (average - size/2) * br);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

// sequential, high miss rate, write heavy
vector<tuple<string, double>> *seq_miss_write(uint64_t size, double average) {
  int16_t bw = BASE_WRITE_HEAVY_WRITES;
  int16_t br = BASE_WRITE_HEAVY_READS;
  int16_t bd = BASE_WRITE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  // have different data for read than write if high miss rate
  vector<double> *v_r = make_seq_data(size * br, 100 * (average - size/2) * br);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

// sequential, high miss rate, delete heavy
vector<tuple<string, double>> *seq_miss_delete(uint64_t size, double average) {
  int16_t bw = BASE_DELETE_HEAVY_WRITES;
  int16_t br = BASE_DELETE_HEAVY_READS;
  int16_t bd = BASE_DELETE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_seq_data(size * bw, (average - size/2) * bw);
  // have different data for read than write if high miss rate
  vector<double> *v_r = make_seq_data(size * br, 100 * (average - size/2) * br);
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

// random, high hit rate, read heavy
// write:read:delete :: 10:1000:5
vector<tuple<string, double>> *rnd_hit_read(uint64_t size, double average) {
  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bd = BASE_READ_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (i = 0; i < size; i++) { // reset i because we want everything to hit
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  int16_t bw = BASE_WRITE_HEAVY_WRITES;
  int16_t br = BASE_WRITE_HEAVY_READS;
  int16_t bd = BASE_WRITE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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

// random, high hit rate, delete heavy
vector<tuple<string, double>> *rnd_hit_delete(uint64_t size, double average) {
  int16_t bw = BASE_DELETE_HEAVY_WRITES;
  int16_t br = BASE_DELETE_HEAVY_READS;
  int16_t bd = BASE_DELETE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
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

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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

// random, high miss rate, read heavy
vector<tuple<string, double>> *rnd_miss_read(uint64_t size, double average) {
  int16_t bw = BASE_READ_HEAVY_WRITES;
  int16_t br = BASE_READ_HEAVY_READS;
  int16_t bd = BASE_READ_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  // have mostly different data for read than write if high miss rate
  vector<double> *v_r = make_rnd_data(size * br, 100 * (average * br));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

// random, high miss rate, write heavy
vector<tuple<string, double>> *rnd_miss_write(uint64_t size, double average) {
  int16_t bw = BASE_WRITE_HEAVY_WRITES;
  int16_t br = BASE_WRITE_HEAVY_READS;
  int16_t bd = BASE_WRITE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  // have mostly different data for read than write if high miss rate
  vector<double> *v_r = make_rnd_data(size * br, 100 * (average * br));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

// random, high miss rate, delete heavy
vector<tuple<string, double>> *rnd_miss_delete(uint64_t size, double average) {
  int16_t bw = BASE_DELETE_HEAVY_WRITES;
  int16_t br = BASE_DELETE_HEAVY_READS;
  int16_t bd = BASE_DELETE_HEAVY_DELETES;
  uint64_t i = 0;

  vector<double> *v = make_rnd_data(size * bw, (average * bw));
  // have mostly different data for read than write if high miss rate
  vector<double> *v_r = make_rnd_data(size * br, 100 * (average * br));
  vector<tuple<string, double>> *run_results =
    new vector<tuple<string, double>>();

  // perform writes
  time_acc = 0;
  i = 0;
  for (uint16_t write_count = 0; write_count < bw; write_count++) {
    for(; i < size; i++) {
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
  i = 0;
  for (uint16_t read_count = 0; read_count < br; read_count++) {
    for (; i < size; i++) {
      double data = v_r->at(i);
      string comp;
      time_func(rocks_get, to_string(data), &comp);
    }
  }
  run_results->push_back(make_tuple("get_time [ms]",
        us_2_ms(average_time(time_acc, br))));

  // perform deletes
  time_acc = 0;
  i = 0;
  for (uint16_t delete_count = 0; delete_count < bd; delete_count++) {
    for (; i < size; i++) {
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
  delete(v_r);
  return run_results;
}

