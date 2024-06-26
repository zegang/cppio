// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_ROCKSDB_KVS_H_
#define CPPIO_ROCKSDB_KVS_H_

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

#include "kvs_impl.h"

using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::PinnableSlice;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

namespace CPPIO_NAMESPACE {

class RocksDBKVS : public KVSImpl {
public:
    RocksDBKVS() = default;
    RocksDBKVS(const std::string& dbpath) : KVSImpl(dbpath) {}
    virtual ~RocksDBKVS();

    Error Open();
    Error Put(const std::string& key, const std::string& value);
    Error Get(const std::string& key, CPPIO_OUT std::string& value);

    Error Close();

private:
    std::unique_ptr<DB> db_ = nullptr;
    Options options_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_ROCKSDB_KVS_H_