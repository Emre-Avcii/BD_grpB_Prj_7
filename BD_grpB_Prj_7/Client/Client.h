#ifndef CLIENT_H
#define CLIENT_H
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <string>

namespace Client
{
	extern sf::TcpSocket message_socket;
	extern std::thread message_receiver;
	extern sf::IpAddress ip_address;
	extern int port;
	extern std::thread client_loop_thread;

	bool start_connecting(std::string ip, int port);
	void client_loop();
	void message_sender(std::string &_message);
}
#endif

