#include <iostream>
#include "server.h"

int main() {
    auto config_file = "config.ini";
    if (!boost::filesystem::exists(config_file)) {
        std::cerr << "config not exists." << std::endl;
        return -1;
    }
    boost::property_tree::ptree root_node;
    boost::property_tree::ini_parser::read_ini(config_file, root_node);
    auto server_tag = root_node.get_child("Server");
    auto port_tag = server_tag.get_child("port");
    auto port = port_tag.get_value("");
    login_server::Server(std::stoi(port));
    return 0;
}
