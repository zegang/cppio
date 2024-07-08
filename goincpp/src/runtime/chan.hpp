// Copyright 2024 The Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef GOINCPP_RUNTIME_CHAN_HPP
#define GOINCPP_RUNTIME_CHAN_HPP

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace goincpp {
namespace runtime {

template <typename T>
class Channel {
public:
    Channel() : closed(false) {}

    void send(const T& message) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(message);
        condition.notify_one();
    }

    bool receive(T& message) {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]() { return !queue.empty() || closed; });
        if (!queue.empty()) {
            message = queue.front();
            queue.pop();
            return true;
        }
        return false; // Channel is closed and empty
    }

    void close() {
        std::unique_lock<std::mutex> lock(mutex);
        closed = true;
        condition.notify_all();
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool closed;
};

}
}

#endif // GOINCPP_RUNTIME_CHAN_HPP