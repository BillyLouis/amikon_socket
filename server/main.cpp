#include "amikon_server.h"
#include <iostream>

int main(int argc, char **argv)
{
    std::string address = "127.0.0.1";
    int port = 5000;

    if (argc >= 2)
        address = argv[1];
    if (argc >= 3)
        port = std::atoi(argv[2]);

    AmikonServer server(address, port);

    if (!server.start())
    {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    return 0;
}
