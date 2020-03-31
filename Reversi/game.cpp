#include "game.h"
#include <cstdlib>

#define max(a, b) ((a > b) ? a : b)

bool Board::IsGridPriorityDictReady = false;
array<array<char, GRID_NUM>, GRID_PRIORITY_DICT_NUM> Board::gridPriorityDict;

Board::Board()
{
	if (!Board::IsGridPriorityDictReady)
		Board::InitGridPriorityDict();

	Clear();
}

void Board::Clear()
{
	grids.fill(E_EMPTY);
	gridCheckStatus.fill(E_OTHER_TYPE);

	blackCount = whiteCount = 0;
}

void Board::InitGridPriorityDict()
{
	for (int i = 0; i < GRID_PRIORITY_DICT_NUM; ++i)
	{
		Board::gridPriorityDict[i].fill(E_PRIORITY_MIDDLE);

		int winningGrids[4] = { Board::Coord2Id(0, 0), Board::Coord2Id(7, 0),
								Board::Coord2Id(0, 7), Board::Coord2Id(7, 7)};
		for (int j = 0; j < 4; ++j)
		{
			Board::gridPriorityDict[i][winningGrids[j]] = E_PRIORITY_HIGH;
		}

		int losingGrids[4][3] = {{Board::Coord2Id(0, 1), Board::Coord2Id(1, 0), Board::Coord2Id(1, 1) },
								{ Board::Coord2Id(0, 6), Board::Coord2Id(1, 7), Board::Coord2Id(1, 6) },
								{ Board::Coord2Id(7, 1), Board::Coord2Id(6, 0), Board::Coord2Id(6, 1) },
								{ Board::Coord2Id(7, 6), Board::Coord2Id(6, 7), Board::Coord2Id(6, 6)}};
		for (int j = 0; j < 4; ++j)
		{
			bool isCornerFill = i & (1 << j);
			if (!isCornerFill)
			{
				for (int k = 0; k < 3; ++k)
					Board::gridPriorityDict[i][losingGrids[j][k]] = E_PRIORITY_LOW;
			}
		}
	}

	Board::IsGridPriorityDictReady = true;
}

void Board::PrintSplitLine(int i)
{
	for (int j = 0; j <= BOARD_SIZE; ++j)
	{
		if (i == 0)
		{
			if (j == 0)
			{
				printf("  ┌─");
			}
			else if (j == BOARD_SIZE)
			{
				printf("──┐");
			}
			else
			{
				printf("──┬─");
			}
		}
		else if (i == BOARD_SIZE)
		{
			if (j == 0)
			{
				printf("  └─");
			}
			else if (j == BOARD_SIZE)
			{
				printf("──┘");
			}
			else
			{
				printf("──┴─");
			}
		}
		else
		{
			if (j == 0)
			{
				printf("  ├─");
			}
			else if (j == BOARD_SIZE)
			{
				printf("──┤");
			}
			else
			{
				printf("──┼─");
			}
		}
	}
	cout << endl;
}

void Board::Print(int lastMove)
{
	cout << "    Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ";
	cout << endl;

	string digits[] = { "１", "２", "３", "４", "５", "６", "７", "８" };

	PrintSplitLine(0);
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		cout << digits[i] << "│";

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int grid = grids[id];
			if (id == lastMove)
			{
				cout << ((grid == E_BLACK) ? " ◎│" : " ◎│");
				continue;
			}

			if (grid == E_BLACK)
			{
				cout << " ●│";
			}
			else if (grid == E_WHITE)
			{
				cout << " ○│";
			}
			else if (grid == E_EMPTY && gridCheckStatus[id] == E_VALID_TYPE)
			{
				cout << " ×│";
			}
			else
			{
				cout << "   │";
			}
		}
		cout << endl;
		PrintSplitLine(i + 1);
	}
}

void Board::SetGrid(int id, char value, bool needReverse)
{
	int oldValue = grids[id];
	if (oldValue == value)
		return;

	if (oldValue == E_BLACK)
		--blackCount;

	if (oldValue == E_WHITE)
		--whiteCount;

	if (value == E_BLACK)
		++blackCount;

	if (value == E_WHITE)
		++whiteCount;

	grids[id] = value;
	MarkNearGrids(id);
	UpdatePriorityDictKey();

	if (needReverse)
	{
		for (int i = 0; i < 8; ++i)
		{
			TryReverseInDirection(value, id, (ChessDirection)i, true);
		}
	}
}

char Board::GetGrid(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return E_INVALID;

	return grids[Board::Coord2Id(row, col)];
}

void Board::GetValidGridsByPriority(GridPriority priority, array<uint8_t, GRID_NUM> &validGrids, int &validGridCount)
{
	validGridCount = 0;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (gridCheckStatus[i] == E_VALID_TYPE && Board::gridPriorityDict[priorityDictKey][i] == priority)
			validGrids[validGridCount++] = i;
	}
}

bool Board::IsKeyGridsValid()
{
	int keyGrids[4] = { 0, BOARD_SIZE - 1, BOARD_SIZE * (BOARD_SIZE - 1), BOARD_SIZE * BOARD_SIZE - 1 };
	for (int i = 0; i < 4; ++i)
	{
		if (gridCheckStatus[keyGrids[i]] == E_VALID_TYPE)
		{
			return true;
		}
	}
	return false;
}

void Board::MarkNearGrids(int id)
{
	gridCheckStatus[id] = E_OTHER_TYPE;

	int row, col;
	Board::Id2Coord(id, row, col);

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			int chess = GetGrid(row + i, col + j);
			if (chess == E_EMPTY)
			{
				int id1 = Board::Coord2Id(row + i, col + j);
				gridCheckStatus[id1] = E_MAYBE_TYPE;
			}
		}
	}
}

__declspec(noinline)
void Board::CheckGridStatus(int side)
{
	hasPriority.fill(false);

	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (gridCheckStatus[i] != E_OTHER_TYPE)
		{
			gridCheckStatus[i] = E_MAYBE_TYPE;
			for (int k = 0; k < 8; ++k)
			{
				if (TryReverseInDirection(side, i, (ChessDirection)k, false))
				{
					gridCheckStatus[i] = E_VALID_TYPE;
					hasPriority[Board::gridPriorityDict[priorityDictKey][i]] = true;
					break;
				}
			}
		}
	}
}

__declspec(noinline)
bool Board::TryReverseInDirection(int side, int id, ChessDirection dir, bool isChange)
{
	int row = 0, col = 0;
	Board::Id2Coord(id, row, col);

	int dx = 0, dy = 0;
	switch (dir)
	{
	case Board::E_L_R:
		dx = 1;
		break;
	case Board::E_R_L:
		dx = -1;
		break;
	case Board::E_T_B:
		dy = 1;
		break;
	case Board::E_B_T:
		dy = -1;
		break;
	case Board::E_TL_BR:
		dx = dy = 1;
		break;
	case Board::E_BR_TL:
		dx = dy = -1;
		break;
	case Board::E_TR_BL:
		dx = 1;
		dy = -1;
		break;
	case Board::E_BL_TR:
		dx = -1;
		dy = 1;
		break;
	}

	return TryReverseInDirectionReal(side, row, col, dx, dy, isChange);
}

__declspec(noinline)
bool Board::TryReverseInDirectionReal(int side, int row, int col, int dx, int dy, bool isChange)
{
	bool valid = false;
	char otherSide = Board::GetOtherSide(side);

	int row1 = row, col1 = col;
	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		row1 += dy; col1 += dx;
		char chess = GetGrid(row1, col1);

		if (!valid) // try to find an opposite chess
		{
			if (chess == otherSide)
			{
				valid = true;
			}
			else
			{
				return false;
			}
		}
		else // try to find a same chess
		{
			if (chess == side)
			{
				if (isChange)
				{
					int id1 = Board::Coord2Id(row, col);
					for (int j = 1; j < i; ++j) // do reverse
					{
						id1 += dy * BOARD_SIZE + dx;
						SetGrid(id1, side, false);
					}
				}
				return true;
			}
			else if (chess == otherSide)
			{
				// do nothing
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

void Board::UpdatePriorityDictKey()
{
	priorityDictKey = 0;

	int cornerGrids[4] = { Board::Coord2Id(0, 0), Board::Coord2Id(7, 0), Board::Coord2Id(0, 7), Board::Coord2Id(7, 7) };
	for (int i = 0; i < 4; ++i)
	{
		if (grids[cornerGrids[i]] != E_EMPTY)
			priorityDictKey += (1 << i);
	}
}

int Board::Coord2Id(int row, int col)
{
	return row * BOARD_SIZE + col;
}

void Board::Id2Coord(int id, int &row, int &col)
{
	row = id / BOARD_SIZE;
	col = id % BOARD_SIZE;
}

bool Board::IsValidCoord(int row, int col)
{
	return (0 <= row && row < BOARD_SIZE && 0 <= col && col < BOARD_SIZE);
}

int Board::GetOtherSide(int side)
{
	return 3 - (int)side;
}

///////////////////////////////////////////////////////////////////////

GameBase::GameBase()
{
	Init();
}

void GameBase::Init()
{
	turn = 1;
	lastMove = -1;
	board.Clear();
	state = E_NORMAL;
	validGridCount = 0;
	lastBlackCount = 0;
	lastWhiteCount = 0;

	board.SetGrid(Game::Str2Id("D4"), Board::E_WHITE);
	board.SetGrid(Game::Str2Id("D5"), Board::E_BLACK);
	board.SetGrid(Game::Str2Id("E4"), Board::E_BLACK);
	board.SetGrid(Game::Str2Id("E5"), Board::E_WHITE);

	UpdateValidGrids();
}

__declspec(noinline)
bool GameBase::PutChess(int id)
{
	lastBlackCount = board.blackCount;
	lastWhiteCount = board.whiteCount;

	if (state == E_NORMAL)
	{
		if (board.GetGrid(id) != Board::E_EMPTY)
			return false;

		if (board.GetGridType(id) != Board::E_VALID_TYPE)
			return false;

		//////////////////////////////////////////////////////////////////////////
		int side = GetSide();
		board.SetGrid(id, side);
		lastMove = id;
	}
	else if (state == E_PASS)
	{
		lastMove = -1;
	}

	++turn;

	UpdateValidGrids();

	if (IsGameFinishThisTurn())
	{
		if (board.blackCount == board.whiteCount)
		{
			state = E_DRAW;
		}
		else if (board.blackCount > board.whiteCount)
		{
			state = E_BLACK_WIN;
		}
		else
		{
			state = E_WHITE_WIN;
		}
	}

	if (state == E_NORMAL && validGridCount == 0) // enter pass state if not valid grid is found
		state = E_PASS;

	if (state == E_PASS && validGridCount > 0) // restore from pass state
		state = E_NORMAL;

	return true;
}

__declspec(noinline)
bool GameBase::PutRandomChess()
{
	if (state == E_PASS)
		return PutChess(-1);

	int id = rand() % validGridCount;
	swap(validGrids[id], validGrids[validGridCount - 1]);
	int gridId = validGrids[validGridCount - 1];

	return PutChess(gridId);
}

void GameBase::UpdateValidGrids()
{
	board.CheckGridStatus(GetSide());

	validGridCount = 0;
	for (int i = Board::E_PRIORITY_HIGH; i < Board::E_PRIORITY_MAX; ++i)
	{
		if (board.hasPriority[i])
		{
			board.GetValidGridsByPriority((Board::GridPriority)i, validGrids, validGridCount);
			break;
		}
	}
}

bool GameBase::UpdateValidGridsExtra()
{
	if (turn < GRID_NUM / 2)
		return false;

	bool isExtra = false;
	for (int i = Board::E_PRIORITY_HIGH; i < Board::E_PRIORITY_MAX; ++i)
	{
		if (board.hasPriority[i])
		{
			if (!isExtra)
			{
				isExtra = true;
			}
			else
			{
				board.GetValidGridsByPriority((Board::GridPriority)i, validGrids, validGridCount);
				break;
			}
		}
	}
	return false;
}

int GameBase::GetSide()
{
	return (turn % 2 == 1) ? Board::E_BLACK : Board::E_WHITE;
}

bool GameBase::IsGameFinishThisTurn()
{
	if (board.blackCount == 0 || board.whiteCount == 0)
		return true;

	if (board.blackCount + board.whiteCount == GRID_NUM)
		return true;

	if (state == E_PASS && validGridCount == 0) // no valid moves for both side
		return true;

	return false;
}

bool GameBase::IsGameFinish()
{
	return state != E_NORMAL && state != E_PASS;
}

bool GameBase::IsOverwhelming()
{
	if (turn <= GRID_NUM / 2)
	{
		return board.IsKeyGridsValid();
	}
	return false;
}

int GameBase::CalcBetterSide()
{
	int blackCount = lastBlackCount + board.blackCount;
	int whiteCount = lastWhiteCount + board.whiteCount;

	if (blackCount > whiteCount)
		return E_BLACK_WIN;

	if (blackCount < whiteCount)
		return E_WHITE_WIN;

	return E_DRAW;
}

///////////////////////////////////////////////////////////////////

bool Game::PutChess(int Id)
{
	if (GameBase::PutChess(Id))
	{
		record.push_back(lastMove);
		return true;
	}
	return false;
}

void Game::Regret(int step)
{
	while (!record.empty() && --step >= 0)
		record.pop_back();

	GameBase::Init();
	for (int i = 0; i < record.size(); ++i)
	{
		GameBase::PutChess(record[i]);
	}
}

void Game::Reset()
{
	GameBase::Init();
	record.clear();
}

void Game::Print()
{
	string sideText[] = { "Black", "White" };
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw", "Pass" };

	cout << endl;
	printf("   ==== Turn %02d, %s's turn ====\n", GetTurn(), sideText[GetSide() - 1].c_str());
	printf("   ==== Current State: %s ====\n", stateText[state].c_str());
	printf("   ==== Black: %02d | White: %02d ====\n\n", board.blackCount, board.whiteCount);
	board.Print(lastMove);
}

int Game::Str2Id(const string &str)
{
	int col = str[0] - 'A';
	int row = str[1] - '1';
	if (!Board::IsValidCoord(row, col))
		return -1;

	int id = Board::Coord2Id(row, col);
	return id;
}

string Game::Id2Str(int id)
{
	if (id == -1)
		return "pass";

	int row, col;
	Board::Id2Coord(id, row, col);
	string result(1, col + 'A');
	result += row + '1';
	return result;
}
