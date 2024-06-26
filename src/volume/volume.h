// Copyright (C) 2025 cppio authors. All rights reserved.

#ifndef CPPIO_VOLUME_H_
#define CPPIO_VOLUME_H_

#include <any>
#include <memory>

#include "error.h"
#include "topology.pb.h"

namespace CPPIO_NAMESPACE {

class VolumeContext {
public:
    VolumeContext();
    VolumeContext(const std::string& json_file_path);
    VolumeContext(std::shared_ptr<Volume> volume);
    VolumeContext(Volume& volume);

    auto volume() { return volume_; }
    auto root_path() { return root_path_; }
    auto sub_volume_contexts() { return sub_volume_contexts_; }

    /**
     * @brief Ownning sub volumes or storage endpoints
     * @return True or False
     */
    bool has_sub_elements() {
        return volume_->sub_volumes_size() != 0;
    }

    std::string DebugString();

private:
    std::shared_ptr<Volume> volume_;
    std::string root_path_;
    std::vector<std::shared_ptr<VolumeContext>> sub_volume_contexts_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_VOLUME_H_