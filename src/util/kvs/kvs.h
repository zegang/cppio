// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_KVS_H_
#define CPPIO_KVS_H_

#include <string>
#include <memory>

#include "../../include/error.h"

namespace CPPIO_NAMESPACE {

class KVSImpl;

enum class KVSDBType : int {
    kKVSDBTYPE_LEVELDB,
    kKVSDBTYPE_ROCKSDB,
};

class KVS {
public:
    KVS(KVSDBType dbtype, const std::string& dbpath);
    ~KVS();
    const std::string& dbpath() const;
    Error Open();
    Error Put(const std::string& key, const std::string& value);
    Error Get(const std::string& key, CPPIO_OUT std::string& value);
    Error Close();

private:
    std::unique_ptr<KVSImpl> pKVSImp_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_KVS_H_