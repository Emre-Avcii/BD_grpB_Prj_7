#include <fstream>
#include <regex>
#include "Server.h"

int main()
{
    std::string ip_string = "";
    std::string port_string = "";
    int port = -1;
    {
        std::ifstream file("server_settings.txt");
        if (!file.is_open())
        {
            std::cerr << "Failed to open the file." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return -1;
        }
        std::string file_string = "";
        std::getline(file, file_string);

        std::regex file_regex(R"((\d{1,3}(?:\.\d{1,3}){3}):(\d+))");
        std::smatch matches;
        if (std::regex_match(file_string, matches, file_regex))
        {
            ip_string = matches[1];
            port_string = matches[2];

            std::cout << "IP: " << ip_string << "\nPort: " << port_string << std::endl;
        }
        else
        {
            std::cerr << "Invalid format. Expected IP:PORT format.\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return -2;
        }

        port = std::stoi(port_string);
        do
        {
            Server::cout_mutex.lock();
            std::cout << "Trying to start the server at " << ip_string <<  " with port " <<  port << "." << std::endl;
            Server::cout_mutex.unlock();
        } while (Server::start_listening(ip_string, port) == false);
    }
    Server::cout_mutex.lock();
    std::cout << "The server at "<< ip_string << " with port "<< port << " have been started to listening" << std::endl;
    Server::cout_mutex.unlock();
    Server::blocks = new Server::Block_Struct[CLIENT_BLOCK_COUNT];
    for (size_t i = 0; i < CLIENT_BLOCK_COUNT; i++)
    {
        Server::blocks[i].block = new std::thread(Server::block_loop, i);
        Server::blocks[i].block->detach();
    }
    Server::server_loop_thread = std::thread(Server::client_accepter);

    Server::server_loop_thread.join();
    delete[] Server::blocks;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}