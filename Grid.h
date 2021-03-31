#pragma once
namespace Tmpl8
{
	class Grid; //Forward declare

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
		static const int NUM_CELLS = 10;
		static const int CELL_SIZE = 20;
	private:
		Tank* cells_[NUM_CELLS][NUM_CELLS];
	};
}