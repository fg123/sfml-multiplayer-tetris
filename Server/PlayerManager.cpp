#include "PlayerManager.h"

bool PlayerManager::Add(Player p)
{
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		if (Players[i].Name == p.Name)
		{
			return false;
		}
		if (Players[i].Name.isEmpty())
		{
			Players[i] = p;
			return true;
		}
	}
	return false;
}
bool PlayerManager::Remove(sf::IpAddress IP)
{
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		if (Players[i].IP == IP)
		{
			Players[i] = Player();
			return true;
		}
	}
	return false;
}
sf::Packet PlayerManager::CreateUpdatePacket()
{
	sf::Packet packet;
	packet << (int)ClientListUpdate;
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		
		packet << Players[i].Name << Players[i].IsAlive;
		
	}
	return packet;
}
sf::String PlayerManager::GetByIp(sf::IpAddress IP)
{
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		if (Players[i].IP == IP)
			return Players[i].Name;
	}
	return "Not Found";
}
int PlayerManager::CountPlayers()
{
	int count = 0;
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		if (!Players[i].Name.isEmpty())
		{
			count++;
		}
	}
	return count;
}