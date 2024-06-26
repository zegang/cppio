// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_NET_H_
#define CPPIO_NET_H_

#include <string>
#include <algorithm>

namespace CPPIO_NAMESPACE {
namespace net {

std::string parse_host(const std::string& address) {
    size_t start = address.find("//");
    if (start != std.string::npos) {
        start += 2;
    } else {
        start = 0;
    }
    size_t end = address.find(":", start);
    if (end == std::string::npos) {
        end = address.find("/", start);
    }
    if (end == std::string::npos) {
        return address.substr(start);
    }
    return address.substr(start, end - start);
}


Error check_local_serveraddr(const std::string& serverAddr) {
    return nullptr;
}

} // namespace net
} // namespace CPPIO_NAMESPACE

#endif // CPPIO_NET_H_