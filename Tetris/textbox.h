#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <SFML/Graphics.hpp>

class TextBox
{
	public:
		TextBox();
		TextBox(sf::String promptText, sf::Vector2f size, sf::Vector2f position, int fontSize, sf::Font font, void(*callback)());
		sf::String Text;
		sf::String PromptText;
		int CursorIndex;
		sf::Vector2f Size;
		sf::Vector2f Position;
		int FontSize;
		bool InFocus;
		sf::RectangleShape tbRect;
		sf::Font TextFont;
		bool active;
		mutable sf::Clock  m_cursor_timer;
		mutable sf::RectangleShape m_cursor;
		void(*Callback)();

		sf::FloatRect oldRect;

		void Draw(sf::RenderWindow &window);
		void Focus();
		void Unfocus();
		void TextBox::TextEntered(sf::Event event);
		void MouseEvent(sf::Vector2i position, sf::RenderWindow &window);
		bool Contains(sf::Vector2i pos);
		void TextBox::setCursor(int pos);
		int TextBox::getCursor();
		
};
#endif

