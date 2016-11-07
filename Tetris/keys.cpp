#include "keys.h"

Keys::Keys() {};
void Keys::PressKey(sf::Keyboard::Key d)
{
	switch (d)
	{
	case sf::Keyboard::Key::Left:
		KeyLeft = true;
		break;
	case sf::Keyboard::Key::Right:
		KeyRight = true;
		break;
	case sf::Keyboard::Key::Up:
		KeyUp = true;
		break;	
	case sf::Keyboard::Key::Down:
		KeyDown = true;
		break;
	case sf::Keyboard::Key::Space:
		KeySpace = true;
		break;
	case sf::Keyboard::Key::LShift:
		KeyShift = true;
		break;
	default:
		break;
	}
};

void Keys::ReleaseKey(sf::Keyboard::Key d)
{
	switch (d)
	{
	case sf::Keyboard::Key::Left:
		KeyLeft = false;
		break;
	case sf::Keyboard::Key::Right:
		KeyRight = false;
		break;
	case sf::Keyboard::Key::Up:
		KeyUp = false;
		break;
	case sf::Keyboard::Key::Down:
		KeyDown = false;
		break;
	case sf::Keyboard::Key::Space:
		KeySpace = false;
		break;
	case sf::Keyboard::Key::LShift:
		KeyShift = false;
		break;
	default:
		break;
	}
};