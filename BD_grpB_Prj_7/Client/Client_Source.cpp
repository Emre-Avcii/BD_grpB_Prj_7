#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <atomic>
#include <regex>
#include <chrono>
#include "Client.h"

int main()
{
    {
        std::ifstream file("client_settings.txt");
        if (!file.is_open()) 
        {
            std::cerr << "Failed to open the file." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return -1;
        }
        std::string file_string = "";
        std::getline(file, file_string);
        std::string ip_string = "";
        std::string port_string = "";
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

        int port = std::stoi(port_string);

        std::cout << "Connecting to the server " << std::endl;
        do
        {
            std::cout << "Trying to connect the server at " << ip_string << " with port " << port << std::endl;
        } 
        while (Client::start_connecting(ip_string, port) == false);
        std::cout << "The connection at " << ip_string << " with port " << port << " have been started" << std::endl;
    }
    Client::client_loop_thread = std::thread(Client::client_loop);
    while (true)
    {
        std::string message;
        std::getline(std::cin, message);
        Client::message_sender(message);
    }
    Client::client_loop_thread.join();
 
    return 0;
}