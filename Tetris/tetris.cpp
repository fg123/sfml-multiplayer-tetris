#include "tetris.h"
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <sstream>

Point TetrisBase::AllPieces[7][4] =
{
	{ Point(3, 1), Point(4, 1), Point(5, 1), Point(6, 1) }, //line piece
	{ Point(3, 1), Point(4, 1), Point(5, 1), Point(4, 0) }, //t block
	{ Point(3, 1), Point(4, 1), Point(5, 1), Point(3, 0) }, // /__
	{ Point(3, 1), Point(4, 1), Point(5, 1), Point(5, 0) }, // __/
	{ Point(3, 1), Point(4, 1), Point(4, 0), Point(5, 0) }, // reverse z
	{ Point(5, 1), Point(6, 1), Point(4, 0), Point(5, 0) }, //z block
	{ Point(4, 0), Point(4, 1), Point(5, 1), Point(5, 0) } // square
};
Point TetrisBase::lineMat[2][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(1, -1), Point(0, 0), Point(-1, 1), Point(-2, 2) }
};
Point TetrisBase::tMat[4][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(1, 1), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(0, -1), Point(0, -1), Point(0, -1), Point(0, 1) },
	{ Point(0, 0), Point(0, 0), Point(-1, 1), Point(0, 0) }
};
Point TetrisBase::zMat[2][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(0, 0), Point(0, -1), Point(2, -1), Point(0, 0) }
};
Point TetrisBase::zRevMat[2][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(0, -1), Point(0, 0), Point(0, 0), Point(-2, -1) }
};
Point TetrisBase::leftLMat[4][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(0, 0), Point(-1, -2), Point(-1, -2), Point(0, 0) },
	{ Point(0, -1), Point(0, -1), Point(0, -1), Point(2, 1) },
	{ Point(0, 0), Point(0, 0), Point(-1, -2), Point(1, 0) }
};
Point TetrisBase::rightLMat[4][4] =
{
	{ Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0) },
	{ Point(1, -2), Point(0, 0), Point(0, 0), Point(-1, 0) },
	{ Point(0, -1), Point(0, -1), Point(0, -1), Point(-2, 1) },
	{ Point(1, -2), Point(1, -2), Point(0, 0), Point(0, 0) }
};

TetrisBase::TetrisBase()
{
	
}
void TetrisBase::reset()
{
	srand(time(NULL));
	memset(grid, 0, sizeof(grid[0][0]) * 10 * 22);
	randomInt = rand() % 7;
	nextPiece = rand() % 7;
	pieceIndex = randomInt;
	clearTime = 0;
	for (int i = 0; i < 4; i++)
	{
		currentPiece[i] = AllPieces[randomInt][i];
		currentBasePiece[i] = AllPieces[randomInt][i];
		nextPieceArr[i] = AllPieces[nextPiece][i];
	}
	resetSetter();
	for (int i = 0; i < 21; i++)
	{
		clearingLine[i] = false;
	}
	hasLost = false;
	score = 0;
}
bool TetrisBase::contains(int x, int y, Point(&pieceList)[4])
{
	for (int i = 0; i < 4; i++)
	{
		if (pieceList[i].X == x && pieceList[i].Y == y)
		{
			return true;
		}
	}
	return false;
}
void TetrisBase::copyArray(Point source[], Point(&dest)[4], int size)
{
	for (int i = 0; i < size; i++)
	{
		dest[i] = source[i];
	}
}
bool TetrisBase::isOccupied(int x, int y)
{
	if (x < 0 || x > 9) { return true; }
	if (y > 21) { return true; }
	return (grid[x][y] != 0);
}
void TetrisBase::clearLine(int y)
{
	for (int z = y; z > 0; z--)
	{
		for (int i = 0; i < 10; i++)
		{
			grid[i][z] = grid[i][z - 1];
		}
	}
}
bool TetrisBase::checkLine(int y)
{
	for (int i = 0; i < 10; i++)
	{
		if (grid[i][y] == 0)
		{
			return false;
		}
	}
	return true;
}
void TetrisBase::rotatePiece()
{
	//rotation limits 
	currentRotation++;
	int limit = 0;
	Point rotationMatrix[4];
	switch (pieceIndex)
	{
	case 0: limit = 1; break;
	case 1: limit = 3; break;
	case 2: limit = 3; break;
	case 3: limit = 3; break;
	case 4: limit = 1; break;
	case 5: limit = 1; break;
	case 6: limit = 0; break;
	default: break;
	}
	if (currentRotation > limit) currentRotation = 0;
	switch (pieceIndex)
	{
	case 0: copyArray(lineMat[currentRotation], rotationMatrix, 4); break;
	case 1: copyArray(tMat[currentRotation], rotationMatrix, 4); break;
	case 2: copyArray(leftLMat[currentRotation], rotationMatrix, 4); break;
	case 3: copyArray(rightLMat[currentRotation], rotationMatrix, 4); break;
	case 4: copyArray(zRevMat[currentRotation], rotationMatrix, 4); break;
	case 5: copyArray(zMat[currentRotation], rotationMatrix, 4); break;
	case 6: /*is a square so we just ignore and default to 0 transformations*/ break;
	default: break;
	}
	Point backup[4];
	copyArray(currentPiece, backup, 4);
	Point overAllShift = Point(0, 0);
	// Testing 
	for (int i = 0; i < 4; i++)
	{
		currentPiece[i] = Point(currentBasePiece[i].X + rotationMatrix[i].X + overAllShift.X,
			currentBasePiece[i].Y + rotationMatrix[i].Y + overAllShift.Y);
		while (currentPiece[i].X < 0)
		{
			overAllShift.X += 1;
			currentPiece[i].X += 1;
		}
		while (currentPiece[i].X > 9)
		{
			overAllShift.X -= 1;
			currentPiece[i].X -= 1;
		}
	}
	bool unable = false;
	for (int i = 0; i < 4; i++)
	{
		currentPiece[i] = Point(currentBasePiece[i].X + rotationMatrix[i].X + overAllShift.X,
			currentBasePiece[i].Y + rotationMatrix[i].Y + overAllShift.Y);
		if (grid[currentPiece[i].X][currentPiece[i].Y] != 0)
		{
			unable = true;
			break;
		}
	}
	if (unable)
	{
		currentRotation--;
		for (int i = 0; i < 4; i++)
		{
			currentPiece[i] = backup[i];
		}
	}
	resetSetter();
}
void TetrisBase::hardDrop()
{
	randomInt = rand() % 7;
	int oldPiece = pieceIndex + 1;
	pieceIndex = nextPiece;
	nextPiece = randomInt;
	currentRotation = 0;
	for (int i = 0; i < 4; i++)
	{
		grid[currentPieceSetter[i].X][currentPieceSetter[i].Y] = oldPiece;
		currentPiece[i] = AllPieces[pieceIndex][i];
		currentBasePiece[i] = AllPieces[pieceIndex][i];
		nextPieceArr[i] = AllPieces[randomInt][i];
	}
	for (int i = 21; i > -1; i--)
	{
		if (checkLine(i))
		{
			clearingLine[i] = true;
			clearTime = 1;
		}
		else
		{
			clearingLine[i] = false;
		}
	}
	if (!insertLines(linesToInsert))
	{
		hasLost = true;
	}
	resetSetter();
	hasHolded = false;
}
void TetrisBase::holdPiece()
{
	if (!hasHolded)
	{
		if (holdIndex == -1)
		{
			holdIndex = pieceIndex;
			randomInt = rand() % 7;
			pieceIndex = randomInt;
			for (int i = 0; i < 4; i++)
			{
				currentPiece[i] = AllPieces[randomInt][i];
				currentBasePiece[i] = AllPieces[randomInt][i];
				nextPieceArr[i] = AllPieces[nextPiece][i];
			}
			resetSetter();
		}
		else
		{
			int temp = pieceIndex;
			pieceIndex = holdIndex;
			holdIndex = temp;
			for (int i = 0; i < 4; i++)
			{
				currentPiece[i] = AllPieces[pieceIndex][i];
				currentBasePiece[i] = AllPieces[pieceIndex][i];
			}
		}
		hasHolded = true;
		resetSetter();
	}
}
void TetrisBase::resetSetter()
{
	bool set = false;
	copyArray(currentPiece, currentPieceSetter, 4);
	int yDrop = 0;
	while (!set)
	{
		for (int i = 0; i < 4; i++)
		{
			if (isOccupied(currentPieceSetter[i].X, currentPieceSetter[i].Y + 1))
			{
				set = true;
				break;
			}
		}
		if (!set)
		{
			for (int i = 0; i < 4; i++)
			{
				currentPieceSetter[i].Y++;
			}
		}
	}
}
void TetrisBase::generateNewPiece()
{
	randomInt = rand() % 7;
	int oldPiece = pieceIndex + 1;
	pieceIndex = nextPiece;
	nextPiece = randomInt;
	currentRotation = 0;
	bool cannotGenerate = false;
	for (int i = 0; i < 4; i++)
	{
		grid[currentPiece[i].X][currentPiece[i].Y] = oldPiece;
		currentPiece[i] = AllPieces[pieceIndex][i];
		currentBasePiece[i] = AllPieces[pieceIndex][i];
		nextPieceArr[i] = AllPieces[randomInt][i];
		if (isOccupied(currentPiece[i].X, currentPiece[i].Y))
		{
			cannotGenerate = true;
		}
	}
	resetSetter();
	if (cannotGenerate) //haslost
	{
		hasLost = true;
	}
}
void TetrisBase::movePieceLeft()
{
	canMove = true;
	for (int i = 0; i < 4; i++)
	{
		if (isOccupied(currentPiece[i].X - 1, currentPiece[i].Y))
		{
			canMove = false;
		}
	}
	if (canMove)
	{
		for (int i = 0; i < 4; i++)
		{
			currentPiece[i].X -= 1;
			currentBasePiece[i].X -= 1;
			currentPieceSetter[i].X -= 1;
		}
	}
	resetSetter();
}
void TetrisBase::movePieceRight()
{
	canMove = true;
	for (int i = 0; i < 4; i++)
	{
		if (isOccupied(currentPiece[i].X + 1, currentPiece[i].Y))
		{
			canMove = false;
		}
	}
	if (canMove)
	{
		for (int i = 0; i < 4; i++)
		{
			currentPiece[i].X += 1;
			currentBasePiece[i].X += 1;
			currentPieceSetter[i].X += 1;
		}
	}
	resetSetter();
}
bool TetrisBase::insertLines(int &lines)
{
	if (lines == 0) return true;
	
	bool okay = true;
	for (int y = 0; y < 22; y++)
	{
		int gap = rand() % 10;
		for (int x = 0; x < 10; x++)
		{
			if (y < lines)
			{
				if (grid[x][y] != 0)
				{
					grid[x][y] = 0;
					okay = false;
				}
			}
			else
			{
				grid[x][y - lines] = grid[x][y];
				grid[x][y] = 0;
				if (y >= 22 - lines)
				{
					if (x != gap)
					{
						grid[x][y] = 8;
					}
				}
			}
		}
	}
	lines = 0;
	return okay;
}
void TetrisBase::timerTick()
{
	allowMovementDown = true;
	hitBottom = false;
	for (int i = 0; i < 4; i++)
	{
		if (isOccupied(currentPiece[i].X, currentPiece[i].Y + 1))
		{
			allowMovementDown = false;
		}
		if (currentPiece[i].Y == 21)
		{
			hitBottom = true;
		}
	}
	
	if (hitBottom || !allowMovementDown)
	{
		if (freezeTime > 1)
		{ 
			freezeTime = 0;
			generateNewPiece();
			hasHolded = false;
			for (int i = 21; i > -1; i--)
			{
				if (checkLine(i))
				{
					clearingLine[i] = true;
					clearTime = 1;					
				}
				else
				{
					clearingLine[i] = false;
				}
			}
			if (!insertLines(linesToInsert))
			{
				hasLost = true;
			}
		}
		else
		{
			freezeTime++;
		}
	}
	else if (allowMovementDown)
	{
		freezeTime = 0;
		for (int i = 0; i < 4; i++)
		{
			currentPiece[i].Y += 1;
			currentBasePiece[i].Y += 1;
		}
	}
	
	if (clearTime >= 2)
	{
		int wendy = 0; 
		for (int i = 21; i > -1; i--)
		{
			if (clearingLine[i - wendy])
			{
				clearLine(i);
				clearingLine[i - wendy] = false;
				i++;
				wendy++;
			}
		}
		clearTime = 0;
		resetSetter();
		if (wendy == 1) score += 40;
		else if (wendy == 2) score += 100;
		else if (wendy == 3) score += 300;
		else if (wendy >= 4) score += 1200;
		hasClearedLines = wendy;
	}
	else if (clearTime >= 1)
	{
		clearTime++;
	}
}