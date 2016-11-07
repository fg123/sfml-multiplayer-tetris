#include "textbox.h"
#include <SFML/Graphics.hpp>
#define BLINK_PERIOD 0.5f
TextBox::TextBox()
{
	active = false;
}
TextBox::TextBox(sf::String promptText, sf::Vector2f size, sf::Vector2f position, int fontSize, sf::Font font, void(*callback)() = {})
{
	active = true;
	PromptText = promptText;
	Size = size;
	Position = position;
	FontSize = fontSize;
	TextFont = font;
	
	tbRect = sf::RectangleShape(Size);
	tbRect.setFillColor(sf::Color(0, 0, 0, 120));
	tbRect.setOutlineColor(sf::Color(0, 0, 0, 120));
	tbRect.setPosition(Position.x - (Size.x / 2.0f),
		Position.y - (Size.y / 2.0f));
	m_cursor.setPosition(0, 0);
	m_cursor.setSize(sf::Vector2f(1.f, FontSize));
	m_cursor.setFillColor(sf::Color::White);
	setCursor(0);
	Callback = callback;
}

void TextBox::Draw(sf::RenderWindow &window)
{
	window.draw(tbRect);
	sf::Text TextObj;
	TextObj.setFont(TextFont);
	TextObj.setCharacterSize(FontSize);
	if (Text.isEmpty())
	{
		TextObj.setString(PromptText);
		TextObj.setFillColor(sf::Color(255, 255, 255, 120));
		TextObj.setOutlineColor(sf::Color(255, 255, 255, 120));
	}
	else
	{
		TextObj.setString(Text);
		TextObj.setFillColor(sf::Color::White);
		TextObj.setOutlineColor(sf::Color::White);
	}
	
	sf::FloatRect textRect = TextObj.getLocalBounds();
	textRect.height = FontSize;
	TextObj.setOrigin(0, FontSize + 5);
	TextObj.setPosition(sf::Vector2f(10 + (Position.x - Size.x / 2.0f), Position.y + FontSize / 2.0f));
	window.draw(TextObj);

	// Show cursor if focused
	if (InFocus)
	{
		// Make it blink
		float timer = m_cursor_timer.getElapsedTime().asSeconds();
		if (timer >= BLINK_PERIOD)
			m_cursor_timer.restart();

		// Updating in the drawing method, deal with it
		sf::Color color = sf::Color::White;
		color.a = 255 - (255 * timer / BLINK_PERIOD);
		m_cursor.setFillColor(color);
		m_cursor.setPosition(TextObj.findCharacterPos(CursorIndex).x, Position.y - FontSize / 2.0f);
		window.draw(m_cursor);
	}
}
void TextBox::Focus()
{
	InFocus = true;
}
void TextBox::Unfocus()
{
	InFocus = false;
}
void TextBox::MouseEvent(sf::Vector2i position, sf::RenderWindow &window)
{
	if (Contains(sf::Mouse().getPosition(window)))
	{
		tbRect.setFillColor(sf::Color(0, 0, 0, 200));
		tbRect.setOutlineColor(sf::Color(0, 0, 0, 200));
		if (sf::Mouse().isButtonPressed(sf::Mouse::Button::Left))
		{
			Focus();
		}
	}
	else
	{
		tbRect.setFillColor(sf::Color(0, 0, 0, 120));
		tbRect.setOutlineColor(sf::Color(0, 0, 0, 120));
	}
}
bool TextBox::Contains(sf::Vector2i pos)
{
	if (pos.x >= tbRect.getPosition().x &&
		pos.x <= tbRect.getPosition().x + tbRect.getSize().x &&
		pos.y >= tbRect.getPosition().y &&
		pos.y <= tbRect.getPosition().y + tbRect.getSize().y) return true;
	else return false;
}
void TextBox::setCursor(int pos)
{
	if (pos >= 0 && pos <= Text.getSize())
	{
		CursorIndex = pos;
		m_cursor_timer.restart();
	}
}
int TextBox::getCursor()
{
	return CursorIndex;
}
void TextBox::TextEntered(sf::Event event)
{
	if (InFocus)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Left:
			setCursor(CursorIndex - 1);
			break;

		case sf::Keyboard::Right:
			setCursor(CursorIndex + 1);
			break;

		case sf::Keyboard::BackSpace:
			// Erase character before cursor
			if (CursorIndex > 0)
			{
				Text.erase(CursorIndex - 1);

				setCursor(CursorIndex - 1);
			}
			
			break;
		case sf::Keyboard::LShift:
			break;
		case sf::Keyboard::RShift:
			break;
		case sf::Keyboard::Space:
			Text.insert(Text.getSize(), ' ');
			setCursor(CursorIndex + 1);
			break;
		case sf::Keyboard::Delete:
			// Erase character after cursor
			if (CursorIndex < Text.getSize())
			{
				Text.erase(CursorIndex);

				setCursor(CursorIndex);
			}
			break;

		case sf::Keyboard::Home:
			setCursor(0);
			break;

		case sf::Keyboard::End:
			setCursor(Text.getSize());
			break;

		case sf::Keyboard::Return:
			(*Callback)();
			break;

		default:
			for (int i = sf::Keyboard::Key::A; i <= sf::Keyboard::Key::Z; i++)
			{
				if (event.key.code == i)
				{

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
					{
						Text.insert(CursorIndex, static_cast<char>(65 + i));
					}
					else
					{
						Text.insert(CursorIndex, static_cast<char>(65 + 32 + i));
					}
					
					setCursor(CursorIndex + 1);
				}
			}
			break;
		}
		
	}
}