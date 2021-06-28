#include "precomp.h"
#include "Grid.h"

unsigned int threads = std::thread::hardware_concurrency();
ThreadPool threadpool(threads);
std::vector<std::future<void>> futures;
std::mutex mut;

void Tmpl8::Grid::add(Tank* tank)
{
	int cellX = (int)(tank->x_ / CELL_SIZE);
	int cellY = (int)(tank->y_ / CELL_SIZE);

	tank->prev_ = NULL;
	if (tank->x_ >= 0 && tank->y_ >= 0 && tank->x_ < 9999 && tank->y_ < 9999)
	{
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
	
}

void Tmpl8::Grid::handleTanks()
{
	futures.push_back(threadpool.enqueue([&]() {run(0, 25);}));
	futures.push_back(threadpool.enqueue([&]() {run(26, 50);}));
	futures.push_back(threadpool.enqueue([&]() {run(51, 75);}));
	futures.push_back(threadpool.enqueue([&]() {run(76, 100);}));
	
	for (auto& fut : futures)
	{
		fut.wait();
	}

	
}

void Tmpl8::Grid::handleCell(Tank* tank)
{
	while (tank != NULL && tank ->active)
	{
		
		Tank* other = tank->next_;
		if(other != NULL && other->active)
		{
			handleCell(tank->x_, tank->y_);
			other = other->next_;
		}
		tank = tank->next_;
	}
	
}

void Tmpl8::Grid::handleCell(int x, int y)
{
	
	int cellX = (int)(x / CELL_SIZE);
	int cellY = (int)(y / CELL_SIZE);
	if (x >= 0 && y >= 0 && x < 9999 && y < 9999)
	{

		Tank* tank = cells_[cellX][cellY];

		while (tank != NULL && tank->active)
		{
			if (tank->next_ != NULL && tank->next_->active)
			{
				std::lock_guard<std::mutex> lock(mut);
				handleCollision(tank, tank->next_);
			}
			if (cellX > 0 && cellY > 0)
			{
				std::lock_guard<std::mutex> lock(mut);

				handleCollision(tank, cells_[cellX - 1][cellY - 1]);
			}
			if (cellX > 0)
			{
				std::lock_guard<std::mutex> lock(mut);

				handleCollision(tank, cells_[cellX - 1][cellY]);
			}
			if (cellY > 0)
			{
				std::lock_guard<std::mutex> lock(mut);

				handleCollision(tank, cells_[cellX][cellY - 1]);
			}
			if (cellX > 0 && cellY < NUM_CELLS - 1)
			{
				std::lock_guard<std::mutex> lock(mut);

				tank = tank->next_;
			}
		}
	}
}

void Tmpl8::Grid::handleCollision(Tank* tank, Tank* other)
{
	std::lock_guard<std::mutex> lock(mut);

	while (other != NULL && other->active)
	{

		if (tank == other) return;
	
		vec2 dir = tank->get_position() - other->get_position();
		float dir_squared_len = dir.sqr_length();

		float col_squared_len = (tank->get_collision_radius() + other->get_collision_radius());
		col_squared_len *= col_squared_len;
		if (dir_squared_len <= col_squared_len)
		{
			if (tank->active)
			{
				tank->push(dir.normalized(), 0.5f);
			}
			
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
	if (x >= 0 && y >= 0 && x < 9999 && y < 9999)
	{
		
		if (oldCellX == cellX && oldCellY == cellY) return;

		if (tank->prev_ != NULL)
		{
			tank->prev_->next_ = tank->next_;
		}
		if (tank->next_ != NULL)
		{
			tank->next_->prev_ = tank->prev_;
		}

		if (cells_[oldCellX][oldCellY] != NULL && cells_[oldCellX][oldCellY] == tank)
		{
			cells_[oldCellX][oldCellY] = tank->next_;
		}
		add(tank);

	}
}

void Tmpl8::Grid::run(int from, int to)
{
	for (int y = from; y < to; y++)
	{
		for (int x = from; x < to; x++)
		{
			
			handleCell(cells_[y][x]);
		}

	}
}

