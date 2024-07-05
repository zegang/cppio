#ifndef CPPIO_HTTPSERVER_HPP
#define CPPIO_HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace cppio {

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class HttpServer {

private:
    boost::asio::io_context ioContext;
    tcp::acceptor acceptor;

public:
    HttpServer() : ioContext{1}, acceptor(ioContext, tcp::endpoint(tcp::v4(), 9399)) {}

    void start() {
        std::cout << "Server started on port 9399" << std::endl;
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
    void handle_request(const std::shared_ptr<tcp::socket> socket) {
        try {
            // Create a buffer to hold the incoming request
            beast::flat_buffer buffer;

            // Declare a container to hold the request
            http::request<http::string_body> req;

            std::cout << "To read request: " << req << std::endl;
            // Read the request
            http::read(*socket, buffer, req);
            std::cout << "Received request: " << req << std::endl;

            // Create a response message
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, "Boost Beast Async Server");
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "-------Hello, Boost.Beast Async!\n\n";
            res.prepare_payload();

            // Write the response
            http::write(*socket, res);

            // Perform clean shutdown to close the socket
            socket->shutdown(tcp::socket::shutdown_send);
        } catch (std::exception const& e) {
            if (socket.get() == nullptr) {
                std::cerr << "Socket Nullptr " << std::endl;
            }
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    // This function handles accepting new connections asynchronously
    void start_accept() {
        // Create a socket for the next incoming connection
        auto socket = std::make_shared<tcp::socket>(ioContext);

        // Start asynchronous accept operation
        acceptor.async_accept(*socket, [&, socket](const beast::error_code& ec) {
            if (!ec) {
                std::cout << "Accepted new connection" << std::endl;
                tcp::endpoint remoteEndpoint = socket->remote_endpoint();
                std::cout << "Remote IP address: " << remoteEndpoint.address() << std::endl;
                std::cout << "Remote port      : " << remoteEndpoint.port() << std::endl;
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