// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_TOPOLOGY_H_
#define CPPIO_TOPOLOGY_H_

#include <fstream>
#include <iostream>
#include <google/protobuf/util/json_util.h>
#include <string>
#include <boost/url/url.hpp>

#include "error.h"
#include "topology.pb.h"

namespace CPPIO_NAMESPACE {
namespace Topology {

static Error ParseJson(const std::string& file_path, CPPIO_OUT Volume& volume) {
    Error err = ErrorOK;

    std::ifstream json_file(file_path);
    std::string file_content((std::istreambuf_iterator<char>(json_file)),
                std::istreambuf_iterator<char>());
    
    google::protobuf::util::JsonParseOptions options;
    auto status =
        google::protobuf::util::JsonStringToMessage(file_content, &volume, options);

    if (!status.ok()) {
        std::cerr << "Error parsing JSON: " << status.message() << std::endl;
    }

    return err;
}

static Error ParseJson(const std::string& file_path, CPPIO_OUT std::shared_ptr<Volume> volume) {
    Error err = ErrorOK;

    std::ifstream json_file(file_path);
    std::string file_content((std::istreambuf_iterator<char>(json_file)),
                std::istreambuf_iterator<char>());
    
    google::protobuf::util::JsonParseOptions options;
    auto status =
        google::protobuf::util::JsonStringToMessage(file_content, volume.get(), options);

    if (!status.ok()) {
        std::cerr << "Error parsing JSON: " << status.message() << std::endl;
    }

    return err;
}

} // namespace Topology
} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_TOPOLOGY_H_