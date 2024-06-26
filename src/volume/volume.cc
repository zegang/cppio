// Copyright 2025 cppio authors. All rights reserved.

#include "log.h"
#include "volume.h"
#include "layout/topology.h"

namespace CPPIO_NAMESPACE {

VolumeContext::VolumeContext() {}

VolumeContext::VolumeContext(const std::string& json_file_path) {
    volume_ = std::make_shared<Volume>();
    Topology::ParseJson(json_file_path, volume_);
    root_path_ = volume_->url();
    for (auto v: volume_->sub_volumes()) {
        sub_volume_contexts_.push_back(std::make_shared<VolumeContext>(v));
    }
}

VolumeContext::VolumeContext(std::shared_ptr<Volume> volume) : volume_(volume) {
    root_path_ = volume_->url();
}

VolumeContext::VolumeContext(Volume& volume) {
    volume_ = std::make_shared<Volume>(volume);
    root_path_ = volume_->url();
}

std::string VolumeContext::DebugString() {
    std::ostringstream oss;
    oss << "VolumeContext " << this << ": ";
    oss << "Root Path: " << root_path_ << ", ";
    oss << volume_->DebugString();
    oss << std::endl;
    return oss.str();
}

} // namespace CPPIO_NAMESPACE