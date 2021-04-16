#pragma once
namespace Tmpl8
{
	class Tank;
	class Grid
	{
	public:

		Grid()
		{
			for (size_t x = 0; x < NUM_CELLS; x++)
			{
				for (size_t y = 0; y < NUM_CELLS; y++)
				{
					cells_[x][y] = NULL;
				}
			}
		}
		void add(Tank* tank);
		void init(Tank* tank);
		void move(Tank* tank, double x, double y);
		void handleTank(Tank* tank, Tank* other);
		void handleCollision();
		void handleCell(Tank* tank);
		void handleCell(int x, int y);
		static const int NUM_CELLS = 51;
		 const float CELL_SIZE = 8.5f;
		Tank* cells_[NUM_CELLS][NUM_CELLS];

	private:
		

		
	};
}