#include <Windows.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "keys.h"
#include "point.h"
#include "tetris.h"
#include <stdint.h>
#include <sstream>
#include "button.h"
#include "textbox.h"
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <thread>

#define WIN_WIDTH 600
#define WIN_HEIGHT 800
#define WIN_TITLE "Tetris - By Felix"
#define DOWN_TIMER 500 
#define REGULAR_TIMER 200 //in milliseconds
#define BUTTONS_PER_SCREEN 5
#define TEXTBOX_PER_SCREEN 2
#define MAXPLAYERS 6
sf::Clock gameClock;

enum Screens { Main, Lobby, Game, PostGame, PromptName };
enum PacketTypes {ServerBroadcast, RequestJoin, JoinSuccess, JoinFail, ClientListUpdate, ClientLeave, StartGame, ClearLine, GameEnd, ServerShutdown };

Screens CurrentScreen = PromptName;
Button Buttons[5][BUTTONS_PER_SCREEN];
TextBox TextBoxes[5][BUTTONS_PER_SCREEN];
sf::Text ButtonText;
sf::String Username;

bool isSinglePlayer = false;
Keys KeyboardState;
Keys OldKeyboardState;
std::map<sf::String, sf::Text> GameText;

void update(sf::RenderWindow &window, Keys currentState, Keys oldState);
void draw(sf::RenderWindow &window);
void drawMain(sf::RenderWindow &window);
void drawLobby(sf::RenderWindow &window);
void drawGame(sf::RenderWindow &window);
void drawPostGame(sf::RenderWindow &window);
void drawPromptName(sf::RenderWindow &window);
void keyPressTetris(sf::Keyboard::Key pressed, sf::RenderWindow &window, Keys currentState);
void updateButtons(sf::RenderWindow &window);
void buttonClick(sf::String id);
void workerThread();
void switchScreen(Screens a);

int timer;

sf::Sprite gridSprite, ghostSprite;
sf::Sprite backgroundSprite;
sf::Sprite colorBlock[8];

sf::Mutex mutex;
bool quit = false;
sf::IpAddress serverIP;
bool hasAnotherServer = false;
bool oldMouseClick;
bool joiningRequest;
sf::Clock screenSwitchTimer;

TetrisBase Tetris;
sf::String playerList[MAXPLAYERS];
bool playerAlive[MAXPLAYERS];

void CallNameClick()
{
	buttonClick("StartGame");
}
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Initial Setup for Sprites
	sf::Texture gridTexture, colors, backgroundTexture;

	gridTexture.loadFromFile("Resources/grid.png");
	colors.loadFromFile("Resources/colors.png");
	backgroundTexture.loadFromFile("Resources/background.png");

	backgroundSprite.setTexture(backgroundTexture);
	gridSprite.setTexture(gridTexture);
	ghostSprite.setTexture(colors);
	ghostSprite.setTextureRect(sf::IntRect(240, 0, 30, 30));
	for (int i = 0; i < 8; i++)
	{
		colorBlock[i].setTexture(colors);
		colorBlock[i].setTextureRect(sf::IntRect(i * 30, 0, 30, 30));
	}

	#pragma region Initial Setup for Text
	sf::Font brainFont, bebasFont, myriadFont;
	brainFont.loadFromFile("Resources/brainflower.ttf");
	bebasFont.loadFromFile("Resources/bebas.otf");
	myriadFont.loadFromFile("Resources/myriad.otf");

	const int totalText = 14; 
	sf::Text textList[totalText];

	textList[0].setString("Tetris");
	textList[0].setCharacterSize(58);

	textList[1].setString("Next Piece:");
	textList[1].setCharacterSize(32);

	textList[2].setString("Hold:");
	textList[2].setCharacterSize(32);

	textList[3].setString("Single Player");
	textList[3].setCharacterSize(64);

	textList[4].setString("Join Game");
	textList[4].setCharacterSize(64);

	textList[5].setString("Create Game");
	textList[5].setCharacterSize(64);

	textList[6].setString("Lobby");
	textList[6].setCharacterSize(64);

	textList[7].setString("Start Game");
	textList[7].setCharacterSize(64);

	textList[8].setString("PLAYERNAME");
	textList[8].setCharacterSize(64);

	textList[9].setString("Game Over!");
	textList[9].setCharacterSize(64);

	textList[10].setString("SCORES");
	textList[10].setCharacterSize(32);

	textList[11].setString("TETRIS");
	textList[11].setCharacterSize(150);

	textList[12].setString("Created by Felix Guo");
	textList[12].setCharacterSize(15);

	for (int i = 0; i < totalText; i++)
	{
		textList[i].setFont(myriadFont);
		textList[i].setFillColor(sf::Color::White);
		textList[i].setOutlineColor(sf::Color::White);
	}
	GameText["TetrisTitleInGame"] = textList[0];
	GameText["NextPiece"] = textList[1];
	GameText["HoldPiece"] = textList[2];
	GameText["SPBtnText"] = textList[3];
	GameText["JoinBtnText"] = textList[4];
	GameText["CreateBtnText"] = textList[5];
	GameText["LobbyTitle"] = textList[6];
	GameText["StartBtnText"] = textList[7];
	GameText["PlayerNameText"] = textList[8];
	GameText["GameOverText"] = textList[9];
	GameText["GameOverScoreText"] = textList[10];
	GameText["TitleText"] = textList[11];
	GameText["CreatedBy"] = textList[12];
	#pragma endregion

	#pragma region Initial Setup for Buttons
	ButtonText.setFont(bebasFont);
	ButtonText.setOutlineColor(sf::Color::White);
	ButtonText.setFillColor(sf::Color::White);

	Button playButton("Single Player", sf::Vector2f(WIN_WIDTH / 2.0f, 450), sf::Vector2f(300, 80), "StartSingle", 48);	
	Button joinServer("Join Room", sf::Vector2f(WIN_WIDTH / 2.0f, 600), sf::Vector2f(300, 80), "JoinServer", 48);
	Button playAgainButton("Play Again", sf::Vector2f(WIN_WIDTH / 2.0f, 450), sf::Vector2f(300, 80), "StartSingle", 48);
	Button backMenuButton("Back to Menu", sf::Vector2f(WIN_WIDTH / 2.0f, 600), sf::Vector2f(300, 80), "ReturnMenu", 48);
	Button startButton("Start", sf::Vector2f(WIN_WIDTH / 2.0f, 600), sf::Vector2f(300, 80), "StartGame", 48);

	Buttons[Main][0] = playButton;
	Buttons[Main][1] = joinServer;

	Buttons[PostGame][0] = playAgainButton;
	Buttons[PostGame][1] = backMenuButton;

	Buttons[PromptName][0] = startButton;

	TextBox nameEnter("Please enter your name", sf::Vector2f(400, 50), sf::Vector2f(WIN_WIDTH / 2.0f, 500), 32, myriadFont, CallNameClick);
	TextBoxes[PromptName][0] = nameEnter;
	#pragma endregion

	// Initial Setup for SFML
	sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), WIN_TITLE, sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60);
	sf::Image icon;
	icon.loadFromFile("Resources/icon.png");
	window.setIcon(128, 128, icon.getPixelsPtr());

	std::thread worker(workerThread);

	//Main Window Loop
	while (window.isOpen())
	{
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				mutex.lock();
				quit = true;
				mutex.unlock();
				worker.join();
				window.close();
				break;
			case sf::Event::KeyPressed:
				KeyboardState.PressKey(event.key.code);
				if (CurrentScreen == Game)
				{
					if (!Tetris.hasLost)
					{
						keyPressTetris(event.key.code, window, KeyboardState);
					}
				}
				for (int i = 0; i < TEXTBOX_PER_SCREEN; i++)
				{
					if (TextBoxes[CurrentScreen][i].active)
					{
						TextBoxes[CurrentScreen][i].TextEntered(event);
					}
				}
				break;
			case sf::Event::KeyReleased:
				KeyboardState.ReleaseKey(event.key.code);
			case sf::Event::TextEntered:
				
				break;
			default:
				break;

			}
		}
		update(window, KeyboardState, OldKeyboardState);
		if (CurrentScreen != Game) //game drawing is handled in update
		{
			draw(window);
		}
		OldKeyboardState = KeyboardState;
	}

	return 0;
}
sf::Text CreateCenteredText(sf::Text text, sf::Vector2f pos)
{
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top);
	text.setPosition(pos);
	return text;
}
void drawMain(sf::RenderWindow &window)
{
	window.draw(CreateCenteredText(GameText["TitleText"], sf::Vector2f(WIN_WIDTH / 2.0f, 150)));
}
void drawLobby(sf::RenderWindow &window)
{
	window.draw(CreateCenteredText(GameText["LobbyTitle"], sf::Vector2f(WIN_WIDTH / 2.0f, 150)));
	for (int i = 0; i < MAXPLAYERS; i++)
	{
		if (playerList[i].isEmpty())
		{
			GameText["PlayerNameText"].setString("Open");
			GameText["PlayerNameText"].setFillColor(sf::Color(255, 255, 255, 120));
		}
		else
		{
			GameText["PlayerNameText"].setString(playerList[i]);
			GameText["PlayerNameText"].setFillColor(sf::Color(255, 255, 255, 120));
		}
		if (i < 3)
		{
			window.draw(CreateCenteredText(GameText["PlayerNameText"],
				sf::Vector2f(WIN_WIDTH / 3.0f, 250 + (i * 75))));
		}
		else
		{
			window.draw(CreateCenteredText(GameText["PlayerNameText"],
				sf::Vector2f(2 * WIN_WIDTH / 3.0f, 250 + ((i - 3) * 75))));
		}
	}
		
}
void drawPostGame(sf::RenderWindow &window)
{
	window.draw(CreateCenteredText(GameText["GameOverText"], sf::Vector2f(WIN_WIDTH / 2.0f, 150)));

	sf::Text text = GameText["GameOverScoreText"];
	text.setString("Your score is:");
	window.draw(CreateCenteredText(text, sf::Vector2f(WIN_WIDTH / 2.0f, 280)));

	char score[20];
	sprintf_s(score, "%d", Tetris.score);
	text.setString(score);
	window.draw(CreateCenteredText(text, sf::Vector2f(WIN_WIDTH / 2.0f, 330)));
}
void drawPromptName(sf::RenderWindow &window)
{
	
	window.draw(CreateCenteredText(GameText["TitleText"], sf::Vector2f(WIN_WIDTH / 2.0f, 150)));
	
}
void drawGame(sf::RenderWindow &window)
{
	if (Tetris.hasLost)
	{
		CurrentScreen = PostGame;
	}
	std::ostringstream ss;
	ss << "Tetris: " << Tetris.score;
	GameText["TetrisTitleInGame"].setString(ss.str());
	window.draw(GameText["TetrisTitleInGame"], sf::Transform().translate(10, -7));
	window.draw(gridSprite, sf::Transform().translate(10, 60));
	for (int y = 2; y < 22; y++)
	{
		bool isClearing = Tetris.clearingLine[y];
		for (int x = 0; x < 10; x++)
		{
			sf::Transform t;
			t.translate(10 + 2 + (32 * x), 2 + (32 * (y - 2) + 60));
			if (isClearing && Tetris.clearTime != 0)
			{
				window.draw(ghostSprite, t);
			}
			else
			{
				if (Tetris.contains(x, y, Tetris.currentPiece))
				{
					window.draw(colorBlock[Tetris.pieceIndex], t);
				}
				else if (Tetris.grid[x][y] != 0)
				{
					window.draw(colorBlock[Tetris.grid[x][y] - 1], t);
				}
				else if (Tetris.contains(x, y, Tetris.currentPieceSetter))
				{
					window.draw(ghostSprite, t);
				}
			}
		}
		sf::Transform t;
		t.translate(352, 20);
		if (y == 2) window.draw(GameText["NextPiece"], t);
		if (y == 3 || y == 4)
		{
			for (int x = 3; x < 7; x++)
			{
				sf::Transform t1;
				t1.translate(352 + (32 * (x - 3)), 2 + (32 * (y - 3) + 65));
				if (Tetris.contains(x, y - 3, Tetris.nextPieceArr))
				{
					window.draw(colorBlock[Tetris.nextPiece], t1);
				}
			}
		}
		t.translate(0, 130);
		if (y == 5) window.draw(GameText["HoldPiece"], t);
		if (y == 6 || y == 7)
		{
			for (int x = 3; x < 7; x++)
			{
				sf::Transform t1;
				t1.translate(352 + (32 * (x - 3)), 2 + (32 * (y - 3) + 100));
				if (Tetris.contains(x, y - 6, Tetris.AllPieces[Tetris.holdIndex]))
				{
					window.draw(colorBlock[Tetris.holdIndex], t1);
				}
			}
		}
	}
	
}
void drawButtons(sf::RenderWindow &window)
{
	for (int i = 0; i < BUTTONS_PER_SCREEN; i++)
	{
		if (Buttons[CurrentScreen][i].Id != "")
		{
			if (Buttons[CurrentScreen][i].Id == "StartServer" && hasAnotherServer) continue;
			if (Buttons[CurrentScreen][i].Id == "JoinServer" && !hasAnotherServer) continue;

			window.draw(Buttons[CurrentScreen][i].ButtonRect);
			
			ButtonText.setString(Buttons[CurrentScreen][i].Text);
			ButtonText.setCharacterSize(Buttons[CurrentScreen][i].FontSize);
			sf::FloatRect textRect = ButtonText.getLocalBounds();
			ButtonText.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);
			ButtonText.setPosition(Buttons[CurrentScreen][i].Position);
			window.draw(ButtonText);
		}
	}
}
void draw(sf::RenderWindow &window)
{
	window.clear(sf::Color::Black);
	window.draw(backgroundSprite);
	sf::Text text = GameText["CreatedBy"];
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height);
	text.setPosition(sf::Vector2f(WIN_WIDTH / 2.0f, WIN_HEIGHT - 4));

	window.draw(text);
	switch (CurrentScreen)
	{
		case Main:
			drawMain(window); 
			break;
		case Lobby:
			drawLobby(window);
			break;
		case Game:
			drawGame(window);
			break;
		case PostGame:
			drawPostGame(window);
			break;
		case PromptName:
			drawPromptName(window);
			break;
		default: break;
	}
	drawButtons(window);
	for (int i = 0; i < TEXTBOX_PER_SCREEN; i++)
	{
		if (TextBoxes[CurrentScreen][i].active)
		{
			TextBoxes[CurrentScreen][i].Draw(window);
		}
	}
	window.display();
}
void keyPressTetris(sf::Keyboard::Key pressed, sf::RenderWindow &window, Keys currentState)
{
	if (pressed == sf::Keyboard::Key::Left)
	{
		Tetris.movePieceLeft();
		draw(window);
	}
	else if (pressed == sf::Keyboard::Key::Right)
	{
		Tetris.movePieceRight();
		draw(window);
	}
	if (pressed == sf::Keyboard::Key::Up)
	{
		Tetris.rotatePiece();
		draw(window);
	}
	if (pressed == sf::Keyboard::Key::Down)
	{
		timer = REGULAR_TIMER + 1;
		Tetris.resetSetter();
	}
	if (pressed == sf::Keyboard::Key::LShift)
	{
		Tetris.holdPiece();
		draw(window);
	}
	if (pressed == sf::Keyboard::Key::Space)
	{
		Tetris.hardDrop();
		draw(window);
	}
}
void update(sf::RenderWindow &window, Keys currentState, Keys oldState)
{
	updateButtons(window);
	oldMouseClick = sf::Mouse().isButtonPressed(sf::Mouse::Button::Left);
	for (int i = 0; i < TEXTBOX_PER_SCREEN; i++)
	{
		if (TextBoxes[CurrentScreen][i].active)
		{
			TextBoxes[CurrentScreen][i].MouseEvent(sf::Mouse::getPosition(), window);
		}
	}
	if (CurrentScreen == Game)
	{
		if (!Tetris.hasLost)
		{
			int timerInterval = REGULAR_TIMER;
			
			if (timer > timerInterval)
			{

				Tetris.timerTick(mutex);
				timer = 0;
				draw(window);
			}
	
			sf::Time elapsedTime = gameClock.restart();
			timer += elapsedTime.asMilliseconds();
		}
	}
}
void updateButtons(sf::RenderWindow &window)
{
	for (int i = 0; i < BUTTONS_PER_SCREEN; i++)
	{
		if (Buttons[CurrentScreen][i].Id != "")
		{
			if (Buttons[CurrentScreen][i].Contains(sf::Mouse().getPosition(window)))
			{
				Buttons[CurrentScreen][i].ButtonRect.setFillColor(sf::Color(0, 0, 0, 200));
				Buttons[CurrentScreen][i].ButtonRect.setOutlineColor(sf::Color(0, 0, 0, 200));
				if (sf::Mouse().isButtonPressed(sf::Mouse::Button::Left) && !oldMouseClick
					&& screenSwitchTimer.getElapsedTime().asMilliseconds() > 500 && window.hasFocus())
				{
					buttonClick(Buttons[CurrentScreen][i].Id);
				}
			}
			else
			{
				Buttons[CurrentScreen][i].ButtonRect.setFillColor(sf::Color(0, 0, 0, 120));
				Buttons[CurrentScreen][i].ButtonRect.setOutlineColor(sf::Color(0, 0, 0, 120));
			}
		}
	}
	
}
void buttonClick(sf::String id)
{
	if (id == "StartSingle")
	{
		screenSwitchTimer.restart();
		switchScreen(Game);
		isSinglePlayer = true;
		Tetris.reset();
	}
	else if (id == "ReturnMenu")
	{
		switchScreen(Main);
	}
	else if (id == "StartGame")
	{
		if (!TextBoxes[PromptName][0].Text.isEmpty())
		{
			Username = TextBoxes[PromptName][0].Text;
			switchScreen(Main);
		}
	}
	else if (id == "JoinServer")
	{
		if (hasAnotherServer)
		{
			joiningRequest = true;
		}
	}
}
void switchScreen(Screens a)
{
	screenSwitchTimer.restart();
	CurrentScreen = a;
}
void workerThread()
{
	sf::UdpSocket socket;
	socket.bind(14242);
	socket.setBlocking(false);
	while (true)
	{
		sf::Packet packet;
		sf::IpAddress sender;
		unsigned short port;
		
		int response = -1;
		/*if (packet.getDataSize() > 0)
		{
			mutex.lock();
		}*/
		if (socket.receive(packet, sender, port) == sf::Socket::Status::Done)
		{
			if (packet >> response)
			{
				if (response == (int)JoinFail)
				{
					joiningRequest = false;
				}
				else if (response == (int)JoinSuccess)
				{
					mutex.lock();
					switchScreen(Lobby);
					mutex.unlock();
					joiningRequest = false;
				}
				else if (response == (int)StartGame)
				{
					mutex.lock();
					screenSwitchTimer.restart();
					switchScreen(Game);
					Tetris.reset();
					isSinglePlayer = false;
					mutex.unlock();
				}
				else if (response == (int)ClearLine)
				{
					mutex.lock();
					int lines;
					packet >> lines;
					Tetris.linesToInsert = lines;
					mutex.unlock();
				}
				else if (response == (int)ServerBroadcast && !hasAnotherServer)
				{
					mutex.lock();
					hasAnotherServer = true;
					serverIP = sender;
					mutex.unlock();
				}
				else if (response == (int)ClientListUpdate)
				{
					mutex.lock();
					for (int i = 0; i < MAXPLAYERS; i++)
					{
						packet >> playerList[i] >> playerAlive[i];
					}
					mutex.unlock();
				}
			}
		}
		mutex.lock();
		if (joiningRequest)
		{
			sf::Packet packet;
			packet << (int)RequestJoin << Username;
			socket.send(packet, serverIP, 14243);
			joiningRequest = false;
		}
		if (Tetris.hasClearedLines != 0 && !isSinglePlayer)
		{
			sf::Packet packet;
			packet << (int)ClearLine << Tetris.hasClearedLines;
			socket.send(packet, serverIP, 14243);
			Tetris.hasClearedLines = 0;
		}
		if (quit)
		{
			return;
		}
		mutex.unlock();
	}
}