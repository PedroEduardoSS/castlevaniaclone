#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <functional>

// ── Internal resolution & window scaling ────────────────────────────
const int INTERNAL_W = 320;
const int INTERNAL_H = 240;
const int SCALE      = 3;
const int WINDOW_W   = INTERNAL_W * SCALE;   // 960
const int WINDOW_H   = INTERNAL_H * SCALE;   // 720

// ── Tile / level grid ───────────────────────────────────────────────
const int TILE_SIZE = 16;
const int LEVEL_W   = 200;  // tiles wide
const int LEVEL_H   = 15;   // tiles high

// ── Physics ─────────────────────────────────────────────────────────
const float GRAVITY  = 0.35f;
const float MAX_FALL = 6.0f;

// ── Timing ──────────────────────────────────────────────────────────
const int FPS = 60;

// ── Enums ───────────────────────────────────────────────────────────
enum class TileType {
    AIR         = 0,
    GROUND      = 1,
    PLATFORM    = 2,   // one-way
    SPIKES      = 3,
    LAVA        = 4,
    DOOR        = 5,   // level exit
    MOVING_PLAT = 6,
    DECO        = 7,
    CHAIN       = 8
};

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    LEVEL_COMPLETE,
    GAME_OVER,
    WIN
};

enum class EnemyType {
    SLIME,
    BAT,
    SKELETON,
    FIRE_SPIRIT,
    BOSS
};

enum class Direction {
    LEFT,
    RIGHT
};

// ── Utility structs ─────────────────────────────────────────────────

struct Rect {
    float x, y, w, h;
};

struct Color {
    Uint8 r, g, b, a;
    Color() : r(255), g(255), b(255), a(255) {}
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
        : r(r), g(g), b(b), a(a) {}
};

struct Particle {
    float x, y, vx, vy;
    int   life;
    Uint8 r, g, b;
};

struct TutorialTrigger {
    float       x;
    std::string text;
    bool        shown;
};

// ── Inline helpers ──────────────────────────────────────────────────

inline bool rectsOverlap(const Rect& a, const Rect& b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}
