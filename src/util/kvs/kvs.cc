// Copyright 2025 cppio authors. All rights reserved.

#include "kvs_impl.h"
#include "kvs.h"
#include "rocksdb_kvs.h"

namespace CPPIO_NAMESPACE {

KVS::KVS(KVSDBType dbtype, const std::string& dbpath) {
    pKVSImp_ = KVSImpl::GetKVSImpl(dbtype, dbpath);
}

KVS::~KVS() {}

const std::string& KVS::dbpath() const { return pKVSImp_->dbpath(); }

Error KVS::Open() { return pKVSImp_->Open(); }

Error KVS::Put(const std::string& key, const std::string& value) {
    return pKVSImp_->Put(key, value);
}

Error KVS::Get(const std::string& key, CPPIO_OUT std::string& value) {
    return pKVSImp_->Get(key, value);
}

Error KVS::Close() { return pKVSImp_->Close(); }

std::unique_ptr<KVSImpl> KVSImpl::GetKVSImpl(KVSDBType dbtype,
                                             const std::string& dbpath) {
    switch (dbtype)
    {
    case KVSDBType::kKVSDBTYPE_ROCKSDB:
        return std::make_unique<RocksDBKVS>(dbpath);
    default:
        return std::make_unique<RocksDBKVS>(dbpath);
    }
}

} // namespace CPPIO_NAMESPACE