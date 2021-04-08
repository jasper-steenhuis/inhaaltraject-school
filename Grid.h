#pragma once
namespace Tmpl8
{
	class Tank;
	class Grid
	{
	public:

		Grid()
		{
			cells_.resize(NUM_CELLS);
			for (size_t x = 0; x < NUM_CELLS; x++)
			{
				cells_.at(x).resize(NUM_CELLS);
				for (size_t y = 0; y < NUM_CELLS; y++)
				{
					cells_[x][y] = NULL;
				}
			}
		}
		void add(Tank* tank);
		void move(Tank* tank, double x, double y);
		void handleTank(Tank* tank, Tank* other);
		void handleCollision();
		void handleCell(Tank* tank);
		void handleCell(int x, int y);
		static const int NUM_CELLS = 10;
		const float CELL_SIZE = 8.5f ;
		vector<vector<Tank*>> cells_;

	private:
		

		
	};
}