#include "precomp.h" // include (only) this in every .cpp file
#include "thread_pool.h"
#define NUM_TANKS_BLUE 1279
#define NUM_TANKS_RED 1279

#define TANK_MAX_HEALTH 1000
#define ROCKET_HIT_VALUE 60
#define PARTICLE_BEAM_HIT_VALUE 50

#define TANK_MAX_SPEED 1.5

#define HEALTH_BARS_OFFSET_X 0
#define HEALTH_BAR_HEIGHT 70
#define HEALTH_BAR_WIDTH 1
#define HEALTH_BAR_SPACING 0

#define MAX_FRAMES 2000

//Global performance timer
#define REF_PERFORMANCE 59636.6   //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface* background_img = new Surface("assets/Background_Grass.png");
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite background(background_img, 1);
static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(14, 18);
const static vec2 rocket_size(25, 24);

const static float tank_radius = 8.5f;
const static float rocket_radius = 10.f;
std::mutex m;
Grid* grid = new Grid();

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::init()
{
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);
    tanks_health.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);
    
    
   
    uint rows = (uint)sqrt(NUM_TANKS_BLUE + NUM_TANKS_RED);
    uint max_rows = 12;

    float start_blue_x = tank_size.x + 10.0f;
    float start_blue_y = tank_size.y + 80.0f;

    float start_red_x = 980.0f;
    float start_red_y = 100.0f;

    float spacing = 15.0f;

    

    //Spawn blue tanks
    for (int i = 0; i < NUM_TANKS_BLUE; i++)
    {
        tanks.push_back(Tank(start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing), BLUE, &tank_blue, &smoke, 1200, 600, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED,grid));
    }
    //Spawn red tanks
    
    for (int i = 0; i < NUM_TANKS_RED; i++)
    {
        tanks.push_back(Tank(start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing), RED, &tank_red, &smoke, 80, 80, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED,grid));
    }    
    for (int i = 0; i < tanks.size()-1; i++)
    {
        tanks_health.push_back(tanks.at(i).health);
        
    }
    mergeSort(tanks_health);
    particle_beams.push_back(Particle_beam(vec2(SCRWIDTH / 2, SCRHEIGHT / 2), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(Particle_beam(vec2(80, 80), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::shutdown()
{
}

void Tmpl8::Game::updateExplosions()
{
    //Update explosion sprites and remove when done with remove erase idiom
    for (Explosion& explosion : explosions)
    {
        explosion.tick();
    }

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());

}
void Tmpl8::Game::updateSmokes()
{
    //Update smoke plumes
    for (Smoke& smoke : smokes)
    {
        smoke.tick();
    }
}
void Tmpl8::Game::updateRockets()
{
    //Update rockets
    for (Rocket& rocket : rockets)
    {
        rocket.tick();

        //Check if rocket collides with enemy tank, spawn explosion and if tank is destroyed spawn a smoke plume
        for (Tank& tank : tanks)
        {
            if (tank.active && (tank.allignment != rocket.allignment) && rocket.intersects(tank.position, tank.collision_radius))
            {
                explosions.push_back(Explosion(&explosion, tank.position));

                if (tank.hit(ROCKET_HIT_VALUE))
                {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                }

                rocket.active = false;
                break;
            }
        }
    }

    //Remove exploded rockets with remove erase idiom
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());

}
void Tmpl8::Game::updateParticleBeams()
{
    //Update particle beams
    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.tick(tanks);

        //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
        for (Tank& tank : tanks)
        {
            if (tank.active && particle_beam.rectangle.intersects_circle(tank.get_position(), tank.get_collision_radius()))
            {
                if (tank.hit(particle_beam.damage))
                {
                    smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                }
            }
        }
    }
}
void Tmpl8::Game::updateTanks()
{
  
    //Update tanks
    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            //Check tank collision and nudge tanks away from each other
            /*for (Tank& o_tank : tanks)
            {
                if (&tank == &o_tank) continue;

                vec2 dir = tank.get_position() - o_tank.get_position();
                float dir_squared_len = dir.sqr_length();

                float col_squared_len = (tank.get_collision_radius() + o_tank.get_collision_radius());
                col_squared_len *= col_squared_len;
                

                if (dir_squared_len < col_squared_len)
                {
                    tank.push(dir.normalized(), 1.f);
                }
                
                
            }*/
            //Move tanks according to speed and nudges (see above) also reload
            grid->handleCollision();
           
            tank.tick();
            
            //Shoot at closest target if reloaded
            if (tank.rocket_reloaded())
            {
                
                Tank& target = find_closest_enemy(tank);
                
                rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

                tank.reload_rocket();
                
            }
        }
    }


}


// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// -----------------------------------------------------------
Tank& Game::find_closest_enemy(Tank& current_tank)
{
    float closest_distance = numeric_limits<float>::infinity();
    int closest_index = 0;

    for (int i = tanks.size() -1; i > 0; i--)
    {
        if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active)
        {
            float sqr_dist = fabsf((tanks.at(i).get_position() - current_tank.get_position()).sqr_length());
            if (sqr_dist < closest_distance)
            {
                closest_distance = sqr_dist;
                closest_index = i;
            }
        }
    }

    return tanks.at(closest_index);
}

// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::update(float deltaTime)
{
    std::thread t1([this] { this->updateTanks(); });
    t1.join();
    updateRockets();
    updateSmokes();
    updateParticleBeams();
    updateExplosions();
    
}

void Game::draw()
{
    // clear the graphics window
    screen->clear(0);

    //Draw background
    background.draw(screen, 0, 0);

    //Draw sprites
    for (int i = 0; i < NUM_TANKS_BLUE + NUM_TANKS_RED; i++)
    {
        tanks.at(i).draw(screen);

        vec2 tank_pos = tanks.at(i).get_position();
        // tread marks
        if ((tank_pos.x >= 0) && (tank_pos.x < SCRWIDTH) && (tank_pos.y >= 0) && (tank_pos.y < SCRHEIGHT))
            background.get_buffer()[(int)tank_pos.x + (int)tank_pos.y * SCRWIDTH] = sub_blend(background.get_buffer()[(int)tank_pos.x + (int)tank_pos.y * SCRWIDTH], 0x808080);
    }

    for (Rocket& rocket : rockets)
    {
        rocket.draw(screen);
    }

    for (Smoke& smoke : smokes)
    {
        smoke.draw(screen);
    }

    for (Particle_beam& particle_beam : particle_beams)
    {
        particle_beam.draw(screen);
    }

    for (Explosion& explosion : explosions)
    {
        explosion.draw(screen);
    }
   
    //Draw sorted health bars
    for (int t = 0; t < 2; t++)
    {
        const int NUM_TANKS = ((t < 1) ? NUM_TANKS_BLUE : NUM_TANKS_RED);

        const int begin = ((t < 1) ? 0 : NUM_TANKS_BLUE);
        
        for (int i = 0; i < NUM_TANKS; i++)
        {
            tanks_health.at(i) = tanks.at(i).health;
            int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
            int health_bar_start_y = (t < 1) ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
            int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
            int health_bar_end_y = (t < 1) ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

            screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
            screen->bar(health_bar_start_x, health_bar_start_y + (int)((double)HEALTH_BAR_HEIGHT * (1 - ((double) tanks_health.at(i) / (double)TANK_MAX_HEALTH))), health_bar_end_x, health_bar_end_y, GREENMASK);
            
        }
        
        
    }
    
}

void Tmpl8::Game::mergeSort(std::vector<int>& original)
{
    if (original.size() <= 1 )
    {
        return;
    }
    int mid = original.size() / 2;
    std::vector<int> left;
    std::vector<int> right;

    left.reserve(mid);
    right.reserve(original.size() - mid);

    for (int i = 0; i < mid - 1; i++) 
    {
        left.push_back(original.at(i));
    }
    for (int j = 0; j < (original.size()) - mid; j++)
    {
        right.push_back(original.at(mid + j));
    }
    mergeSort(left);
    mergeSort(right);
    merge(left, right);

}

std::vector<int> Tmpl8::Game::merge(std::vector<int>& left, std::vector<int>& right)
{
    std::vector<int> results;
    int l_size = left.size();
    int r_size = right.size();
    results.reserve(l_size + r_size);
    int i = 0;
    int j = 0;
    int k = 0;

    while (j < l_size && k < r_size)
    {
        if (left[j] <= right[k])
        {
            results.emplace_back(left[j]);
            j++;
        }
        else
        {
            results.emplace_back(right[k]);
            k++;
        }
        i++;
    }
    while (j < l_size)
    {
        results.emplace_back(left[j]);
        j++;
        i++;
    }
    while (k < r_size)
    {
        results.emplace_back(right[k]);
        k++;
        i++;
    }

   
    return results;
}

void Tmpl8::Grid::add(Tank* tank)
{
    //determine grid cell index
    int cellX = (int)(tank->position.x / Grid::CELL_SIZE);
    int cellY = (int)(tank->position.y / Grid::CELL_SIZE);

    tank->prev_ = NULL;
    tank->next_ = cells_[cellX][cellY];
    cells_[cellX][cellY] = tank;

    if (tank->next_ != NULL)
    {
        tank->next_->prev_ = tank;
    }
}

void Tmpl8::Grid::move(Tank* tank, double x, double y)
{
    int oldCellX = (int)(tank->position.x / Grid::CELL_SIZE);
    int oldCellY = (int)(tank->position.y / Grid::CELL_SIZE);

    int cellX = (int)(x / Grid::CELL_SIZE);
    int cellY = (int)(y / Grid::CELL_SIZE);
    
    tank->position.x = x;
    tank->position.y = y;

    if (oldCellX == cellX && oldCellY == cellY)
    {
        return;
    }
    if (tank->prev_ != NULL)
    {
        tank->prev_->next_ = tank->next_;
    }
    if (tank->next_ != NULL)
    {
        tank->next_->prev_ = tank->prev_;
    }
    if (cells_[oldCellX][oldCellY] == tank)
    {
        cells_[oldCellX][oldCellY] = tank->next_;
    }
    add(tank);
}

void Tmpl8::Grid::handleTank(Tank* tank, Tank* other)
{
    while (other != NULL)
    {
        if (tank->active)
        {
            vec2 dir = tank->get_position() - other->get_position();
           
            tank->push(dir.normalized(), 1.f);
        }
       
        other->next_;
    }
    

}


void Tmpl8::Grid::handleCollision()
{
    for (size_t x = 0; x < NUM_CELLS; x++)
    {
        for (size_t y = 0; y < NUM_CELLS; y++)
        {
            handleCell(cells_[x][y]);
        }
    }
}

void Tmpl8::Grid::handleCell(Tank* tank)
{
    while (tank != NULL && tank->active)
    {
        Tank* other = tank->next_;
        while (other != NULL && other->active)
        {
            vec2 dir = tank->get_position() - other->get_position();
            float dir_squared_len = dir.sqr_length();

            float col_squared_len = (tank->get_collision_radius() + other->get_collision_radius());
            col_squared_len *= col_squared_len;


            if (dir_squared_len < col_squared_len)
            {
                tank->push(dir.normalized(), 1.f);
            }
               
            other = other->next_;
        }
        tank = tank->next_;
    }
}
void Tmpl8::Grid::handleCell(int x, int y)
{
    Tank* tank = cells_[x][y];
    while (tank != NULL)
    {
        handleTank(tank, tank->next_);
        if (x > 0 && y > 0)
        {
            handleTank(tank, cells_[x - 1][y - 1]);
        }
        if (x > 0)
        {
            handleTank(tank, cells_[x - 1][y]);
        }
        if (y > 0)
        {
            handleTank(tank, cells_[x][y - 1]);
        }
        if (x > 0 && y < NUM_CELLS - 1)
        {
            handleTank(tank, cells_[x - 1][y + 1]);
        }

        tank = tank->next_;
    }
}

// -----------------------------------------------------------
// When we reach MAX_FRAMES print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void Tmpl8::Game::measure_performance()
{
    char buffer[128];
    if (frame_count >= MAX_FRAMES)
    {
        if (!lock_update)
        {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;
        }

        frame_count--;
    }

    if (lock_update)
    {
        screen->bar(420, 170, 870, 430, 0x030000);
        int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->centre(screen, buffer, 340);
    }
}


// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::tick(float deltaTime)
{
    
    if (!lock_update)
    {
        update(deltaTime);
    }
    draw();

    measure_performance();

    // print something in the graphics window
    //screen->Print("hello world", 2, 2, 0xffffff);

    // print something to the text window
    //cout << "This goes to the console window." << std::endl;

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->print(screen, frame_count_string.c_str(), 350, 580);
}
