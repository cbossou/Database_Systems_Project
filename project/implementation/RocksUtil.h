#ifndef ROCKSUTIL_H
#define ROCKSUTIL_H

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/utilities/transaction.h>
#include <rocksdb/utilities/transaction_db.h>

#include "ExtraUtil.h"

using namespace rocksdb;

Status rocks_init();

Status rocks_destroy();

Status rocks_commit_begin();

Status rocks_commit_finish();

Status rocks_put(string key, string value);

Status rocks_put_buffer(string key, string value);

Status rocks_get(string key, string *pvalue);

Status rocks_get_buffer(string key, string *pvalue);

Status rocks_delete(string key);

#endif
