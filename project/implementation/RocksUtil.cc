#include "RocksUtil.h"

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
Status rocks_init() {
  opts.create_if_missing = true;
  Status s = TransactionDB::Open(opts, dbopts, DBPath, &dbptr);
  txn = NULL;
  return s;
}

// destroy the current database
Status rocks_destroy() {
  delete dbptr;
  dbptr = NULL;
  Status s = DestroyDB(DBPath, opts);
  return s;
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
Status rocks_put_buffer(string key, string value) {
  if (txn == NULL) return Status::Aborted("Couldn't Begin Transaction");
  Status s = txn->Put(key, value);
  return s;
}

// direct read from the database
Status rocks_get(string key, string *pvalue) {
  Status s = dbptr->Get(ropts, key, pvalue);
  return s;
}

// read from the current transaction before commit
Status rocks_get_buffer(string key, string *pvalue) {
  Status s = txn->Get(ropts, key, pvalue);
  return s;
}

Status rocks_delete(string key) {
  Status s = dbptr->Delete(wopts, key);
  return s;
}
