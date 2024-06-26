#ifndef CPPIO_ERROR_HPP
#define CPPIO_ERROR_HPP

#include <iostream>
#include <string>
#include <memory>

namespace cppio {

struct ErrorObj {
    int code = 0;
    std::string msg;

    std::string to_string() const;
};

using Error = std::shared_ptr<ErrorObj>;

extern std::ostream& operator<<(std::ostream& os, const ErrorObj& error);

extern std::ostream& operator<<(std::ostream& os, const Error errorPtr);

extern Error newError(int code, const std::string& message);

}

#endif // CPPIO_ERROR_HPP