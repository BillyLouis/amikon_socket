#include "amikon_server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

AmikonServer::AmikonServer(const std::string &address, int port)
    : address_(address), port_(port), server_fd_(-1), running_(false)
{
}

AmikonServer::~AmikonServer()
{
    stop();
}

bool AmikonServer::start()
{
    if (!init_socket())
        return false;

    running_ = true;

    std::cout << "[SERVER] Listening on " << address_ << ":" << port_ << std::endl;

    // Accept loop runs on main thread for now
    accept_loop();
    return true;
}

void AmikonServer::stop()
{
    running_ = false;

    if (server_fd_ != -1)
        close(server_fd_);

    for (auto &t : client_threads_)
        if (t.joinable())
            t.join();
}

bool AmikonServer::init_socket()
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        std::cerr << "[SERVER] Failed to create socket\n";
        return false;
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    inet_pton(AF_INET, address_.c_str(), &server_addr.sin_addr);

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd_, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "[SERVER] Bind failed\n";
        return false;
    }

    if (listen(server_fd_, 10) < 0)
    {
        std::cerr << "[SERVER] Listen failed\n";
        return false;
    }

    return true;
}

void AmikonServer::accept_loop()
{
    while (running_)
    {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0)
        {
            if (running_)
                std::cerr << "[SERVER] Accept failed\n";
            continue;
        }

        std::cout << "[SERVER] Client connected\n";

        client_threads_.push_back(std::thread(&AmikonServer::handle_client, this, client_fd));
    }
}

void AmikonServer::handle_client(int client_fd)
{
    char buffer[2048];
    std::memset(buffer, 0, sizeof(buffer));

    int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes <= 0)
    {
        close(client_fd);
        return;
    }

    std::string msg(buffer);

    std::cout << "[SERVER] Received:\n" << msg << std::endl;

    if (is_valid_xml(msg))
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        work_queue_.push(msg);
        std::cout << "[SERVER] XML queued for processing\n";
    }
    else
    {
        std::cout << "[SERVER] Unknown Command (invalid XML)\n";
    }

    // Temporary response
    std::string response = "<response><status>OK</status></response>";
    write(client_fd, response.c_str(), response.size());

    close(client_fd);
}

bool AmikonServer::is_valid_xml(const std::string &msg)
{
    // Placeholder: real XML parsing will be added later
    return msg.find("<request>") != std::string::npos;
}