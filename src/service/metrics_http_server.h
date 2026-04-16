// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_METRICS_HTTP_SERVER_H_
#define CPPIO_METRICS_HTTP_SERVER_H_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace CPPIO_NAMESPACE {

class MetricsHttpServer {
public:
    using MetricsProvider = std::function<std::string()>;

    explicit MetricsHttpServer(int port = 9399, MetricsProvider provider = nullptr);
    ~MetricsHttpServer();

    void Start();
    void Stop();

private:
    void startAccept();
    void handleRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    MetricsProvider provider_;
    std::unique_ptr<std::thread> thread_;
};

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_METRICS_HTTP_SERVER_H_
