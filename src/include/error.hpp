#ifndef CPPIO_CLI_HPP
#define CPPIO_CLI_HPP

#include <string>
#include <memory>

namespace cppio {

class ErrorObj {

public:
    ErrorObj(int c, const std::string& message) : code(c), msg(message) {}

    std::string to_string() const {
        return "Error[" +  std::to_string(code) + "]: " + msg;
    }

    friend std::ostream& operator<<(std::ostream& os, const ErrorObj& error) {
        os << error.to_string();
        return os;
    }

private:
    int code = 0;
    std::string msg;

};

typedef std::shared_ptr<ErrorObj> Error;

// Overload operator<< for Error (which is std::shared_ptr<ErrorObj>)
std::ostream& operator<<(std::ostream& os, const Error& errorPtr) {
    if (errorPtr) {
        os << *errorPtr; // Dereference the shared_ptr to use the operator<< for ErrorObj
    } else {
        os << "No Error[nullptr]"; // Handle the case where Error is nullptr
    }
    return os;
}

Error newError(int code, const std::string& message) {
    return std::make_shared(ErrorObj(code, message));
}

}

#endif // CPPIO_ERROR_HPP