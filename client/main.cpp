#include "amikon_client.h"
#include <iostream>

int main(int argc, char **argv)
{
    std::string address = "127.0.0.1";
    int port = 5000;

    if (argc >= 2)
        address = argv[1];
    if (argc >= 3)
        port = std::atoi(argv[2]);

    AmikonClient client(address, port);

    if (!client.connect_to_server())
        return 1;

    // Temporary XML message (later we load from file or GUI)
    std::string xml =
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<request>\n"
    "    <command>Print</command>\n"
    "    <data>\n"
    "        <row type='name'>Mr. Joe Chase</row>\n"
    "        <row type='address'>123 Anywhere Lane</row>\n"
    "    </data>\n"
    "</request>\n";

    if (!client.send_message(xml))
        return 1;

    std::string response = client.receive_response();

    std::cout << "[CLIENT] Server Response:\n" << response << std::endl;

    return 0;
}
