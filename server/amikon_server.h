#ifndef AMIKON_SERVER_H
#define AMIKON_SERVER_H

#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

class AmikonServer
{
public:
    AmikonServer(const std::string &address = "127.0.0.1", int port = 5000);
    ~AmikonServer();

    bool start();
    void stop();

private:
    std::string address_;
    int port_;
    int server_fd_;
    bool running_;

    std::vector<std::thread> client_threads_;

    // Shared work queue (future XML processing)
    std::queue<std::string> work_queue_;
    std::mutex queue_mutex_;

    // Internal helpers
    bool init_socket();
    void accept_loop();
    void handle_client(int client_fd);

    // Placeholder XML validation
    bool is_valid_xml(const std::string &msg);
};

#endif