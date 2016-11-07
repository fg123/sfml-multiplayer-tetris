#include "button.h"
#include <SFML/Graphics.hpp>

Button::Button(sf::String text, sf::Vector2f position, sf::Vector2f size, sf::String id, int fontsize)
{
	Text = text;
	Position = position;
	Size = size;
	Id = id;
	FontSize = fontsize;
	ButtonRect = sf::RectangleShape(Size);
	ButtonRect.setFillColor(sf::Color(0, 0, 0, 120));
	ButtonRect.setOutlineColor(sf::Color(0, 0, 0, 120));
	ButtonRect.setPosition(Position.x - (Size.x / 2.0f),
		Position.y - (Size.y / 2.0f));
}
Button::Button()
{
	Text = "";
	Position = sf::Vector2f(0,0);
	Size = sf::Vector2f(0, 0);
	Id = "";
	FontSize = 0;
}
bool Button::Contains(sf::Vector2i pos)
{
	if (pos.x >= ButtonRect.getPosition().x &&
		pos.x <= ButtonRect.getPosition().x + ButtonRect.getSize().x &&
		pos.y >= ButtonRect.getPosition().y &&
		pos.y <= ButtonRect.getPosition().y + ButtonRect.getSize().y) return true;
	else return false;
}