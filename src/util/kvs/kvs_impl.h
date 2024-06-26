// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_KVS_IMPL_h
#define CPPIO_KVS_IMPL_h

#include <string>
#include <memory>

#include "kvs.h"
#include "../../debug/debug.h"
#include "../../include/error.h"

namespace CPPIO_NAMESPACE {

class KVSImpl {
public:
    KVSImpl(const std::string& dbpath) { dbpath_ = dbpath; }
    virtual ~KVSImpl() = default;

    static std::unique_ptr<KVSImpl> GetKVSImpl(KVSDBType dbtype,
        const std::string& dbpath);

    const std::string& dbpath() const { return dbpath_; }

    virtual Error Open() = 0;
    virtual Error Put(const std::string& key, const std::string& value) = 0;
    virtual Error Get(const std::string& key, CPPIO_OUT std::string& value) = 0;

    virtual Error Close() = 0;

protected:
    std::string dbpath_;
};


} // namespace CPPIO_NAMESPACE

#endif // CPPIO_KVS_IMPL_h