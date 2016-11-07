#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <SFML/Network.hpp>
#define MAXPLAYERS 6
enum PacketTypes { ServerBroadcast, RequestJoin, JoinSuccess, JoinFail, ClientListUpdate, ClientLeave, StartGame, ClearLine, GameEnd, ServerShutdown };

struct Player
{
	sf::String Name;
	sf::IpAddress IP;
	bool IsAlive;
};

class PlayerManager
{
	public:
		PlayerManager(){};
		Player Players[MAXPLAYERS];
		bool Add(Player p);
		bool Remove(sf::IpAddress IP);
		sf::String GetByIp(sf::IpAddress IP);
		sf::Packet CreateUpdatePacket();
		int CountPlayers();

};

#endif