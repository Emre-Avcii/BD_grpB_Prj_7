#ifndef CLIENT_H
#define CLIENT_H
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>

namespace Client
{
	extern std::vector<sf::TcpSocket*> message_sockets;
	extern sf::IpAddress ip_address;
	extern int port;

	extern int DDoS_thread_count;
	extern int DDoS_message_count;
	extern size_t DDoS_message_length;
	extern sf::Packet DDoS_message;
	extern std::thread* DDoS_threads;
	extern std::atomic<bool>* DDoS_threads_atomic_flag;

	extern sf::Time DDoS_waiting_time;
	extern std::atomic<size_t> DDoS_total_packet_sended;

	bool start_connecting(int index);
	void client_loop(int index);
	void message_sender(int index);
}
#endif


