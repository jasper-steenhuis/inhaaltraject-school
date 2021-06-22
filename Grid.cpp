#include "precomp.h"
#include "Grid.h"


void Tmpl8::Grid::add(Tank* tank)
{
	int cellX = (int)(tank->x_ / CELL_SIZE);
	int cellY = (int)(tank->y_ / CELL_SIZE);

	tank->prev_ = NULL;
	if (!cells_[cellX][cellY] == NULL)
	{
		tank->next_ = cells_[cellX][cellY];
	}
	cells_[cellX][cellY] = tank;

	if (tank->next_ != NULL)
	{
		if (tank->next_->active)
		{
			tank->next_->prev_ = tank;

		}
	}
}

void Tmpl8::Grid::handleTanks()
{
	for (int x = 0; x < NUM_CELLS; x++)
	{
		for (int y = 0; y < NUM_CELLS; y++)
		{
			handleCell(cells_[x][y]);
		}
	}
}

void Tmpl8::Grid::handleCell(Tank* tank)
{
	while (tank != NULL && tank ->active)
	{
		Tank* other = tank->next_;
		if (other != NULL && other->active)
		{
			
			handleCell(tank->x_, tank->y_);
			other = other->next_;
		}
		tank = tank->next_;
	}
}

void Tmpl8::Grid::handleCell(int x, int y)
{
	Tank* tank = cells_[x][y];
	while (tank != NULL && tank->active)
	{
		if (tank->next_ != NULL && tank->next_->active)
		{
			handleCollision(tank, tank->next_);
		}
		if (x > 0 && y > 0)
		{
			handleCollision(tank, cells_[x - 1][y - 1]);
		}
		if (x > 0)
		{
			handleCollision(tank, cells_[x - 1][y]);
		}
		if (y > 0)
		{
			handleCollision(tank, cells_[x][y - 1]);
		}
		if (x > 0 && y < NUM_CELLS - 1)
		{
			tank = tank->next_;
		}
	}
}

void Tmpl8::Grid::handleCollision(Tank* tank, Tank* other)
{
	while (other != NULL && other->active)
	{
		if (tank == other) return;
		vec2 dir = tank->get_position() - other->get_position();
		float dir_squared_len = dir.sqr_length();

		float col_squared_len = (tank->get_collision_radius() + other->get_collision_radius());
		col_squared_len *= col_squared_len;

		while (dir_squared_len < col_squared_len)
		{
			tank->push(dir.normalized(), 1.f);
		}
		other = other->next_;
	}
	
}
void Tmpl8::Grid::moveCell(Tank* tank, float x, float y)
{
	int oldCellX = (int)(tank->x_ / CELL_SIZE);
	int oldCellY = (int)(tank->y_ / CELL_SIZE);

	int cellX = (int)(x / CELL_SIZE);
	int cellY = (int)(y / CELL_SIZE);

	tank->x_ = x;
	tank->y_ = y;

	if (oldCellX == cellX && oldCellY == cellY) return;

	if (tank->prev_ != NULL)
	{
		tank->prev_->next_ = tank->next_;
	}
	if (tank->next_!= NULL)
	{
		tank->next_->prev_ = tank->prev_;
	}

	if (cells_[oldCellX][oldCellY] == tank)
	{
		cells_[oldCellX][oldCellY] = tank->next_;
	}

	add(tank);
}

