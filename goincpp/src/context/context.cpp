#include "context.hpp"

using namespace goincpp::context;

int CancelCtx::cancelCtxKey = 0;

std::shared_ptr<UnbufferedChannel>
CancelCtx::done() override {
    auto d = _done.load();
    if (d != nullptr) {
        return d;
    }
    std::lock_guard<std::mutex> lock(_mu);
    d = _done.load();
    if (d == nullptr) {
        _done.store(UnbufferedChannel::make());
    }
    return _done.load();
}

Error CancelCtx::err() override {
    std::lock_guard<std::mutex> lock(_mu);
    return _err;
}

std::optional<std::any> CancelCtx::
value(std::any key) override {
    if (key.type() == typeid(int*) &&
        std::any_cast<int*>(key) == &CancelCtx::cancelCtxKey) {
        return std::optional<std::any>(std::any(shared_from_this()));
    }
    return value(_context, key);
}

void
CancelCtx::propagateCancel(std::shared_ptr<Context> parent,
                           std::shared_ptr<Canceler> child) {
    _context = parent;

    auto done = parent->done();
    if (done == nullptr) {
        return; // parent is never canceled
    }

    if (done->select()) {
        // parent is already canceled
        child->cancel(false, parent->err(), Cause(parent));
        return;
    }
}

void
CancelCtx::cancel(bool removeFromParent, Error err, Error cause) override {
    if (err == nullptr) {
        panic("context: internal error: missing cancel error")
    }
    if (cause == nullptr) {
        cause = err;
    }

    do {
        std::lock_guard<std::mutex> lock(_mu);
        if (_err) {
            return; // already canceled
        }

        _err = err;
        _cause = cause;

        auto d = _done.load();
        if (d == nullptr) {
            _done.store();
        } else {
            close(d);
        }

        for (auto& child : _children) {
            // Cancel child cancelers if any
            child->cancel(false, err, cause);
        }
        children.clear();
    } while(0);

    if (removeFromParent) {
        removeChild(_context);
    }
}

Error Canceled = newError("context canceled");

std::pair<std::shared_ptr<CancelCtx>, CancelFunc>
withCancel(std::shared_ptr<Context> parent) {
    if (parent == nullptr) {
        panic("cannot create context from nil parent");
    }
    auto c = std::make_shared<CancelCtx>();
    c->propagateCancel(parent);
    return { c, [c]() { c->cancel(true, Canceled, nullptr); } };
}