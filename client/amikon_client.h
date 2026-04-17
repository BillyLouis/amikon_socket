#ifndef AMIKON_CLIENT_H
#define AMIKON_CLIENT_H

#include <string>

class AmikonClient
{
public:
    AmikonClient(const std::string &address = "127.0.0.1", int port = 5000);
    ~AmikonClient();

    bool connect_to_server();
    bool send_message(const std::string &xml);
    std::string receive_response();

private:
    std::string address_;
    int port_;
    int sock_fd_;
};

#endif
