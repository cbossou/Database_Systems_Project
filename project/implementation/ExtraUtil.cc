#include "ExtraUtil.h"

using namespace std;

// prints error to stderr and then exits program
// prints using red color
void eexit(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_RED);
  vfprintf(stderr, fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
  exit(-1);
}

// prints using magenta color
void warn_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_MAGENTA);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
}

// prints using blue color
void normal_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf(ANSI_COLOR_CYAN);
  vprintf(fmt, args);
  printf(ANSI_COLOR_RESET);
  va_end(args);
}

// returns the trimmed version of this string based off delimiters
string trimmed(string orig, string delimiters) {
  int begin = 0;
  int end = orig.length() - 1;
  while (begin < end) { // trim beginning
    if (delimiters.find(orig.at(begin)) == string::npos) {
      break;
    }
    begin++;
  }
  while (end > begin) { // trim at end
    if (delimiters.find(orig.at(end)) == string::npos) {
      break;
    }
    end++;
  }
  int length = end - begin + 1;
  return orig.substr(begin, length);
}

string DBPath = "/tmp/RocksBench";

// The following code was taken/modified from
// rocksdb/examples/transaction_example.cc

Options opts;
TransactionDB* dbptr;
TransactionDBOptions dbopts;
WriteOptions wopts;
ReadOptions ropts;
Transaction *txn;
TransactionOptions txnops;

// create an instance of the database
void rocks_init() {
  opts.create_if_missing = true;
  Status s = TransactionDB::Open(opts, dbopts, DBPath, &dbptr);
  if (!s.ok()) {
    eexit("Database creation failed\n");
  }
  txn = NULL;
}

// destroy the current database
void rocks_destroy() {
  delete dbptr;
  dbptr = NULL;
  Status s = DestroyDB(DBPath, opts);
  if (!s.ok()) {
    eexit("Can't destroy database\n");
  }
}

// begin a new transaction
// This is required before making calls to
// rocks_put_buffer and rocks_get_buffer
Status rocks_commit_begin() {
  // Start a transaction
  txn = dbptr->BeginTransaction(wopts);
  return txn == NULL ? Status::Aborted("Couldn't Begin Transaction") :
    Status::OK();
}

// commit the current transaction and destroy it
Status rocks_commit_finish() {
  // Commit transaction
  Status s = txn->Commit();
  delete txn;
  txn = NULL;
  return s;
}

// direct put to the database
Status rocks_put(string key, string value) {
  Status s = dbptr->Put(wopts, key, value);
  if (!s.ok()) {
    warn_printf("Unable to put (%s,%s) on the database\n", key, value);
  }
  return s;
}

// put on to the current transaction before commit
Status rocks_put_buffer() {
  if (txn == NULL) return Status::Aborted("Couldn't Begin Transaction");
  Status s = txn->Put("abc", "def");
  //assert(s.ok());
  return s;
}

// direct read from the database
Status rocks_get(string key, string *pvalue) {
  Status s = dbptr->Get(ropts, key, pvalue);
  //assert(s.IsNotFound());
  return s;
}

// read from the current transaction before commit
Status rocks_get_buffer(string key, string *pvalue) {
  string value;
  Status s = txn->Get(ropts, key, pvalue);
  //assert(s.IsNotFound());
  return s;
}
