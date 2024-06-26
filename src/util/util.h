// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_UTIL_UTIL_H_
#define CPPIO_UTIL_UTIL_H_

#include <vector>
#include <string>
#include <numeric>

namespace CPPIO_NAMESPACE {

inline std::string concatenate_vector(const std::vector<std::string>& strings,
                               const std::string& delimiter = "") {
    return std::accumulate(
        strings.begin(), strings.end(), std::string{},
        [&delimiter](std::string a, std::string b) { return a + delimiter + b; });
}

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_UTIL_UTIL_H_