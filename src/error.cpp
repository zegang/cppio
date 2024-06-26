#include "include/error.hpp"

using namespace cppio;

std::string ErrorObj::to_string() const {
    return "Error[" +  std::to_string(code) + "]: " + msg;
}

std::ostream& operator<<(std::ostream& os, const ErrorObj& error) {
    os << error.to_string();
    return os;
}

// Overload operator<< for Error (which is std::shared_ptr<ErrorObj>)
std::ostream& operator<<(std::ostream& os, const Error error) {
    if (error) {
        os << error->to_string();
    } else {
        os << "No Error[nullptr]";
    }
    return os;
}

Error newError(int code, const std::string& message) {
    return std::make_shared<ErrorObj>(ErrorObj{code, message});
}