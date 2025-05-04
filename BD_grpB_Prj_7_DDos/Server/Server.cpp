#include "Server.h"
namespace Server
{
	sf::TcpListener listener;

	Server::Socket_Struct sockets[CLIENT_COUNT_MAX];
	std::mutex sockets_mutex[CLIENT_COUNT_MAX];

	sf::IpAddress ip_address;
	int port;

	std::stack<int> unique_ids;
	std::mutex unique_ids_mutex;

	Server::Block_Struct *blocks = nullptr;

	std::thread server_loop_thread;

	std::mutex cout_mutex;

	const double tick_sleep_time = (double)1 / SERVER_TICK;
	void print_screen(std::string str)
	{
		Server::cout_mutex.lock();
		std::cout << str << std::endl;
		Server::cout_mutex.unlock();
	}
	bool start_listening(std::string ip, int port)
	{
		Server::ip_address = sf::IpAddress(ip);
		Server::port = port;

		Server::listener.setBlocking(false);

		if (Server::listener.listen(Server::port, Server::ip_address) == sf::Socket::Status::Done)
		{
			for (int i = CLIENT_COUNT_MAX - 1; i >= 0; i--)
			{
				Server::unique_ids.push(i);
			}
			return true;
		}
		Server::cout_mutex.lock();
		std::cout << "start_listening() Error" << std::endl;
		Server::cout_mutex.unlock();
		return false;
	}
	void block_loop(int index)
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		while (true)
		{
			Server::blocks[index].indexes_mutexes.lock();
			for (size_t i = 0; i < Server::blocks[index].indexes.size(); i++)
			{
				Server::socket_read(Server::blocks[index].indexes[i]);
			}
			Server::blocks[index].indexes_mutexes.unlock();

			start += std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(Server::tick_sleep_time));
			std::this_thread::sleep_until(start);
		}
	}
	void socket_read(int index)
	{
		sf::Packet packet;
		sf::Socket::Status status = Server::sockets[index].socket->receive(packet);

		Server::sockets_mutex[index].lock();


		if (status == sf::Socket::Done)
		{
			//Success
			char* message = (char*)packet.getData();
			
			std::thread work = std::thread(Server::work, packet.getDataSize() * 100);
			work.detach();
			if (packet.getDataSize() <= 100)
			{
				Server::cout_mutex.lock();
				std::cout << "Received " << index << " : " << message << std::endl;
				Server::cout_mutex.unlock();
			}

			Server::sockets_mutex[index].unlock();
		}
		else if (status == sf::Socket::Error)
		{
			//Error
			std::cerr << "Error with socket with id" << index << std::endl;
			Server::sockets_mutex[index].unlock();
		}
		else if (status == sf::Socket::Disconnected)
		{
			//Disconnect
			if (Server::sockets[index].socket != nullptr)
			{
				std::thread client_disconnecter_thread = std::thread(Server::client_disconnector, index);
				client_disconnecter_thread.detach();
			}
			Server::sockets_mutex[index].unlock();
		}
		else
		{
			Server::sockets_mutex[index].unlock();
		}
	}
	void client_accepter()
	{
		while (true)
		{

			Server::unique_ids_mutex.lock();
			if (Server::unique_ids.size() != 0)
			{
				Server::unique_ids_mutex.unlock();
				sf::TcpSocket* client = new sf::TcpSocket();
				if (Server::listener.accept(*client) == sf::Socket::Done)
				{
					client->setBlocking(false);
					Server::unique_ids_mutex.lock();
					int unique_id = Server::unique_ids.top();

					Server::unique_ids.pop();

					Server::unique_ids_mutex.unlock();
					Server::sockets[unique_id].socket = new sf::TcpSocket();
					Server::sockets[unique_id].socket = client;
					Server::sockets[unique_id].local_address = client->getLocalPort();
					Server::sockets[unique_id].remote_address = client->getRemoteAddress();

					Server::print_screen("New client connected " + std::to_string(unique_id));
					Server::blocks[unique_id % CLIENT_BLOCK_COUNT].indexes_mutexes.lock();
					Server::blocks[unique_id % CLIENT_BLOCK_COUNT].indexes.push_back(unique_id);
					Server::blocks[unique_id % CLIENT_BLOCK_COUNT].indexes_mutexes.unlock();
				}
				else
				{
					delete client;
				}
			}
			else
			{
				Server::unique_ids_mutex.unlock();
			}
			sf::sleep(sf::milliseconds(10));
		}
	}
	void client_disconnector(int index)
	{
		Server::blocks[index % CLIENT_BLOCK_COUNT].indexes_mutexes.lock();
		Server::sockets_mutex[index].lock();

		Server::print_screen("client_disconnector() " + std::to_string(index));

		Server::sockets[index].socket = nullptr;
		Server::sockets[index].local_address = 0;
		Server::sockets[index].remote_address = sf::IpAddress("0.0.0.0");

		Server::unique_ids_mutex.lock();
		Server::unique_ids.push(index);
		Server::unique_ids_mutex.unlock();

		std::vector<int>::iterator it = std::find(Server::blocks[index % CLIENT_BLOCK_COUNT].indexes.begin(), Server::blocks[index % CLIENT_BLOCK_COUNT].indexes.end(), index);
		if (it != Server::blocks[index % CLIENT_BLOCK_COUNT].indexes.end())
		{
			Server::blocks[index % CLIENT_BLOCK_COUNT].indexes.erase(it);
		}
		

		Server::sockets_mutex[index].unlock();
		Server::blocks[index % CLIENT_BLOCK_COUNT].indexes_mutexes.unlock();
	}
	void work(int times)
	{
		for (size_t i = 0; i < times; i++)
		{
			
		}
	}
}