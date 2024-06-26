// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_COMPONENT_H_
#define CPPIO_COMPONENT_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <utility>

#include "error.h"

namespace CPPIO_NAMESPACE {

enum class HSDState : int {
    HSDSTATE_UNKNOWN = 0,
    HSDSTATE_STARTREQ,      /* start required */
    HSDSTATE_STARTING,
    HSDSTATE_STARTTED,
    HSDSTATE_SHUTDOWNREQ,   /* shutdown required */
    HSDSTATE_SHUTDOWNING,
    HSDSTATE_SHUTDOWNDONE,
};

static const char* HSDStateStr[] = {
    "HSDSTATE_UNKNOWN",
    "HSDSTATE_STARTREQ",
    "HSDSTATE_STARTING",
    "HSDSTATE_STARTTED",
    "HSDSTATE_SHUTDOWNREQ",
    "HSDSTATE_SHUTDOWNING",
    "HSDSTATE_SHUTDOWNDONE",
};

static const inline char* EnumToString(HSDState enum_item) {
    return HSDStateStr[std::to_underlying(enum_item)];
}

using ComponentState = HSDState;

class Component {
public:
    Component(const std::string& name) : name_(name) {}
    ~Component() {}

    std::string     name() { return name_; }
    virtual bool    Startable() = 0;
    virtual Error   Start() = 0;
    virtual Error   Shutdown() = 0;

private:
    std::string name_;
    std::atomic<HSDState> state_;
};

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_COMPONENT_H_