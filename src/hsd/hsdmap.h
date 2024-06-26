// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_HSDMAP_H_
#define CPPIO_HSDMAP_H_

#include <fstream>
#include <iostream>
#include <string>

#include "error.h"
#include "topology.pb.h"

namespace CPPIO_NAMESPACE {
namespace HSD {

class HSDMap {

public:

private:
    std::string uuid_;

    int64_t num_hsd_;
    int64_t num_up_hsd_;
    
    std::vector<uint32_t> hsd_state_;

};

} // namespace HSD
} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_HSDMAP_H_

