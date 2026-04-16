// Copyright 2025 cppio authors. All rights reserved.

#include "metrics_http_server.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

namespace CPPIO_NAMESPACE {

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

MetricsHttpServer::MetricsHttpServer(int port, MetricsProvider provider)
    : io_context_(1),
      acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
      provider_(std::move(provider)) {}

MetricsHttpServer::~MetricsHttpServer() {
    Stop();
}

void MetricsHttpServer::Start() {
    startAccept();
    thread_ = std::make_unique<std::thread>([this] {
        try {
            io_context_.run();
        } catch (const std::exception& e) {
            std::cerr << "MetricsHttpServer run error: " << e.what() << std::endl;
        }
    });
}

void MetricsHttpServer::Stop() {
    if (acceptor_.is_open()) {
        boost::system::error_code ec;
        acceptor_.close(ec);
    }
    io_context_.stop();
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void MetricsHttpServer::startAccept() {
    auto socket = std::make_shared<tcp::socket>(io_context_);
    acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
        if (!ec) {
            handleRequest(socket);
        }
        if (acceptor_.is_open()) {
            startAccept();
        }
    });
}

void MetricsHttpServer::handleRequest(const std::shared_ptr<tcp::socket>& socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(*socket, buffer, req);

        std::string body;
        std::string content_type = "text/plain; version=0.0.4";

        if (req.target() == "/metrics") {
            if (provider_) {
                body = provider_();
            } else {
                body = "# CppIO metrics provider not configured\n";
            }
        } else {
            body = "CppIO metrics server is running. Fetch metrics at /metrics\n";
            content_type = "text/plain";
        }

        http::response<http::string_body> res{
            http::status::ok, req.version()};
        res.set(http::field::server, "CppIO Metrics HTTP Server");
        res.set(http::field::content_type, content_type);
        res.keep_alive(req.keep_alive());
        res.body() = std::move(body);
        res.prepare_payload();

        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
    } catch (const std::exception& e) {
        std::cerr << "MetricsHttpServer request failed: " << e.what() << std::endl;
    }
}

} // namespace CPPIO_NAMESPACE
