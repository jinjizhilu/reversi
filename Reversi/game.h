#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <list>

#pragma warning (disable:4244)
#pragma warning (disable:4018)

using namespace std;

const int BOARD_SIZE = 8;
const int GRID_NUM = BOARD_SIZE * BOARD_SIZE;

class Board
{
public:
	enum Chess
	{
		E_EMPTY,
		E_BLACK,
		E_WHITE,
		E_LEGAL,
		E_INVALID,
	};
	
	enum ChessDirection {
		E_L_R,
		E_R_L,
		E_T_B,
		E_B_T,
		E_TL_BR,
		E_BR_TL,
		E_TR_BL,
		E_BL_TR,
	};

	Board();

	void Clear();
	void Print(uint8_t* validGrids, int validGridCount);

	bool TryReverseInDirection(int side, int id, ChessDirection dir, bool isChange);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);
	static int GetOtherSide(int side);

	array<char, GRID_NUM> grids;

private:
	char GetGrid(int row, int col);
	bool SetGrid(int row, int col, char value);
	bool TryReverseInDirectionReal(int side, int row, int col, int dx, int dy, bool isChange);
};

class GameBase
{
public:
	enum State
	{
		E_NORMAL,
		E_BLACK_WIN,
		E_WHITE_WIN,
		E_DRAW,
		E_PASS,
	};

	GameBase();
	void Init();
	bool PutChess(int id);
	bool PutRandomChess();
	int GetSide();
	bool IsGameFinishThisTurn();
	void UpdateValidGrids();

	Board board;
	int state;
	int turn;
	int lastMove;
	int blackCount;
	int whiteCount;

	int validGridCount;
	array<uint8_t, GRID_NUM> validGrids;
};

class Game : private GameBase
{
public:
	int GetState() { return state; }
	int GetTurn() { return turn; }

	bool PutChess(int id);
	void Regret(int step = 2);
	void Print();

	const vector<uint8_t>& GetRecord() { return record; }
	static int Str2Id(const string &str);
	static string Id2Str(int id);

private:
	vector<uint8_t> record;
};

