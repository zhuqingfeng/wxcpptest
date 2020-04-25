//
// Created by lionel on 2020/4/25.
//

#ifndef LOGINSERVER_SERVER_H
#define LOGINSERVER_SERVER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace login_server {

    struct accept_callback_data;
    struct session_data;

    class Server {
    private:
        int port;
        boost::asio::io_service *io_service;
        boost::asio::ip::tcp::acceptor *acceptor;
        std::map<std::string, boost::shared_ptr<session_data>> login_mapper;

        void init_server();

        void create_session(boost::shared_ptr<boost::asio::ip::tcp::socket> conn);

        void handle_request(boost::shared_ptr<session_data> data,
                            const boost::system::error_code &error,
                            const std::size_t bytes_transferred);

        static void handle_accept(accept_callback_data *callback_data, const boost::system::error_code &err);

    public:
        Server(int port);
    };

}


#endif //LOGINSERVER_SERVER_H
