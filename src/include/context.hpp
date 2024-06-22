#ifndef CPPIO_CONTEXT_HPP
#define CPPIO_CONTEXT_HPP

#include <iostream>
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <functional>

namespace cppio {

// Context class to handle cancellation
class Context {
private:
    std::atomic<bool> canceled { false };
    std::mutex mutex;
    std::condition_variable cv;

public:
    void cancel() {
        std::unique_lock<std::mutex> lock(mutex);
        canceled = true;
        cv.notify_all();
    }

    bool isCanceled() const {
        return canceled;
    }

    template<typename F>
    void runAsync(F&& func) {
        std::thread([this, func = std::forward<F>(func)]() mutable {
            if (!isCanceled()) {
                func();
            }
        }).detach();
    }

    template<typename R, typename F>
    std::future<R> runAsyncWithFuture(F&& func) {
        std::promise<R> promise;
        auto future = promise.get_future();

        std::thread([this, func = std::forward<F>(func), promise = std::move(promise)]() mutable {
            if (!isCanceled()) {
                try {
                    promise.set_value(func());
                } catch (...) {
                    promise.set_exception(std::current_exception());
                }
            }
        }).detach();

        return future;
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return canceled; });
    }
};

}

// // Example function to simulate async work
// int asyncWork() {
//     std::this_thread::sleep_for(std::chrono::seconds(3));
//     return 42;
// }

// int main() {
//     Context ctx;

//     // Run async function with cancellation check
//     ctx.runAsync([ctx]() {
//         if (!ctx.isCanceled()) {
//             std::cout << "Async operation started..." << std::endl;
//             asyncWork();
//             std::cout << "Async operation completed." << std::endl;
//         }
//     });

//     // Simulate cancellation after 2 seconds
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     ctx.cancel();

//     // Wait for async operation to finish
//     ctx.wait();

//     std::cout << "Main thread: Context canceled." << std::endl;

//     return 0;
// }

#endif // CPPIO_CONTEXT_HPP