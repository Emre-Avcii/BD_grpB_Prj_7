#ifndef SERVER_H
#define SERVER_H

#include "SFML/Network.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <stack>
#include <mutex>
#include <chrono>

namespace Server
{
#define CLIENT_COUNT_MAX 4096
#define CLIENT_BLOCK_COUNT 8
#define SERVER_TICK 16

	struct Socket_Struct
	{
		sf::TcpSocket *socket = nullptr;
		unsigned short local_address = 0;
		sf::IpAddress remote_address = sf::IpAddress("0.0.0.0");
	};
	struct Block_Struct
	{
		std::thread *block = nullptr;
		std::vector<int> indexes;
		std::mutex indexes_mutexes;
	};

	extern sf::TcpListener listener;

	extern Server::Socket_Struct sockets[CLIENT_COUNT_MAX];
	extern std::mutex sockets_mutex[CLIENT_COUNT_MAX];

	extern sf::IpAddress ip_address;
	extern int port;

	extern std::stack<int> unique_ids;
	extern std::mutex unique_ids_mutex;

	extern Server::Block_Struct *blocks;

	extern std::thread server_loop_thread;

	extern std::mutex cout_mutex;

	extern const double tick_sleep_time;

	void print_screen(std::string str);
	bool start_listening(std::string ip, int port);
	void block_loop(int index);
	void socket_read(int index);
	void client_accepter();
	void client_disconnector(int index);
	void work(int times);
}
#endif

