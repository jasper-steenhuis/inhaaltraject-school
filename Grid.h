#pragma once

namespace Tmpl8
{
	class Tank; //Forward Declaration
	class Grid
	{
	public:
		Grid()
		{
			cells.clear();
			
		}
		int NUM_CELLS = 51;
		vector<vector<Tank*>> cells;
		void add(vector<Tank> tanks);
		const float CELL_SIZE =9.5f;
		

	private:
		
	};

}

