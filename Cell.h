#pragma once
#include "tank.h"
namespace Tmpl8
{
	class Cell
	{
public:

		Cell(float width, float height, vector<Tank*> tanks);
		~Cell();
		
		void add(Tank* tank);
		void remove(Tank* tank);
	
private:
	float width_;
	float height_;
	vector<Tank*> tanks_;
	};
}

