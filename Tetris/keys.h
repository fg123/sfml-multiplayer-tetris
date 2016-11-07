#ifndef KEYS_H
#define KEYS_H
#include <SFML/Graphics.hpp>

class Keys
{
public:
	bool KeyUp;
	bool KeyDown;
	bool KeyLeft;
	bool KeyRight;
	bool KeyShift;
	bool KeySpace;
	Keys();
	void PressKey(sf::Keyboard::Key d);
	void ReleaseKey(sf::Keyboard::Key d);
};
#endif