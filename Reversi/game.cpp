#include "game.h"
#include <cstdlib>

#define max(a, b) ((a > b) ? a : b)

Board::Board()
{
	Clear();
}

void Board::Clear()
{
	for (int i = 0; i < GRID_NUM; ++i)
	{
		grids[i] = E_EMPTY;
	}
}

void Board::Print(uint8_t* validGrids, int validGridCount)
{
	// set valid grids
	for (int i = 0; i < validGridCount; ++i)
	{
		grids[validGrids[i]] = E_LEGAL;
	}

	cout << " ";
	for (int i = 1; i <= BOARD_SIZE; ++i)
	{
		printf("%2d", i);
	}
	cout << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf("%c ", 'A' + i);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);

			int grid = grids[id];

			if (grid == E_EMPTY)
			{
				cout << "+ ";
			}
			else if (grid == E_BLACK)
			{
				cout << "@ ";
			}
			else if (grid == E_WHITE)
			{
				cout << "O ";
			}
			else if (grid == E_LEGAL)
			{
				cout << "X ";
			}
		}
		cout << endl;
	}

	// clear valid grids
	for (int i = 0; i < validGridCount; ++i)
	{
		grids[validGrids[i]] = E_EMPTY;
	}
}

char Board::GetGrid(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return E_INVALID;

	return grids[Board::Coord2Id(row, col)];
}

bool Board::SetGrid(int row, int col, char value)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return false;

	grids[Board::Coord2Id(row, col)] = value;
	return true;
}

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

bool Board::TryReverseInDirectionReal(int side, int row, int col, int dx, int dy, bool isChange)
{
	bool valid = false;
	char otherSide = Board::GetOtherSide(side);

	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		char chess = GetGrid(row + dx * i, col + dy * i);

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
					for (int j = 1; j < i; ++j) // do reverse
					{
						SetGrid(row + dx * j, col + dy * j, side);
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
	blackCount = 2;
	whiteCount = 2;
	validGridCount = 0;

	board.grids[27] = Board::E_BLACK;
	board.grids[28] = Board::E_WHITE;
	board.grids[35] = Board::E_WHITE;
	board.grids[36] = Board::E_BLACK;

	UpdateValidGrids();
}

bool GameBase::PutChess(int id)
{
	if (board.grids[id] != Board::E_EMPTY)
		return false;

	if (state == E_NORMAL)
	{
		bool isValid = false;
		for (int i = 0; i < validGridCount; ++i)
		{
			if (validGrids[i] == id)
			{
				isValid = true;
				break;
			}
		}

		if (!isValid)
			return false;

		//////////////////////////////////////////////////////////////////////////
		int side = GetSide();
		board.grids[id] = side;
		lastMove = id;

		for (int i = 0; i < 8; ++i)
		{
			board.TryReverseInDirection(side, id, (Board::ChessDirection)i, true);
		}
	}
	else if (state == E_PASS)
	{
		lastMove = -1;
	}

	++turn;

	UpdateValidGrids();

	if (IsGameFinishThisTurn())
	{
		if (blackCount == whiteCount)
		{
			state = E_DRAW;
		}
		else if (blackCount > whiteCount)
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

bool GameBase::PutRandomChess()
{
	int id = rand() % validGridCount;
	swap(validGrids[id], validGrids[validGridCount - 1]);
	int gridId = validGrids[validGridCount - 1];

	return PutChess(gridId);
}

void GameBase::UpdateValidGrids()
{
	validGridCount = 0;

	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (board.grids[i] == Board::E_EMPTY)
		{
			// test 8 directions
			for (int j = 0; j < 8; ++j)
			{
				if (board.TryReverseInDirection(GetSide(), i, (Board::ChessDirection)j, false))
				{
					validGrids[validGridCount++] = i;
					break;
				}
			}
		}
	}
}

int GameBase::GetSide()
{
	return (turn % 2 == 1) ? Board::E_BLACK : Board::E_WHITE;
}

bool GameBase::IsGameFinishThisTurn()
{
	blackCount = whiteCount = 0;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (board.grids[i] == Board::E_BLACK)
			++blackCount;

		if (board.grids[i] == Board::E_WHITE)
			++whiteCount;
	}

	if (blackCount == 0 || whiteCount == 0)
		return true;

	if (blackCount + whiteCount == GRID_NUM)
		return true;

	if (state == E_PASS && validGridCount == 0) // no valid moves for both side
		return true;

	return false;
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
	{
		--turn;
		validGrids[validGridCount++] = record.back();
		board.grids[record.back()] = Board::E_EMPTY;
		record.pop_back();
	}

	lastMove = record.empty() ? -1 : record.back();
}

void Game::Print()
{
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw" };

	printf("=== Current State: %s ===\n", stateText[state].c_str());
	printf("Black: %d, White: %d\n", blackCount, whiteCount);
	board.Print(&(validGrids[0]), validGridCount);
	cout << endl;
}

int Game::Str2Id(const string &str)
{
	int row = str[0] - 'A';
	int col = str[1] - '1';
	if (!Board::IsValidCoord(row, col))
		return -1;

	int id = Board::Coord2Id(row, col);
	return id;
}

string Game::Id2Str(int id)
{
	int row, col;
	Board::Id2Coord(id, row, col);
	string result(1, row + 'A');
	result += col + '1';
	return result;
}
