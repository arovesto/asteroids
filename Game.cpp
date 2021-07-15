#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#include <vector>
#include <string>
#include <algorithm>
#include <string>

#include "Engine.h"
#include "GameObjects.h"
#include "Game.h"

#include <SFML/Audio.hpp>


//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, VK_RETURN)
//
//  schedule_quit_game() - quit game after act()

enum state {
    starting,
    running,
    game_over,
    life_lost,
    new_level   
};

Player p = Player();
UFO u = UFO();
std::vector<Bullet> bullets;
std::vector<Asteroid> asteroids;
std::vector<Animation> animations;

size_t level = 1;
size_t lifes = 3;
state game_state = starting;
size_t score = 0;
size_t score_mark = 10000;

float cooldown = 2;

float ufo_cooldown = 0;

void generate_asteroids() {
    asteroids.clear();
    for (size_t i = 0; i < level * 2 + 3;) {
        Asteroid a = Asteroid(random_in_map(), 9);
        if (!a.collision(p.position())) {
            asteroids.push_back(a);
            i++;
        }
        
    }
};


std::vector<sf::SoundBuffer> buffs;
std::vector<sf::Sound> s(200);


size_t load_sound(std::string sound_name) {
    sf::SoundBuffer b;
    b.loadFromFile(sound_name);
    buffs.push_back(b);
    return buffs.size() - 1;
};

sf::Sound* play_sound(size_t snd_id, bool loop=false) {
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i].getStatus() != s[i].Playing) {
            s[i].setLoop(loop);
            s[i].setBuffer(buffs[snd_id]);
            s[i].setVolume(1);
            s[i].play();
            return &s[i];
        }
    }
    return &s[0];
}

// TODO (arovesto): Refactor current sound system
// load_sound seams ok
// play_sound should not return sf:Sound type but some our type with cancel function
// play_sound should be visible for all members
// method stop all sound required

size_t shoot_sound = 0; 
size_t explosion_sound = 0;
size_t ufo_sound = 0;

// initialize game data in this function
void initialize() {
    shoot_sound = load_sound("/home/arovesto/cpp/asteroids/assets/shoot.wav");
    explosion_sound = load_sound("/home/arovesto/cpp/asteroids/assets/explosion.wav");
    ufo_sound = load_sound("/home/arovesto/cpp/asteroids/assets/ufo.wav");


    srand(time(NULL));
    generate_asteroids();
}

void delete_bullet(size_t id) {
    if (id >= bullets.size() || bullets.size() == 0) {
        return;
    }
    bullets[id] = bullets[bullets.size() - 1];
    bullets.pop_back();
};

void delete_asteroid(size_t id) {
    score += asteroids[id].asteroid_size() * 15;

    if (id >= asteroids.size() || asteroids.size() == 0) {
        return;
    }
    asteroids[id] = asteroids[asteroids.size() - 1];
    asteroids.pop_back();
};

void delete_animation(size_t id) {
    if (id > animations.size() || animations.size() == 0) {
        return;
    }
    animations[id] = animations[animations.size() - 1];
    animations.pop_back();
}

std::vector<size_t> bullets_to_delete;
std::vector<size_t> asteroids_to_delete;
std::vector<Asteroid> to_add;
std::vector<size_t> animation_to_delete;

bool life_lost_now = false;

void loose_life() {
    if (life_lost_now) {
        return;
    }
    play_sound(explosion_sound);
    life_lost_now = true;
    animations.push_back(Animation(p.position(), 50, 2, player, p.get_angle()));
    if (lifes <= 1) {
        game_state = game_over;
        cooldown = 0;
        return;
    }
    lifes--;
    p.move_to_start();
    cooldown = 0;
    game_state = life_lost;
}

sf::Sound* ufo_loop = nullptr;

void ufo_dead() {
    ufo_loop->stop();
    play_sound(explosion_sound);
    animations.push_back(Animation(u.position(), 50, 3, ufo, 0, u.get_factor()));
    u.deactivate();
    ufo_cooldown = 0;
    score += level * 100;
}

void asteroid_dead(size_t j) {
    play_sound(explosion_sound);
    animations.push_back(Animation(asteroids[j].position(), 100, 0.5, 20));
    asteroids_to_delete.push_back(j);
    if (asteroids[j].asteroid_size() >= 6) {
        to_add.push_back(Asteroid(asteroids[j].position(), asteroids[j].asteroid_size() - 3, asteroids[j].velocity()));
        to_add.push_back(Asteroid(asteroids[j].position(), asteroids[j].asteroid_size() - 3, asteroids[j].velocity().mul(-1)));
    }
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt) {
    if (is_key_pressed(VK_ESCAPE)) schedule_quit_game();

    bullets_to_delete.clear();
    asteroids_to_delete.clear();
    to_add.clear();
    animation_to_delete.clear();
    life_lost_now = false;

    switch (game_state) {
    case starting:
        if (is_key_pressed(VK_SPACE)) game_state = running;
        break;
    case game_over:
        cooldown += dt;

        u.act(dt);
        for (Bullet& b : bullets) b.act(dt);
        for (Asteroid& a : asteroids) a.act(dt);
        for (Animation& a : animations) a.act(dt);

        if (is_key_pressed(VK_SPACE) && cooldown > 0.5) {
            p.move_to_start();
            ufo_cooldown = 0;
            cooldown = 0;
            game_state = running;
            level = 1;
            score = 0;
            lifes = 3;
            if (ufo_loop != nullptr) ufo_loop->stop();
            u.deactivate();
            generate_asteroids();
        }
        break;
    case new_level:
    case life_lost:
        if (cooldown > 1) {
            game_state = running;
        }
    case running:
        cooldown += dt;
        ufo_cooldown += dt;
        if (asteroids.size() < 5 && ufo_cooldown > 5 && !u.is_active()) {
            ufo_cooldown = 0;
            cooldown = 0;
            ufo_loop = play_sound(ufo_sound, true);
            u.activate(level);
        }

        if (asteroids.size() == 0) {
            score += 1000;
            level++;
            generate_asteroids();
            game_state = new_level;
            cooldown = 0;
            return;
        }
        if (score >= score_mark) {
            score_mark += 10000;
            if (lifes < 5) lifes++;
        }
        for (size_t i = 0; i < bullets.size(); i++) if (bullets[i].old()) delete_bullet(i);
        for (size_t i = 0; i < animations.size(); i++) if (animations[i].old()) delete_animation(i);


        for (size_t j = 0; j < asteroids.size(); j++) {
            for (size_t i = 0; i < bullets.size(); i++) {
                if (asteroids[j].collision(bullets[i].position())) {
                    bullets_to_delete.push_back(i);
                    asteroid_dead(j);
                }
            }

            if (asteroids[j].collision(p.position()) && cooldown > 1) {
                loose_life();
                asteroid_dead(j);
            }
            if (asteroids[j].position().sub(u.position()).abs_square() < (u.size() + asteroids[j].asteroid_size() * 4) * (u.size() + asteroids[j].asteroid_size() * 4) && u.is_active()) {
                ufo_dead();
                asteroid_dead(j);
            }

        }
        for (size_t i = 0; i < bullets.size(); i++) {
            if (bullets[i].can_shoot_player() && cooldown > 1 && p.position().sub(bullets[i].position()).abs_square() < 225) {
                loose_life();
            }
            if (!bullets[i].can_shoot_player() && u.is_active() && u.position().sub(bullets[i].position()).abs_square() < u.size() * u.size() ) ufo_dead();
        }

        if (p.position().sub(u.position()).abs_square() < (u.size() + 10) * (u.size() + 10) && u.is_active() && cooldown > 1) {
            ufo_dead();
            loose_life();
        }

        for (size_t id : bullets_to_delete) delete_bullet(id);
        for (size_t id : asteroids_to_delete) delete_asteroid(id);
        for (Asteroid& a : to_add) asteroids.push_back(a);

        p.act(dt);
        u.act(dt);
        for (Bullet& b : bullets) b.act(dt);
        for (Asteroid& a : asteroids) a.act(dt);
        for (Animation& a : animations) a.act(dt);

        break;
    default:
        exit(1);
    }
}

// abcdefghijklmnopqrstuvwxyz
Figure letters[26] = {
    Figure({{0,0}, {10, -30}, {20, 0}, {15, -15}, {5, -15}}),
    Figure({{0, 0}, {0, -30}, {20, -30}, {20, -15}, {10, -15}, {20, -15}, {20, 0}, {0, 0}}),
    Figure({{20, 0}, {0, 0}, {0, -30}, {20, -30}}),
    Figure({{0, 0}, {0, -30}, {10, -30}, {20, -20}, {20, -10}, {10, 0}, {0, 0}}),
    Figure({{20,0}, {0, 0}, {0, -20}, {20, -20}, {0, -20}, {0, -30}, {20, -30}}),
    Figure({{0, 0}, {0, -20}, {20, -20}, {0, -20}, {0, -30}, {20, -30}}),
    Figure({{10,-10}, {10, -20}, {20, -20}, {20, 0}, {0, 0}, {0, -20}, {10, -30}, {20, -30}}),
    Figure({{0, 0}, {0, -30}, {0, -15}, {20, -15}, {20, -30}, {20, 0}}),
    Figure({{0, 0}, {20, 0}, {10, 0}, {10, -30}, {0, -30}, {20, -30}}),
    Figure({{0, -10}, {0, 0}, {10, 0}, {20, -10}, {20, -30}, {10, -30}}),
    Figure({{20, 0}, {0, -15}, {0, 0}, {0, -30}, {0, -15}, {20, -30}}),
    Figure({{20, 0}, {0, 0}, {0, -30}}),
    Figure({{0,0}, {0, -30}, {10, -20}, {20, -30}, {20, 0}}),
    Figure({{0, 0}, {0, -30}, {20, 0}, {20, -30}}),
    Figure({{0,0}, {0, -30}, {20, -30}, {20, 0}, {0, 0}}),
    Figure({{0, 0}, {0, -30}, {20, -30}, {20, -20}, {0, -20}}),
    Figure({{0, 0}, {0, -30}, {20, -30}, {20, -10}, {10, 0}, {0, 0}, {10, 0}, {15, -5}, {20, 0}}),
    Figure({{0,0}, {0, -30}, {20, -30}, {20, -20}, {0, -20}, {20, 0}}),
    Figure({{0, 0}, {20, 0}, {20, -20}, {0, -20}, {0, -30}, {20, -30}}),
    Figure({{0, -30}, {20, -30}, {10, -30}, {10, 0}}),
    Figure({{0, -30}, {0, 0}, {20, 0}, {20, -30}}),
    Figure({{0,-30}, {10, 0}, {20, -30}}),
    Figure({{0,-30}, {5, 0}, {10, -30}, {15, 0}, {20, -30}}),
    Figure({{0, 0}, {20, -30}, {10, -15}, {0, -30}, {20, 0}}),
    Figure({{0, -30}, {10, -20}, {20, -30}, {10, -20}, {10, 0}}),
    Figure({{0, -30}, {20, -30}, {0, 0}, {20, 0}}),
};

// 0123456789
Figure numbers[10] = {
    Figure({{0,0}, {0, -30}, {20, -30}, {20, 0}, {0, 0}}),
    Figure({{10, -20}, {20, -30}, {20, 0}}),
    Figure({{0, -30}, {20, -30}, {20, -20}, {0, 0}, {20, 0}}),
    Figure({{0, -30}, {20, -30}, {0, -20}, {20, -20}, {0, 0}}),
    Figure({{0, -30}, {0, -20}, {20, -20}, {20, -30}, {20, 0}}),
    Figure({{0, 0}, {20, 0}, {20, -20}, {0, -20}, {0, -30}, {20, -30}}),
    Figure({{20, -30}, {0, -20}, {0, 0}, {20, 0}, {20, -20}, {0, -20}}),
    Figure({{0, -30}, {20, -30}, {0, 0}}),
    Figure({{0,0}, {0, -30}, {20, -30}, {20, 0}, {0, 0}, {0, -15}, {20, -15}}),
    Figure({{0,0}, {20, -10}, {20, -30}, {0, -30}, {0, -10}, {20, -10}}),
};

Figure letter_of(char b) {
    return letters[b - 'a'];
}

std::vector<Figure> letter_of(std::string a) {
    std::vector<Figure> res;
    for (char l : a) res.push_back(letter_of(l));
    return res;
};

std::vector<Figure> letter_of(size_t n) {
    if (n == 0) return std::vector<Figure> {numbers[0]};

    std::vector<Figure> res;
    for (;n>0; n /= 10) res.push_back(numbers[n % 10]);
    std::reverse(res.begin(), res.end());
    return res;
}

std::vector<Figure> game_over_letters = letter_of("gameover");
std::vector<Figure> start_letters = letter_of("pressspacetostart");
std::vector<Figure> life_lost_letters = letter_of("oneshiplost");
std::vector<Figure> new_level_letters = letter_of("newlevel");
std::vector<Figure> play_again_letters = letter_of("pressspacetoplayagain");

void draw_text(const Vec& at, std::vector<Figure> letters) {
    Vec pos = at;
    for (Figure& l : letters) {
        l.draw(pos, 0);
        pos = pos.add(Vec{30, 0});
    }
}

void draw_lifes() {
    Vec pos = Vec{20, 80};
    for (size_t i = 0; i < lifes; i++) {
        player.draw(pos, 0);
        pos = pos.add(Vec{30, 0});
    }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // clear backbuffer
    memset(buffer, 0x00000000, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    if (game_state == game_over) {
        draw_text(Vec{400, 300}, game_over_letters);
        draw_text(Vec{460, 400}, letter_of(score));
        draw_text(Vec{200, 500}, play_again_letters);
    }

    if (game_state == starting) draw_text(Vec{250, 500}, start_letters);
    if (game_state == life_lost) draw_text(Vec{300, 500}, life_lost_letters);
    if (game_state == new_level) draw_text(Vec{400, 500}, new_level_letters);

    if (game_state != game_over) {
        draw_text(Vec{10, 40}, letter_of(score));
        draw_text(Vec{500, 40}, letter_of(level));
        draw_lifes();
        p.draw();
    }
    u.draw();
    for (Bullet& b : bullets) b.draw();
    for (Asteroid& a : asteroids) a.draw();
    for (Animation& a : animations) a.draw();
}

// free game data in this function
void finalize() {}

void shoot(const Vec& from, const float angle, bool is_player_bullet = true) {
    play_sound(shoot_sound);
    
    Bullet b = Bullet(Vec{700, 0}.rotate(angle), from, is_player_bullet);
    bullets.push_back(b); 
}

Vec get_direction_player(const Vec& from) {
    Vec pos = p.position();
    int player_x = size_t(pos.x) % SCREEN_WIDTH;
    int player_y = size_t(pos.y) % SCREEN_HEIGHT;

    Vec res = Vec{0,0};
    float abs = (SCREEN_HEIGHT + SCREEN_WIDTH) * (SCREEN_WIDTH + SCREEN_HEIGHT);

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            Vec v = Vec{float(player_x + i * SCREEN_WIDTH), float(player_y + j * SCREEN_HEIGHT)};
            if (v.sub(from).abs_square() < abs) {
                abs = v.sub(from).abs_square();
                res = v;
            }
        }
    }
    return res.sub(from);
}