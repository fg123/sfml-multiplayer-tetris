#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>

class Button
{
public:
	Button();
	Button(sf::String text, sf::Vector2f position, sf::Vector2f size, sf::String id, int fontsize);
	sf::Vector2f Position;
	sf::Vector2f Size;
	sf::String Text;
	sf::String Id;
	int FontSize;
	sf::RectangleShape ButtonRect;
	bool Contains(sf::Vector2i pos);
};
#endif
