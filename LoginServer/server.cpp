//
// Created by lionel on 2020/4/25.
//

#include "server.h"

namespace login_server {

    struct accept_callback_data {
        Server *server;
        boost::shared_ptr<boost::asio::ip::tcp::socket> conn;
    };

    struct session_data {
        boost::shared_ptr<boost::asio::ip::tcp::socket> conn;
        unsigned char buffer[512];
        unsigned int status = 0; // 0: user; 1: password; 2: success
        std::string user;
    };

    Server::Server(int port) {
        this->port = port;

        init_server();
    }

    void Server::handle_accept(
            accept_callback_data *callback_data,
            const boost::system::error_code &err
    ) {
        callback_data->server->create_session(callback_data->conn);

        auto service = callback_data->server->io_service;
        boost::shared_ptr<boost::asio::ip::tcp::socket> s(new boost::asio::ip::tcp::socket(*service));

        callback_data->conn = s;
        callback_data->server->acceptor->async_accept(*s, boost::bind(handle_accept, callback_data, _1));
    }

    void Server::init_server() {
        boost::asio::io_service service;
        this->io_service = &service;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        boost::asio::ip::tcp::acceptor acc(service, endpoint);
        this->acceptor = &acc;
        boost::shared_ptr<boost::asio::ip::tcp::socket> s(new boost::asio::ip::tcp::socket(service));

        auto callback_data = new accept_callback_data();
        callback_data->conn = s;
        callback_data->server = this;
        acceptor->async_accept(*s, boost::bind(handle_accept, callback_data, _1));
        service.run();
    }

    void Server::create_session(boost::shared_ptr<boost::asio::ip::tcp::socket> conn) {
        std::cout << "connected from "
                  << conn->remote_endpoint().address().to_string()
                  << ":"
                  << conn->remote_endpoint().port()
                  << std::endl;

        boost::shared_ptr<session_data> session(new session_data());
        session->conn = conn;

        auto callback = [this, session](const boost::system::error_code &error, std::size_t bytes_transferred) -> void {
            this->handle_request(session, error, bytes_transferred);
        };
        conn->async_read_some(boost::asio::buffer(session->buffer, 512), callback);
    }

    void Server::handle_request(
            boost::shared_ptr<session_data> data,
            const boost::system::error_code &error,
            const std::size_t bytes_transferred
    ) {
        if (error.failed()) {
            return;
        }

        std::cout << "receive from "
                  << data->conn->remote_endpoint().address().to_string()
                  << ":"
                  << data->conn->remote_endpoint().port()
                  << " :[ "
                  << bytes_transferred
                  << " bytes ]"
                  << std::endl;

        if (0 == bytes_transferred) {
            // close
            data->conn->close();
            return;
        }

        // TODO 搞毛分包处理

        unsigned char *data_recv = data->buffer;

        auto request_str = std::string(data_recv, data_recv + bytes_transferred);

        if (0 == data->status) {
            data->user = request_str;
            data->status = 1;
        } else if (1 == data->status) {
            // TODO check password

            // kick/add user
            auto to_kick = this->login_mapper.find(data->user);
            if (to_kick != this->login_mapper.end()) {
                to_kick->second->conn->close();
            }
            this->login_mapper[data->user] = data;
            data->status = 2;
        }

        auto callback = [this, data](const boost::system::error_code &error, std::size_t bytes_transferred) -> void {
            this->handle_request(data, error, bytes_transferred);
        };
        data->conn->async_read_some(boost::asio::buffer(data->buffer, 512), callback);
    }
}
