// Copyright 2024 The Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOINCPP_CONTEXT_CONTEXT_HPP
#define GOINCPP_CONTEXT_CONTEXT_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <optional>

#include "../errors/errors.hpp"
#include "../runtime/chan.hpp"

namespace goincpp {
namespace context {

using UnbufferedChannel = goincpp::runtime::UnbufferedChannel;
using Error = goincpp::errors::Error;

// Forward declarations
class Context;
class Canceler;
class CancelCtx;

// A Context carries a deadline, a cancellation signal, and other values across
// API boundaries.
//
// Context's methods may be called by multiple goroutines simultaneously.
class Context : public std::enable_shared_from_this< Context > {
public:
    virtual ~Context() {}

    // Deadline returns the time when work done on behalf of this context
	// should be canceled. Deadline returns ok==false when no deadline is
	// set. Successive calls to Deadline return the same results.
	virtual std::optional<std::chrono::time_point<std::chrono::system_clock>> deadline() const = 0;

    // Done returns a channel that's closed when work done on behalf of this
    // context should be canceled. Done may return nil if this context can
	// never be canceled. Successive calls to Done return the same value.
	// The close of the Done channel may happen asynchronously,
	// after the cancel function returns.
    virtual std::shared_ptr<UnbufferedChannel> done() = 0;

    // If Done is not yet closed, Err returns nil.
	// If Done is closed, Err returns a non-nil error explaining why:
	// Canceled if the context was canceled
	// or DeadlineExceeded if the context's deadline passed.
	// After Err returns a non-nil error, successive calls to Err return the same error.
    virtual Error err() = 0;

    // Value returns the value associated with this context for key, or nil
	// if no value is associated with key. Successive calls to Value with
	// the same key returns the same result.
	//
	// Use context values only for request-scoped data that transits
	// processes and API boundaries, not for passing optional parameters to
	// functions.
	//
    virtual std::optional<std::any> value(std::any key) const = 0;
};

// An emptyCtx is never canceled, has no values, and has no deadline.
// It is the common base of backgroundCtx and todoCtx.
class EmptyCtx : public Context {
public:
    virtual std::optional<std::chrono::time_point<std::chrono::system_clock>>
    deadline() const override {
        return {};
    }
    virtual std::shared_ptr<UnbufferedChannel> done() override { return nullptr; }
    virtual Error err() override { return nullptr; }
    virtual std::optional<std::any> value(std::any key) { return {}; }
};

class BackgroundCtx : public EmptyCtx {
public:
    std::string string() {
        return "context.Background";
    }
};

class TodoCtx : public EmptyCtx {
public:
    std::string string() {
        return "context.TODO";
    }
};

// Background returns a non-nil, empty [Context]. It is never canceled, has no
// values, and has no deadline. It is typically used by the main function,
// initialization, and tests, and as the top-level Context for incoming
// requests.
std::shared_ptr<Context> background() {
    return std::make_shared_ptr<BackgroundCtx>();
}

// TODO returns a non-nil, empty [Context]. Code should use context.TODO when
// it's unclear which Context to use or it is not yet available (because the
// surrounding function has not yet been extended to accept a Context
// parameter).
std::shared_ptr<Context> todo() {
    return std::make_shared_ptr<TodoCtx>();
}

// A canceler is a context type that can be canceled directly. The
// implementations are *cancelCtx and *timerCtx.
class Canceler {
public:
    virtual ~Canceler() {}
	virtual void cancel(bool removeFromParent, Error err, Error cause) = 0;
	virtual std::shared_ptr<UnbufferedChannel> done() = 0;
}

// A cancelCtx can be canceled. When canceled, it also cancels any children
// that implement canceler.
class CancelCtx : public Context, public std::enable_shared_from_this<CancelCtx> {
public:
    static int cancelCtxKey;
    CancelCtx() : _done(false), _err(nullptr), _cause(nullptr) {}

    virtual std::shared_ptr<UnbufferedChannel> done() override;
    virtual Error err() override;
    virtual std::optional<std::any> value(std::any key) override;
    // propagateCancel arranges for child to be canceled when parent is.
    // It sets the parent context of cancelCtx.
    void propagateCancel(std::shared_ptr<Context> parent, std::shared_ptr<Canceler> child);
    // cancel closes c.done, cancels each of c's children, and, if
    // removeFromParent is true, removes c from its parent's children.
    // cancel sets c.cause to cause if this is the first time c is canceled.
    void cancel(bool removeFromParent, Error err, Error cause) override;

private:
    std::shared_ptr<Context> _context;
    mutable std::mutex _mu;
    std::atomic< std::shared_ptr< UnbufferedChannel > > _done;
    std::unordered_set<std::shared_ptr<Canceler>> _children;
    Error _err;
    Error _cause;
};

using CancelFunc = std::function<void()>;
extern Error Canceled;

extern std::pair<std::shared_ptr<CancelCtx>, CancelFunc> withCancel(std::shared_ptr<Context> parent);

}
}

#endif // GOINCPP_CONTEXT_CONTEXT_HPP