#include "amikon_client.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

AmikonClient::AmikonClient(const std::string &address, int port)
    : address_(address), port_(port), sock_fd_(-1)
{
}

AmikonClient::~AmikonClient()
{
    if (sock_fd_ != -1)
        close(sock_fd_);
}

bool AmikonClient::connect_to_server()
{
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0)
    {
        std::cerr << "[CLIENT] Failed to create socket\n";
        return false;
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    inet_pton(AF_INET, address_.c_str(), &server_addr.sin_addr);

    if (connect(sock_fd_, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "[CLIENT] Connection failed\n";
        return false;
    }

    std::cout << "[CLIENT] Connected to server\n";
    return true;
}

bool AmikonClient::send_message(const std::string &xml)
{
    if (sock_fd_ < 0)
        return false;

    int sent = write(sock_fd_, xml.c_str(), xml.size());
    if (sent < 0)
    {
        std::cerr << "[CLIENT] Failed to send message\n";
        return false;
    }

    return true;
}

std::string AmikonClient::receive_response()
{
    char buffer[2048];
    std::memset(buffer, 0, sizeof(buffer));

    int bytes = read(sock_fd_, buffer, sizeof(buffer) - 1);
    if (bytes <= 0)
    {
        return "";
    }

    return std::string(buffer);
}
