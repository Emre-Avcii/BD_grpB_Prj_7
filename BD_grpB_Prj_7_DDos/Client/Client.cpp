#include "Client.h"

namespace Client
{
	sf::TcpSocket message_socket;
	std::thread message_receiver;
	sf::IpAddress ip_address;
	int port;
	std::thread client_loop_thread;

	bool start_connecting(std::string ip, int port)
	{
		Client::ip_address = sf::IpAddress(ip);
		Client::port = port;

		if (Client::message_socket.connect(Client::ip_address, Client::port) == sf::Socket::Status::Done)
		{
			return true;
		}
		std::cout << "start_connecting() Error" << std::endl;
		return false;
	}

	void client_loop()
	{
		while (true)
		{
			sf::Packet packet;
			sf::Socket::Status status = Client::message_socket.receive(packet);

			if (status == sf::Socket::Done)
			{
				//Success
			}
			else if (status == sf::Socket::Error)
			{
				//Error
				std::cerr << "Error with responding packet" << std::endl;
			}
			else if (status == sf::Socket::Disconnected)
			{
				//Disconnect
			}
		}
	}
	void message_sender(std::string& _message)
	{
		sf::Packet packet;
		_message += '\0';
		packet.append(_message.c_str(), _message.size());
		sf::Socket::Status status = Client::message_socket.send(packet);
		if (status == sf::Socket::Done)
		{
			//Success
			const char *_msg = (char*)packet.getData();
			std::cout << "Message successfully sended: " << _msg << std::endl;
		}
		else if (status == sf::Socket::Error)
		{
			//Error
			std::cerr << "Error with sending packet" << std::endl;
		}
		else if (status == sf::Socket::Disconnected)
		{
			//Disconnect
			std::cout << "Message couldn't send, server is down: " << std::endl;
		}
	}
}

