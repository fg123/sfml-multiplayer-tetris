#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <conio.h>
#include "PlayerManager.h"

using namespace std;

void write(string msg);
void workerThread();
sf::Mutex mutex;
bool exitCond = false;
PlayerManager playerManager;
void updateClients(sf::UdpSocket &socket);
string task = "";

int main()
{
	cout << "Welcome to Tetris Server 1.0" << endl <<
		"Shutdown server gracefully with quit command!" << endl;
	std::thread worker(workerThread);
	cout << "> ";
	while (true)
	{		
		if (_kbhit())
		{
			string command;
			cin >> command;
			
			if (command == "quit")
			{
				mutex.lock();
				exitCond = true;
				mutex.unlock();
				worker.join();
				return 0;
			}
			else if (command == "players")
			{
				mutex.lock();
				cout << "List of Players:" << endl;
				for (int i = 0; i < MAXPLAYERS; i++)
				{
					if (!playerManager.Players[i].Name.isEmpty())
					{
						cout << playerManager.Players[i].Name.toAnsiString() << "|" << playerManager.Players[i].IP.toString() << endl;
					}
				}
				cout << "> ";
				mutex.unlock();
			}
			else if (command == "update")
			{
				mutex.lock();
				task = "update";
				mutex.unlock();
			}
			else if (command == "start")
			{
				mutex.lock();
				cout << "Starting Game" << endl;
				cout << "> ";
				task = "start";
				mutex.unlock();
			}
			else if (command == "sendline")
			{
				mutex.lock();
				cout << "Sending Line" << endl;
				cout << "> ";
				task = "sendline";
				mutex.unlock();
			}
			else
			{
				mutex.lock();
				cout << "> ";
				mutex.unlock();
			}
		}
	}
}
void workerThread()
{
	sf::UdpSocket socket;
	socket.bind(14243);
	socket.setBlocking(false);
	sf::Clock timer;
	write("Server Thread Started");
	while (true)
	{
		mutex.lock();
		if (exitCond)
		{
			return;
		}
		
		mutex.unlock();

		if (timer.getElapsedTime().asSeconds() > 1)
		{
			sf::Packet broadcastPack;
			broadcastPack << (int)ServerBroadcast;
			
			sf::IpAddress broadcast = sf::IpAddress::Broadcast;
			
			//write("Broadcasting Packet To " + broadcast.toString());
			socket.send(broadcastPack, broadcast, 14242);
			timer.restart();
		}
		sf::Packet packet;
		sf::IpAddress sender;
		unsigned short port;
		socket.receive(packet, sender, port);
		int response;
		if (packet >> response)
		{
			if (response == (int)RequestJoin)
			{
				mutex.lock();
				write("Request incoming from " + sender.toString());
				Player p;
				sf::String username;
				packet >> username;
				p.Name = username;
				p.IP = sender;
				p.IsAlive = true;
				if (playerManager.Add(p))
				{
					write("Player add success!");
					sf::Packet responsePacket;
					responsePacket << (int)JoinSuccess;
					socket.send(responsePacket, sender, 14242);
					updateClients(socket);
				}
				else
				{
					write("Player add fail!");
					sf::Packet responsePacket;
					responsePacket << (int)JoinFail;
					socket.send(responsePacket, sender, 14242);
				}
				mutex.unlock();
			}
			else if (response == (int)ServerBroadcast)
			{
				mutex.lock();
				write("Connection from " + sender.toString());
				mutex.unlock();
			}
			else if (response == (int)ClearLine)
			{
				mutex.lock();
				
				int lines;
				packet >> lines;
				int sendTo = rand() % playerManager.CountPlayers();
				if (playerManager.CountPlayers() != 1)
				{
					while (playerManager.Players[sendTo].IP == sender)
					{
						sendTo = rand() % playerManager.CountPlayers();
					}
				}
				write("Lines sent from " + playerManager.GetByIp(sender) + " to " + playerManager.Players[sendTo].Name);
				sf::Packet sendLine;
				sendLine << (int)ClearLine << lines;
				socket.send(sendLine, playerManager.Players[sendTo].IP, 14242);
				mutex.unlock();
			}
		}
		mutex.lock();
		if (task == "update")
		{
			task = "";
			updateClients(socket);
		}
		else if (task == "start")
		{
			task = "";
			sf::Packet responsePacket;
			responsePacket << (int)StartGame;
			for (int i = 0; i <= MAXPLAYERS; i++)
			{
				if (!playerManager.Players[i].Name.isEmpty())
				{
					socket.send(responsePacket, playerManager.Players[i].IP, 14242);
				}
			}
		}
		else if (task == "sendline")
		{
			task = "";
			sf::Packet responsePacket;
			responsePacket << (int)ClearLine << 2;
			for (int i = 0; i <= MAXPLAYERS; i++)
			{
				if (!playerManager.Players[i].Name.isEmpty())
				{
					socket.send(responsePacket, playerManager.Players[i].IP, 14242);
				}
			}
		}
		mutex.unlock();
	}
}
void updateClients(sf::UdpSocket &socket)
{
	sf::Packet p = playerManager.CreateUpdatePacket();
	for (int i = 0; i <= MAXPLAYERS; i++)
	{
		if (!playerManager.Players[i].Name.isEmpty())
		{
			socket.send(p, playerManager.Players[i].IP, 14242);
		}
	}
}
void write(string msg)
{
	cout << "\r" << msg << endl << "> ";
}