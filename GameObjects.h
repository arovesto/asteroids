#pragma once

#include "Engine.h"
#include "Figure.h"
#include "Game.h"


float player_shoot_cooldown = 0.2;

Figure player = Figure({Vec{-10, 15}, Vec{0, -15}, Vec{10, 15}, Vec{8, 10}, Vec{-8, 10}, Vec{-10, 15}});
Figure plum = Figure({Vec{-4, 10}, Vec{0, 15}, Vec{4, 10}});
Vec player_shoot_displacement = {0, -15};

class Player {
public:
    Player() = default;
    void draw();
    void act(float dt);
    Vec& position() { return place; };
    float get_angle() { return angle; };
    void move_to_start() {place = Vec{500,300}; speed = Vec{0, 0}; angle = 0; };
private:
    float since_last_shot = 0;
    bool moving = false;
    Vec speed = Vec{};
    Vec place = Vec{500,300};
    float angle = 0;
    Vec acc = Vec{};
};

void Player::draw() {
    player.draw(place, angle);
    if (moving) {
        plum.draw(place, angle);
    }
}

void Player::act(float dt) {
    if (is_key_pressed(VK_RIGHT)) {
        angle += 5 * dt;
    }
    if (is_key_pressed(VK_LEFT)) {
        angle -= 5 * dt;   
    }

    if (is_key_pressed(VK_SPACE)) {
        if (since_last_shot > player_shoot_cooldown) {
            since_last_shot = 0;
            shoot(place.add(player_shoot_displacement.rotate(angle)), angle - M_PI / 2, true);
        }
    }

    if (is_key_pressed(VK_UP)) {
        acc = Vec{0,-400}.rotate(angle);
        moving = true;
    } else {
        moving = false;
        acc = speed.mul(-0.1);
    }
    speed = speed.add(acc.mul(dt)).clamp(700);
    place = place.add(speed.mul(dt));

    since_last_shot += dt;
};

Figure asteroid_types[4] = {
    Figure({Vec{0, 2}, Vec{2, 4}, Vec{4, 2}, Vec{3, 0}, Vec{4, -2}, Vec{1, -4}, Vec{-4, -2}, Vec{-4, 2}, Vec{-2, 4}, Vec{0, 2}}),
    Figure({Vec{2, 1}, Vec{4, 2}, Vec{2, 4}, Vec{0, 3}, Vec{-2, 4}, Vec{-4, 2}, Vec{-3, 0}, Vec{-4, -2}, Vec{-2, -4}, Vec{-1, -3}, Vec{2, -4}, Vec{4, -1}, Vec{2, 1}}),
    Figure({Vec{-2, 0}, Vec{-4, -1}, Vec{-2, -4}, Vec{0, -1}, Vec{0, -4}, Vec{2, -4}, Vec{4, -1}, Vec{4, 1}, Vec{2, 4}, Vec{-1, 4}, Vec{-4, 1}, Vec{-2, 0}}),
    Figure({Vec{1, 0}, Vec{4, 1}, Vec{4, 2}, Vec{1, 4}, Vec{-2, 4}, Vec{-1, 2}, Vec{-4, 2}, Vec{-4, -1}, Vec{-2, -4}, Vec{1, -3}, Vec{2, -4}, Vec{4, -2}, Vec{1, 0}})
};

class Asteroid {
public:
    Asteroid(const Vec& pos, const size_t size, const Vec& base_speed = Vec{0, 0}) : pos(pos), size(size) {
        tp = rand() % 4;
        vel = random_vec(200).add(base_speed);
        angle_rot = random_vec(20).x;
    };
    void draw();
    void act(float dt);
    bool collision(const Vec& p) {
        return pos.sub(p).abs_square() < (size * 4 + 10) * (size * 4 + 10);
    }
    Vec& position() { return pos; };
    Vec& velocity() { return vel; };
    size_t asteroid_size() { return size; };
private:
    size_t tp = 0;
    Vec pos;
    Vec vel = Vec{0,0}; 
    float angle = 0;
    float angle_rot = 0;
    size_t size;
};

void Asteroid::draw() {
    asteroid_types[tp].draw(pos, angle, size);
}

void Asteroid::act(float dt) {
    pos = pos.add(vel.mul(dt));
    angle += angle_rot * dt;
};

class Bullet {
public:
    Bullet(const Vec& speed, const Vec& pos, bool is_player_bullet): speed(speed), pos(pos), is_player_bullet(is_player_bullet) {};
    void draw();
    void act(float dt);
    Vec& position() { return pos; };
    bool old() { return age > 1; };
    bool can_shoot_player() { return !is_player_bullet; };
private:
    bool is_player_bullet;
    Vec speed;
    Vec pos;
    float age = 0;
};

Figure bullet = Figure({Vec{0, 0}, Vec{1,1}});

void Bullet::draw() {
    bullet.draw(pos, 0);
}

void Bullet::act(float dt) {
    age += dt;
    pos = pos.add(speed.mul(dt));
}

float get_ufo_factor(size_t lvl) {
    float factor = 0.5;
    if (lvl < 7) {
        factor = 1;
    }
    if (lvl < 4) {
        factor = 1.5;
    }
    return factor;
}

class UFO {
public:
    UFO() = default;
    void draw();
    void act(float dt);
    Vec& position() { return pos; };
    void activate(size_t l) { active = true; vel = Vec{0,0}; pos = random_in_map(); lvl = l; shoot_cooldown = 0; factor = get_ufo_factor(l); };
    void deactivate() { active = false; };
    bool is_active() { return active; };
    float size() { return 30 * factor; };
    float get_factor() { return factor; }; 
private:
    Vec pos = Vec{0, 0};
    Vec vel = Vec{0, 0};
    bool active = false;
    size_t lvl = 1;
    float factor = 1;
    float shoot_cooldown = 0;
};

Figure ufo = Figure({{30, 0}, {10, 20}, {-10, 20}, {-30, 0}, {30, 0}, {10, -10}, {5, -20}, {-5, -20}, {-10, -10}, {10, -10}, {-10, -10}, {-30, 0}});

void UFO::draw() {
    if (!active) {
        return;
    }
    
    ufo.draw(pos, 0, factor);
}

void UFO::act(float dt) {
    if (!active) {
        return;
    }
    shoot_cooldown += dt;

    Vec p = get_direction_player(pos);
    if (p.abs_square() < 400*400) {
        if (shoot_cooldown > 2) {
            shoot_cooldown = 0;
            shoot(pos, p.angle() + random_vec(1).x / (1 + lvl / 2), false);
        }
        vel = Vec{0, 0};
    } else {
        vel = p.norm().mul(100 + 50 * lvl);
    }
    pos = pos.add(vel.mul(dt));
}

class Animation {
public: 
    Animation(const Vec& where, float intencity, float time, const size_t points) : time(time) {
        for (size_t i = 0; i < points; i++) {
            particles.push_back(Figure({{0, 0}, {0, 1}}));
            positions.push_back(Vec{where.x, where.y});
            speeds.push_back(random_vec(intencity));
        }
    };
    Animation(const Vec& where, float intencity, float time, Figure& f, float angle, float factor = 1) : angle(angle), time(time), factor(factor) {
        std::vector<Vec> points = f.get_points();
        for (size_t i = 1; i < points.size(); i++) {
            particles.push_back(Figure({points[i - 1], points[i + 1]}));
            positions.push_back(Vec{where.x, where.y});
            speeds.push_back(random_vec(intencity));
        }
    };
    void draw();
    void act(float dt);
    bool old() { return counter > time; };
private:
    std::vector<Figure> particles;
    std::vector<Vec> positions;
    std::vector<Vec> speeds;
    float time;
    float counter = 0;
    float angle = 0;
    float factor = 1;
};

void Animation::draw() {
    if (old()) {
        return;
    }
    for (size_t i = 0; i < particles.size(); i++) particles[i].draw(positions[i], angle, factor);
}

void Animation::act(float dt) {
    if (old()) {
        return;
    }
    counter += dt;
    for (size_t i = 0; i < particles.size(); i++) positions[i] = positions[i].add(speeds[i].mul(dt));
}