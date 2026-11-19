#pragma once
#include "types.h"

class Player {
public:
    Rect pos; // {x, y, 16, 24}
    float vx, vy;
    int hp, maxHp, lives, score;
    Direction dir;
    bool onGround, attacking, hurt, dead;
    int attackTimer, hurtTimer, deathTimer;
    int animFrame, animTimer;
    bool hasSpecial;
    int specialCooldown;
    bool specialAttacking;
    int specialTimer;
    bool jumpHeld;       // for variable-height jump
    bool attackPressed;  // edge detection for attack input
    bool specialPressed; // edge detection for special input

    Player();
    void reset(float startX, float startY);
    void handleInput(const Uint8* keys);
    void update(const std::vector<std::vector<TileType>>& tiles,
                const std::vector<Rect>& movingPlats);
    Rect getAttackHitbox() const;
    void takeDamage(int dmg);
    void render(SDL_Renderer* r, int camX);

private:
    // Internal helpers
    bool isSolid(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const;
    bool isPlatform(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const;
    bool isDangerous(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const;
    void resolveCollisionX(const std::vector<std::vector<TileType>>& tiles);
    void resolveCollisionY(const std::vector<std::vector<TileType>>& tiles);
    void checkHazards(const std::vector<std::vector<TileType>>& tiles);
    void handleMovingPlatforms(const std::vector<Rect>& movingPlats);
    void updateAnimation();
    void drawPlayerSprite(SDL_Renderer* r, int screenX, int screenY);
};
