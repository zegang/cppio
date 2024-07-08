#include "errors.hpp"

using namespace goincpp::errors;

Error
newError(const std::string& message) {
    return std::make_shared<ErrorString>(message);
}