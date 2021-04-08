#pragma once


namespace Tmpl8
{

enum allignments
{
    BLUE,
    RED
};

class Tank
{
    friend class Grid;
  public:
      Tank(float pos_x, float pos_y, allignments allignment, Sprite* tank_sprite, Sprite* smoke_sprite, float tar_x, float tar_y, float collision_radius, int health, float max_speed, Grid* grid);
      ~Tank();

    void tick();

    vec2 get_position() const { return position; };
    float get_collision_radius() const { return collision_radius; };
    bool rocket_reloaded() const { return reloaded; };

    void reload_rocket();
    void move(float x, float y);
    void deactivate();
    bool hit(int hit_value);

    void draw(Surface* screen);

    int compare_health(const Tank& other) const;

    void push(vec2 direction, float magnitude);

    vec2 position;
    vec2 speed;
    vec2 target;

    int health;

    float collision_radius;
    vec2 force;

    float max_speed;
    float reload_time;

    bool reloaded;
    bool active;

    allignments allignment;

    int current_frame;
    Sprite* tank_sprite;
    Sprite* smoke_sprite;
    Tank* prev_;
    Tank* next_;
    Grid* grid_;
    float x_;
    float y_;
private:
   
    
};

} // namespace Tmpl8