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

	enum GridType {
		E_MAYBE_TYPE,
		E_VALID_TYPE,
		E_OTHER_TYPE,
	};

	Board();

	void Clear();
	char GetGrid(int id) { return grids[id]; }
	void SetGrid(int id, char value, bool needReverse = true);
	void GetValidGrids(int side, array<uint8_t, GRID_NUM> &validGrids, int &validGridCount);
	void Print(int lastMove, uint8_t* validGrids, int validGridCount);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);
	static int GetOtherSide(int side);

	int blackCount, whiteCount;

private:
	char GetGrid(int row, int col);

	void MarkNearGrids(int id);
	void CheckGridStatus(int side);
	bool TryReverseInDirection(int side, int id, ChessDirection dir, bool isChange);
	bool TryReverseInDirectionReal(int side, int row, int col, int dx, int dy, bool isChange);

	array<char, GRID_NUM> grids;
	array<char, GRID_NUM> gridCheckStatus;
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
	int CalcBetterSide();

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
	bool IsGameFinish() { return IsGameFinishThisTurn(); }

	bool PutChess(int id);
	void Regret(int step = 2);
	void Reset();
	void Print();

	const vector<uint8_t>& GetRecord() { return record; }
	static int Str2Id(const string &str);
	static string Id2Str(int id);

private:
	vector<uint8_t> record;
};

