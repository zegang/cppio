// Copyright 2025 cppio authors. All rights reserved.

#include "rocksdb_kvs.h"
#include "../../debug/log.h"

namespace CPPIO_NAMESPACE {

RocksDBKVS::~RocksDBKVS() {
    if (db_) {
        db_->Close();
        db_.reset();
    }
}

Error RocksDBKVS::Open() {
    Error err = ErrorOK;
    if (db_) {
        CPPIOLOG::warn("kVS {} already opened", dbpath_);
        return err;
    }

    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    options_.IncreaseParallelism();
    options_.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present
    options_.create_if_missing = true;

    DB* _db;
    Status s = DB::Open(options_, dbpath_, &_db);
    CPPIO_ASSERT(s.ok());
    db_.reset(_db);

    return err;
}

Error RocksDBKVS::Put(const std::string& key, const std::string& value) {
    Status s = db_->Put(WriteOptions(), key, value);
    CPPIO_ASSERT(s.ok());
    return ErrorOK;
}

Error RocksDBKVS::Get(const std::string& key, CPPIO_OUT std::string& value) {
    Status s = db_->Get(ReadOptions(), key, &value);
    CPPIO_ASSERT(s.ok());
    return ErrorOK;
}

Error RocksDBKVS::Close() {
    Status s = db_->Close();
    CPPIO_ASSERT(s.ok());
    return ErrorOK;
}

} // namespace CPPIO_NAMESPACE