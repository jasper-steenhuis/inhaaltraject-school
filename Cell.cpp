#include "precomp.h"
#include "Cell.h"
namespace Tmpl8
{

	Cell::Cell(float width, float height, vector<Tank*> tanks) : width_(width), height_(height), tanks_(tanks) { }

	Cell::~Cell() {};
	void Cell::add(Tank* tank)
	{
		tanks_.push_back(tank);
	}

	void Cell::remove(Tank* tank)
	{
		for (int i = 0; i < tanks_.size(); i++)
		{
			tanks_.erase(tanks_.begin() + i);
		}
	}

}
	


