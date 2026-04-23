//
// Copyright 2025 cppio authors.
//

#ifndef CPPIO_ERROR_HPP
#define CPPIO_ERROR_HPP

#include <iostream>
#include <exception>
#include <system_error>
#include <ios>
#include <future>
#include <string>
#include <memory>

namespace CPPIO_NAMESPACE {

template<typename ErrorCodeEnum>
concept ErrorCodeEnumTypes = std::is_same_v<ErrorCodeEnum, std::errc> ||
                             std::is_same_v<ErrorCodeEnum, std::io_errc> ||
                             std::is_same_v<ErrorCodeEnum, std::future_errc>;

template <ErrorCodeEnumTypes ErrorCodeEnum>
class ErrorObj {
public:

    ErrorObj(ErrorCodeEnum ecode, const std::string& file,
             const std::string& func, int lineno, const std::string& msg)
             : cond_(std::make_error_condition(ecode)),
               file_(file), func_(func), lineno_(lineno), msg_(msg) {}
    
    ErrorObj(ErrorCodeEnum ecode, const char* file,
             const char* func, int lineno, const char* msg)
            : cond_(std::make_error_condition(ecode)),
              file_(file), func_(func), lineno_(lineno), msg_(msg) {}

    std::string ToString() const {
        return "Error: <" + std::to_string(cond_.value()) + ", "
                          + msg_ + ", "
                          + file_ + ", " + func_ + ", "
                          + std::to_string(lineno_) + ", "
                          + cond_.message()
                          + ">";
    }

    ErrorCodeEnum code() { return static_cast<ErrorCodeEnum>(cond_.value()); }

    bool operator==(const ErrorObj& other) const {
        return cond_ == other.cond_ &&
               file_ == other.file_ &&
               func_ == other.func_ &&
               lineno_ == other.lineno_ &&
               msg_ == other.msg_;
    }

    template<ErrorCodeEnumTypes ErrorCodeEnumB>
    bool operator==(ErrorCodeEnumB code) {
        return cond_ == code;
    }

    template<typename ErrorCode>
    bool operator==(ErrorCode code) {
        static_assert(std::is_base_of_v<std::error_condition, ErrorCode> ||
                      std::is_base_of_v<std::error_code, ErrorCode>,
                      "Only std::error_condition/error_code family are supported for comparison");
        return cond_ == code;
    }

private:
    std::error_condition cond_;
    std::string file_;
    std::string func_;
    int lineno_;
    std::string msg_;
};

template <ErrorCodeEnumTypes ErrorCodeEnum>
using ErrorPtr = std::shared_ptr<ErrorObj<std::remove_cv_t<ErrorCodeEnum> > >;

using Error = ErrorPtr<std::errc>;

#define ErrorOK nullptr
#define ERROR_OK ErrorOK
#define ERROR_OK_STR "No Error[nullptr]"

template<ErrorCodeEnumTypes ErrorCodeEnum>
bool operator==(Error error, ErrorCodeEnum code) {
    if (error) {
        return *error == code;
    } else {
        return false;
    }
}

template <typename ErrorCodeEnum>
inline ErrorPtr<ErrorCodeEnum> MakeError(
    ErrorCodeEnum ecode, const char* file,
    const char* func, int lineno, const char* msg)
{
    return std::make_shared<ErrorObj<ErrorCodeEnum> >(ecode, file, func, lineno, msg);
}

template <typename ErrorCodeEnum>
inline std::string ToString(ErrorPtr<ErrorCodeEnum> error) {
    if (error) {
        return error->ToString();
    } else {
        return ERROR_OK_STR;
    }
}

template <typename ErrorCodeEnum>
inline std::ostream& operator<<(std::ostream& os, const ErrorPtr<ErrorCodeEnum> error) {
    if (error) {
        os << error->ToString();
    } else {
        os << ERROR_OK_STR;
    }
    return os;
}

#define MAKE_ERROR(ecode, msg) \
            MakeError(ecode, __FILE__, __FUNCTION__, __LINE__, msg)

} // namespace CPPIO_NAMESPACE 

#endif // CPPIO_ERROR_HPP