#ifndef TETRIS_H
#define TETRIS_H
#include "point.h"
#include <SFML/Graphics.hpp>
class TetrisBase
{
public:
	TetrisBase(); //Constructor

	bool hasLost;
	int freezeTime = 0;
	int score = 0;
	int grid[10][22];
	int currentY;
	int c;
	bool canMove;
	int currentRotation;
	int randomInt;
	int pieceIndex;
	int nextPiece;
	bool hasHolded;
	int hasClearedLines;
	int linesToInsert = 0;

	signed int holdIndex = -1;
	signed int clearTime = -1;
	bool clearingLine[21];
	static Point AllPieces[7][4];
	static Point lineMat[2][4];
	static Point tMat[4][4];
	static Point zMat[2][4];
	static Point zRevMat[2][4];
	static Point leftLMat[4][4];
	static Point rightLMat[4][4];

	Point currentBasePiece[4];
	Point currentPiece[4];
	Point nextPieceArr[4];
	Point currentPieceSetter[4];
	bool pieceIsMoving;
	bool allowMovementDown;
	bool hitBottom;

	bool insertLines(int & lines);
	void resetSetter();
	void holdPiece();
	void hardDrop();
	void rotatePiece();
	bool checkLine(int y);
	bool isOccupied(int x, int y);
	void copyArray(Point source[], Point(&dest)[4], int size);
	bool contains(int x, int y, Point(&pieceList)[4]);
	void clearLine(int y);
	void generateNewPiece();

	void movePieceLeft();
	void movePieceRight();
	void timerTick(sf::Mutex &mutex);
	void reset();
	
};
#endif