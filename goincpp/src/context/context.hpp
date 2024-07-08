// Copyright 2024 The Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOINCPP_CONTEXT_CONTEXT_HPP
#define GOINCPP_CONTEXT_CONTEXT_HPP

#include <iostream>
#include <fstream>
#include <vector>

namespace goincpp {
namespace context {

class Context {
public:
    virtual ~Context() = default;

    // a Done channel for cancellation.
    virtual void done() = 0;

}

}
}

#endif // GOINCPP_CONTEXT_CONTEXT_HPP