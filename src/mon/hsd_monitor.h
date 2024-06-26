// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_MONITOR_H_
#define CPPIO_MONITOR_H_

/* CppIO HSD Monitor
 *
 * The CppIO HSD Monitor is a crucial component of the Ceph storage cluster,
 * responsible for maintaining the cluster's state and metadata.
 */

#include <fstream>
#include <iostream>
#include <google/protobuf/util/json_util.h>
#include <string>
#include <boost/url/url.hpp>

#include "error.h"
#include "topology.pb.h"

namespace CPPIO_NAMESPACE {
namespace Monitor {

class HSDMonitor {

public:
    HSDMap hsdmap;
};

} // namespace Monitor
} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_MONITOR_H_
