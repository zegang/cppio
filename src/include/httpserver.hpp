#ifndef CPPIO_HTTPSERVER_HPP
#define CPPIO_HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace cppio {

using tcp = boost::asio::ip::tcp;         // from <boost/asio/ip/tcp.hpp>

class HttpServer {

private:
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::acceptor acceptor;

public:
    HttpServer() : acceptor(ioContext, tcp::endpoint(tcp::v4(), 9399)) {}

    void start() {
        // Start accepting connections asynchronously
        start_accept();
        // Run the io_context to handle asynchronous operations
        ioContext.run();
    }

    void stop() {
        // Stop accepting new connections
        acceptor.close();
        // Cancel any outstanding asynchronous operations
        ioContext.stop();
    }

private:
    // This function handles an HTTP server request
    void handle_request(const std::shared_ptr<tcp::socket>& socket) {
        try {
            // Create a buffer to hold the incoming request
            boost::beast::flat_buffer buffer;

            // Declare a container to hold the request
            boost::beast::http::request<boost::beast::http::string_body> req;

            // Read the request
            boost::beast::http::read(*socket, buffer, req);

            // Create a response message
            boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
            res.set(boost::beast::http::field::server, "Boost Beast Async Server");
            res.set(boost::beast::http::field::content_type, "text/html");
            // res.keep_alive(req.keep_alive());
            res.body() = "Hello, Boost.Beast Async!";
            res.prepare_payload();

            // Write the response
            boost::beast::http::write(*socket, res);

            // Perform clean shutdown to close the socket
            socket->shutdown(tcp::socket::shutdown_send);
        } catch (std::exception const& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    // This function handles accepting new connections asynchronously
    void start_accept() {
        // Create a socket for the next incoming connection
        auto socket = std::make_shared<tcp::socket>(ioContext);

        // Start asynchronous accept operation
        acceptor.async_accept(*socket, [&](const boost::beast::error_code& ec) {
            if (!ec) {
                // Successfully accepted a connection, handle it
                handle_request(socket);

                // Continue accepting new connections
                start_accept();
            } else {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
        });
    }

};

}

#endif // CPPIO_HTTPSERVER_HPP