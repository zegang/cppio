// Copyright 2024 The Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOINCPP_ERRORS_ERRORS_HPP
#define GOINCPP_ERRORS_ERRORS_HPP

#include <exception>
#include <string>
#include <memory>

namespace goincpp {
namespace errors {

class ErrorInterface : public std::exception {

public:
    virtual ~ErrorInterface() {}
    virtual std::string error() const noexcept = 0;
    virtual const char* what() const noexcept = 0;
};

class ErrorString : public ErrorInterface {

public:
    ErrorString(const std::string& msg) : s(msg) {}
    std::string error() const override { return s; }
    const char* what() const noexcept override { return s.c_str(); }

private:
    std::string s;
};

typedef std::shared_ptr<ErrorInterface> Error;

extern Error newError(const std::string& message);

}
}

#endif // GOINCPP_ERRORS_ERRORS_HPP