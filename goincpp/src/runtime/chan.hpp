// Copyright 2024 The Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOINCPP_RUNTIME_CHAN_HPP
#define GOINCPP_RUNTIME_CHAN_HPP

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <semaphore>

namespace goincpp {
namespace runtime {

template <typename T, int Capacity>
class Channel : public std::enable_shared_from_this< Channel<T, Capacity> > {
private:
    Channel() : _closed(false) {}

public:
    static std::shared_ptr<Channel<T, Capacity>> make() {
        auto cha = std::make_shared<Channel<T, Capacity>>();
        return cha;
    }

    void send(const T& message) {
        do {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_closed) {
                return;
            }
            _queue.push(message);
            _condition.notify_one();
        } while(0);
    }

    bool receive(T& message) {
        do {
            std::lock_guard<std::mutex> lock(_mutex);
            condition.wait(lock, [this]() { return !_queue.empty() || _closed; });
            if (_close) {
                return true;
            }
            if (!_queue.empty()) {
                message = _queue.front();
                _queue.pop();
            }
        } while(0);

        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(_mutex);
        _closed = true;
        _condition.notify_all();
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _closed;
};

template <typename T,  0>
class Channel : public std::enable_shared_from_this< Channel<T, 0> > {
private:
    Channel() : _semaphore(0), _closed(false) {}

public:
    static std::shared_ptr<Channel<T, 0>> make() {
        auto cha = std::make_shared<Channel<T, 0>>();
        return cha;
    }

    void send() {
        do {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_closed) {
                return;
            }
            _condition.notify_one();
        } while(0);
        _semaphore.acquire();
    }

    bool select() {
        do {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_close) {
                return true;
            }
            
            if (condition.wait_until(lock, 0) == std::cv_status::timeout) {
                return false;
            }
        } while(0);
        _semaphore.release();
        return true;
    }

    bool receive() {
        do {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_close) {
                return true;
            }
            condition.wait(lock);
        } while(0);
        _semaphore.release();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(_mutex);
        _closed = true;
        _condition.notify_all();
        _semaphore.release();
    }

private:
    std::binary_semaphore _semaphore;
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _closed;
};

typedef Channel<std::any, 0> UnbufferedChannel;

}
}

#endif // GOINCPP_RUNTIME_CHAN_HPP