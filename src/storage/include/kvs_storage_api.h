// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_KVS_STORAGE_API_H_
#define CPPIO_KVS_STORAGE_API_H_

#include <string>

#include "error.h"

namespace CPPIO_NAMESPACE {

class KVSStorageApi {
public:
    Error Open();
    Error Put(const std::string& key, const std::string& value);
    Error Get(const std::string& key, CPPIO_OUT std::string& value);
    Error Delete(const std::string& key);
    Error Close();
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_KVS_STORAGE_API_H_