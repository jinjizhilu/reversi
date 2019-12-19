#include "game.h"
#include "ctime"

int main()
{
	srand((unsigned)time(NULL));

	Game g;
	string input;
	int move;

	while (g.GetState() == GameBase::E_NORMAL)
	{
		g.Print();

		while (1)
		{
			cout << "Enter your move: ";
			cin >> input;

			if (input == "undo" && g.GetTurn() > 2)
			{
				g.Regret(2);
				g.Print();
				continue;
			}

			if (input == "pass" && g.GetState() == GameBase::E_PASS)
			{
				g.PutChess(-1);
				break;
			}

			move = Game::Str2Id(input);
			if (move != -1)
			{
				if (g.PutChess(move))
					break;
			}

			cout << "!Invalid move!" << endl;
		}

		if (g.GetState() != GameBase::E_NORMAL)
			break;
	}
	g.Print();

	return 0;
}