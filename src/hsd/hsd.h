// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Cppio - Hybrid Storage System with Freely Combine
 *
 * Copyright 2025 cppio authors. All rights reserved.
 *
 */

 /*
  * Hybrid Storage Daemon
  */

#ifndef CPPIO_HSD_H_
#define CPPIO_HSD_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "error.h"
#include "component.h"

namespace CPPIO_NAMESPACE {

class HSD final {
public:
    HSD(const std::string& name);
    ~HSD();

    std::string name() { return name_; }

    Error Start(int argc, char* argv[]);
    Error Shutdown();
    Error Stop();

    Error RegisterComponent(std::unique_ptr<Component>&& component);

private:
    std::string name_;
    std::atomic<HSDState> state_;
    std::vector<std::unique_ptr<Component> > components_;
    std::atomic<bool> stop_required_;
    std::mutex mtx_;
    std::condition_variable cv_;

private:
    Error setState(HSDState state);
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_HSD_H_