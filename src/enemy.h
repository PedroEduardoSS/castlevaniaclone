#pragma once
#include "types.h"

class Enemy {
public:
    Rect pos;
    float vx, vy;
    int hp, maxHp, damage;
    EnemyType type;
    Direction dir;
    bool alive, active;
    int animFrame, animTimer;
    int attackTimer;

    // Projectile for skeleton/fire spirit/boss
    struct Projectile {
        Rect pos;
        float vx, vy;
        bool active;
        int damage;
    };
    std::vector<Projectile> projectiles;

    // Boss phase
    int bossPhase;
    int bossTimer;

    // Sine wave tracking for bat / fire spirit
    float baseY;      // anchor Y for bat sine wave
    float baseX;      // anchor X for fire spirit sine wave
    int sineCounter;   // frame counter for sine wave

    // Patrol boundaries
    float patrolLeft, patrolRight;

    // Activation distance
    bool playerInRange;

    Enemy(EnemyType t, float x, float y);

    void update(const std::vector<std::vector<TileType>>& tiles,
                float playerX, float playerY);
    void takeDamage(int dmg);
    void render(SDL_Renderer* r, int camX);
    void renderProjectiles(SDL_Renderer* r, int camX);

private:
    // Per-type update helpers
    void updateSlime(const std::vector<std::vector<TileType>>& tiles);
    void updateBat(float playerX, float playerY);
    void updateSkeleton(const std::vector<std::vector<TileType>>& tiles,
                        float playerX, float playerY);
    void updateFireSpirit(float playerX, float playerY);
    void updateBoss(const std::vector<std::vector<TileType>>& tiles,
                    float playerX, float playerY);

    // Shared helpers
    void applyGravity();
    void doTileCollisionX(const std::vector<std::vector<TileType>>& tiles);
    void doTileCollisionY(const std::vector<std::vector<TileType>>& tiles);
    bool isSolidTile(TileType t) const;
    bool isEdgeAhead(const std::vector<std::vector<TileType>>& tiles) const;
    void updateProjectiles(const std::vector<std::vector<TileType>>& tiles);
    void fireProjectile(float targetX, float targetY, float speed, int dmg,
                        float projW, float projH);

    // Per-type render helpers
    void renderSlime(SDL_Renderer* r, int screenX, int screenY);
    void renderBat(SDL_Renderer* r, int screenX, int screenY);
    void renderSkeleton(SDL_Renderer* r, int screenX, int screenY);
    void renderFireSpirit(SDL_Renderer* r, int screenX, int screenY);
    void renderBoss(SDL_Renderer* r, int screenX, int screenY);
};
