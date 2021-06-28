#pragma once

namespace Tmpl8
{
	class Tank; //Forward Declaration
	class Grid
	{
	public:
		Grid()
		{
			for (int x = 0; x < NUM_CELLS; x++)
			{
				for (int y = 0; y < NUM_CELLS; y++)
				{
					cells_[x][y] = NULL;
				}
			}
			
		}

		void add(Tank* tank);
		void handleTanks();
		void handleCell(Tank* tank);
		void handleCell(int x, int y);
		void handleCollision(Tank* tank, Tank* other);
		void moveCell(Tank* tank, float x, float y);
		void run(int from, int to);
		static const int NUM_CELLS = 100;
		static const int CELL_SIZE = 20;

	private:
		Tank* cells_[NUM_CELLS][NUM_CELLS];
	};

}
