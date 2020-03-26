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
	
	enum ChessDirection 
	{
		E_L_R,
		E_R_L,
		E_T_B,
		E_B_T,
		E_TL_BR,
		E_BR_TL,
		E_TR_BL,
		E_BL_TR,
	};

	enum GridType
	{
		E_MAYBE_TYPE,
		E_VALID_TYPE,
		E_OTHER_TYPE,
	};

	enum GridPriority
	{
		E_PRIORITY_HIGH,
		E_PRIORITY_MIDDLE,
		E_PRIORITY_LOW,
		E_PRIORITY_MAX,
	};

	Board();

	void Clear();
	char GetGrid(int id) { return grids[id]; }
	char GetGridType(int id) { return gridCheckStatus[id]; }
	void SetGrid(int id, char value, bool needReverse = true);
	void CheckGridStatus(int side);
	void GetValidGrids(array<uint8_t, GRID_NUM> &validGrids, int &validGridCount);
	void GetValidGridsByPriority(GridPriority priority, array<uint8_t, GRID_NUM> &validGrids, int &validGridCount);
	bool IsKeyGridsValid();
	void Print(int lastMove);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);
	static int GetOtherSide(int side);

	int blackCount, whiteCount;
	array<bool, E_PRIORITY_MAX> hasPriority;

private:
	char GetGrid(int row, int col);

	void MarkNearGrids(int id);
	bool TryReverseInDirection(int side, int id, ChessDirection dir, bool isChange);
	bool TryReverseInDirectionReal(int side, int row, int col, int dx, int dy, bool isChange);

	void PrintHSplitLine();
	void PrintVSplitLine();

	static void InitGridPriorityDict();
	static bool IsGridPriorityDictReady;
	static array<char, GRID_NUM> gridPriorityDict;

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
	bool IsGameFinish();
	void UpdateValidGrids();
	bool UpdateValidGridsExtra();
	bool IsOverwhelming();
	int CalcBetterSide();

	Board board;
	int state;
	int turn;
	int lastMove;
	int lastBlackCount;
	int lastWhiteCount;

	int validGridCount;
	array<uint8_t, GRID_NUM> validGrids;
};

class Game : private GameBase
{
public:
	int GetState() { return state; }
	int GetTurn() { return turn; }
	bool IsGameFinish() { return GameBase::IsGameFinish(); }

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

