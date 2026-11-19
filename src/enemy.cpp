#include "enemy.h"
#include "renderer.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy(EnemyType t, float x, float y)
    : vx(0.0f), vy(0.0f), type(t), dir(Direction::LEFT),
      alive(true), active(false), animFrame(0), animTimer(0), attackTimer(0),
      bossPhase(1), bossTimer(0), baseY(y), baseX(x), sineCounter(0),
      patrolLeft(x - 64.0f), patrolRight(x + 64.0f), playerInRange(false) {

    pos.x = x;
    pos.y = y;

    switch (type) {
        case EnemyType::SLIME:
            pos.w = 12.0f;
            pos.h = 8.0f;
            hp = maxHp = 1;
            damage = 1;
            vx = -0.5f;
            break;
        case EnemyType::BAT:
            pos.w = 12.0f;
            pos.h = 10.0f;
            hp = maxHp = 1;
            damage = 1;
            vx = -0.8f;
            break;
        case EnemyType::SKELETON:
            pos.w = 14.0f;
            pos.h = 22.0f;
            hp = maxHp = 2;
            damage = 1;
            vx = -0.6f;
            break;
        case EnemyType::FIRE_SPIRIT:
            pos.w = 10.0f;
            pos.h = 14.0f;
            hp = maxHp = 2;
            damage = 1;
            vx = -0.7f;
            break;
        case EnemyType::BOSS:
            pos.w = 28.0f;
            pos.h = 32.0f;
            hp = maxHp = 15;
            damage = 2;
            vx = -0.8f;
            break;
    }
}

bool Enemy::isSolidTile(TileType t) const {
    return t == TileType::GROUND;
}

bool Enemy::isEdgeAhead(const std::vector<std::vector<TileType>>& tiles) const {
    // Check one tile ahead at feet level
    float checkX = (dir == Direction::RIGHT) ? (pos.x + pos.w + 2.0f) : (pos.x - 2.0f);
    float checkY = pos.y + pos.h + 2.0f;

    int tx = (int)(checkX / TILE_SIZE);
    int ty = (int)(checkY / TILE_SIZE);

    if (ty < 0 || ty >= (int)tiles.size() || tx < 0 || tx >= (int)tiles[0].size()) {
        return true; // Edge of world
    }

    // If tile below feet is AIR or hazard, treat as edge
    return tiles[ty][tx] == TileType::AIR || tiles[ty][tx] == TileType::LAVA || tiles[ty][tx] == TileType::SPIKES;
}

void Enemy::applyGravity() {
    vy += GRAVITY;
    if (vy > MAX_FALL) vy = MAX_FALL;
}

void Enemy::doTileCollisionX(const std::vector<std::vector<TileType>>& tiles) {
    pos.x += vx;

    int leftTile   = (int)(pos.x / TILE_SIZE);
    int rightTile  = (int)((pos.x + pos.w - 0.01f) / TILE_SIZE);
    int topTile    = (int)(pos.y / TILE_SIZE);
    int bottomTile = (int)((pos.y + pos.h - 0.01f) / TILE_SIZE);

    int rows = (int)tiles.size();
    int cols = (int)tiles[0].size();

    topTile    = std::max(0, std::min(rows - 1, topTile));
    bottomTile = std::max(0, std::min(rows - 1, bottomTile));

    if (vx > 0.0f) {
        if (rightTile >= 0 && rightTile < cols) {
            for (int y = topTile; y <= bottomTile; ++y) {
                if (isSolidTile(tiles[y][rightTile])) {
                    pos.x = rightTile * TILE_SIZE - pos.w;
                    vx = -vx;
                    dir = Direction::LEFT;
                    break;
                }
            }
        }
    } else if (vx < 0.0f) {
        if (leftTile >= 0 && leftTile < cols) {
            for (int y = topTile; y <= bottomTile; ++y) {
                if (isSolidTile(tiles[y][leftTile])) {
                    pos.x = (leftTile + 1) * TILE_SIZE;
                    vx = -vx;
                    dir = Direction::RIGHT;
                    break;
                }
            }
        }
    }
}

void Enemy::doTileCollisionY(const std::vector<std::vector<TileType>>& tiles) {
    pos.y += vy;

    int leftTile   = (int)(pos.x / TILE_SIZE);
    int rightTile  = (int)((pos.x + pos.w - 0.01f) / TILE_SIZE);
    int topTile    = (int)(pos.y / TILE_SIZE);
    int bottomTile = (int)((pos.y + pos.h - 0.01f) / TILE_SIZE);

    int rows = (int)tiles.size();
    int cols = (int)tiles[0].size();

    leftTile  = std::max(0, std::min(cols - 1, leftTile));
    rightTile = std::max(0, std::min(cols - 1, rightTile));

    if (vy > 0.0f) {
        if (bottomTile >= 0 && bottomTile < rows) {
            for (int x = leftTile; x <= rightTile; ++x) {
                if (isSolidTile(tiles[bottomTile][x]) || tiles[bottomTile][x] == TileType::PLATFORM) {
                    pos.y = bottomTile * TILE_SIZE - pos.h;
                    vy = 0.0f;
                    break;
                }
            }
        }
    } else if (vy < 0.0f) {
        if (topTile >= 0 && topTile < rows) {
            for (int x = leftTile; x <= rightTile; ++x) {
                if (isSolidTile(tiles[topTile][x])) {
                    pos.y = (topTile + 1) * TILE_SIZE;
                    vy = 0.0f;
                    break;
                }
            }
        }
    }
}

void Enemy::fireProjectile(float targetX, float targetY, float speed, int dmg, float projW, float projH) {
    float dx = targetX - (pos.x + pos.w * 0.5f);
    float dy = targetY - (pos.y + pos.h * 0.5f);
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) dist = 1.0f;

    Projectile p;
    p.pos.x = pos.x + pos.w * 0.5f - projW * 0.5f;
    p.pos.y = pos.y + pos.h * 0.5f - projH * 0.5f;
    p.pos.w = projW;
    p.pos.h = projH;
    p.vx = (dx / dist) * speed;
    p.vy = (dy / dist) * speed;
    p.active = true;
    p.damage = dmg;

    projectiles.push_back(p);
}

void Enemy::updateProjectiles(const std::vector<std::vector<TileType>>& tiles) {
    int rows = (int)tiles.size();
    int cols = (int)tiles[0].size();

    for (auto& p : projectiles) {
        if (!p.active) continue;

        p.pos.x += p.vx;
        p.pos.y += p.vy;

        int tx = (int)(p.pos.x / TILE_SIZE);
        int ty = (int)(p.pos.y / TILE_SIZE);

        if (tx < 0 || tx >= cols || ty < 0 || ty >= rows) {
            p.active = false;
        } else if (tiles[ty][tx] == TileType::GROUND) {
            p.active = false;
        }
    }

    // Clean up inactive
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p) { return !p.active; }),
        projectiles.end());
}

// ── Slime Update ───────────────────────────────────────────────────────
void Enemy::updateSlime(const std::vector<std::vector<TileType>>& tiles) {
    applyGravity();
    doTileCollisionY(tiles);

    // Turn around at ledge or patrol bounds
    if (isEdgeAhead(tiles) || pos.x <= patrolLeft || pos.x >= patrolRight) {
        vx = -vx;
        dir = (vx > 0.0f) ? Direction::RIGHT : Direction::LEFT;
    }

    doTileCollisionX(tiles);
}

// ── Bat Update ─────────────────────────────────────────────────────────
void Enemy::updateBat(float playerX, float playerY) {
    (void)playerY;
    sineCounter++;

    // Hover in a smooth sine wave
    pos.y = baseY + std::sin(sineCounter * 0.08f) * 22.0f;

    // Swoop towards player direction
    if (playerX < pos.x - 10.0f) {
        vx = -1.0f;
        dir = Direction::LEFT;
    } else if (playerX > pos.x + 10.0f) {
        vx = 1.0f;
        dir = Direction::RIGHT;
    }
    pos.x += vx;
}

// ── Skeleton Update ────────────────────────────────────────────────────
void Enemy::updateSkeleton(const std::vector<std::vector<TileType>>& tiles, float playerX, float playerY) {
    applyGravity();
    doTileCollisionY(tiles);

    // Face player
    dir = (playerX > pos.x) ? Direction::RIGHT : Direction::LEFT;

    // Patrol back and forth
    if (isEdgeAhead(tiles) || pos.x <= patrolLeft || pos.x >= patrolRight) {
        vx = -vx;
    }
    doTileCollisionX(tiles);

    // Bone throw attack
    attackTimer++;
    if (attackTimer >= 100) {
        attackTimer = 0;
        fireProjectile(playerX, playerY - 8.0f, 2.5f, 1, 6.0f, 6.0f);
    }
}

// ── Fire Spirit Update ─────────────────────────────────────────────────
void Enemy::updateFireSpirit(float playerX, float playerY) {
    sineCounter++;
    pos.x = baseX + std::sin(sineCounter * 0.05f) * 35.0f;
    pos.y = baseY + std::cos(sineCounter * 0.07f) * 12.0f;

    dir = (playerX > pos.x) ? Direction::RIGHT : Direction::LEFT;

    attackTimer++;
    if (attackTimer >= 80) {
        attackTimer = 0;
        fireProjectile(playerX, playerY - 4.0f, 2.8f, 2, 8.0f, 8.0f);
    }
}

// ── Boss Update ────────────────────────────────────────────────────────
void Enemy::updateBoss(const std::vector<std::vector<TileType>>& tiles, float playerX, float playerY) {
    applyGravity();
    doTileCollisionY(tiles);

    // Determine phase from HP
    if (hp > 10) {
        bossPhase = 1;
    } else if (hp > 5) {
        bossPhase = 2;
    } else {
        bossPhase = 3;
    }

    dir = (playerX > pos.x + pos.w * 0.5f) ? Direction::RIGHT : Direction::LEFT;

    bossTimer++;

    if (bossPhase == 1) {
        // Phase 1: Marching dread knight, occasional lunge
        float speed = 0.7f;
        vx = (dir == Direction::RIGHT) ? speed : -speed;
        doTileCollisionX(tiles);

        if (bossTimer >= 120) {
            bossTimer = 0;
            // Charge jump towards player
            vy = -4.5f;
            vx = (dir == Direction::RIGHT) ? 3.0f : -3.0f;
        }
    } else if (bossPhase == 2) {
        // Phase 2: Demon flame barrage
        float speed = 1.0f;
        vx = (dir == Direction::RIGHT) ? speed : -speed;
        doTileCollisionX(tiles);

        if (bossTimer >= 75) {
            bossTimer = 0;
            fireProjectile(playerX, playerY, 3.2f, 2, 8.0f, 8.0f);
        }
    } else {
        // Phase 3: Shadow archon rage (3-way spread projectiles + hyper speed)
        float speed = 1.4f;
        vx = (dir == Direction::RIGHT) ? speed : -speed;
        doTileCollisionX(tiles);

        if (bossTimer >= 50) {
            bossTimer = 0;
            fireProjectile(playerX, playerY, 3.5f, 2, 8.0f, 8.0f);
            fireProjectile(playerX, playerY - 30.0f, 3.2f, 2, 8.0f, 8.0f);
            fireProjectile(playerX, playerY + 30.0f, 3.2f, 2, 8.0f, 8.0f);
        }
    }
}

// ── Main Update ────────────────────────────────────────────────────────
void Enemy::update(const std::vector<std::vector<TileType>>& tiles, float playerX, float playerY) {
    if (!alive) return;

    // Check activation range (within 280px horizontally)
    float dx = std::abs(playerX - pos.x);
    if (type == EnemyType::BOSS) {
        active = true;
    } else {
        active = (dx < 280.0f);
    }

    if (!active) return;

    animTimer++;
    if (animTimer >= 10) {
        animTimer = 0;
        animFrame = (animFrame + 1) % 4;
    }

    switch (type) {
        case EnemyType::SLIME:
            updateSlime(tiles);
            break;
        case EnemyType::BAT:
            updateBat(playerX, playerY);
            break;
        case EnemyType::SKELETON:
            updateSkeleton(tiles, playerX, playerY);
            break;
        case EnemyType::FIRE_SPIRIT:
            updateFireSpirit(playerX, playerY);
            break;
        case EnemyType::BOSS:
            updateBoss(tiles, playerX, playerY);
            break;
    }

    updateProjectiles(tiles);
}

void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) {
        alive = false;
        hp = 0;
    }
}

void Enemy::render(SDL_Renderer* r, int camX) {
    if (!alive) return;

    int sx = (int)pos.x - camX;
    int sy = (int)pos.y;
    if (sx + (int)pos.w < 0 || sx > INTERNAL_W) return;

    switch (type) {
        case EnemyType::SLIME:
            renderSlime(r, sx, sy);
            break;
        case EnemyType::BAT:
            renderBat(r, sx, sy);
            break;
        case EnemyType::SKELETON:
            renderSkeleton(r, sx, sy);
            break;
        case EnemyType::FIRE_SPIRIT:
            renderFireSpirit(r, sx, sy);
            break;
        case EnemyType::BOSS:
            renderBoss(r, sx, sy);
            break;
    }
}

void Enemy::renderProjectiles(SDL_Renderer* r, int camX) {
    for (const auto& p : projectiles) {
        if (!p.active) continue;

        Uint8 pr = 255, pg = 200, pb = 50;
        if (type == EnemyType::FIRE_SPIRIT) {
            pr = 255; pg = 100; pb = 20;
        } else if (type == EnemyType::BOSS) {
            pr = 200; pg = 50; pb = 255;
        } else if (type == EnemyType::SKELETON) {
            pr = 230; pg = 230; pb = 220;
        }

        drawProjectile(r, p.pos.x, p.pos.y, p.pos.w, p.pos.h, pr, pg, pb, camX);
    }
}

void Enemy::renderSlime(SDL_Renderer* r, int screenX, int screenY) {
    drawSlime(r, screenX, screenY, animFrame, 0);
}

void Enemy::renderBat(SDL_Renderer* r, int screenX, int screenY) {
    drawBat(r, screenX, screenY, animFrame, 0);
}

void Enemy::renderSkeleton(SDL_Renderer* r, int screenX, int screenY) {
    drawSkeleton(r, screenX, screenY, dir, animFrame, 0);
}

void Enemy::renderFireSpirit(SDL_Renderer* r, int screenX, int screenY) {
    drawFireSpirit(r, screenX, screenY, animFrame, 0);
}

void Enemy::renderBoss(SDL_Renderer* r, int screenX, int screenY) {
    drawBoss(r, screenX, screenY, dir, animFrame, bossPhase, 0);

    // Boss Health Bar above head
    int barW = 28;
    int barH = 3;
    int barX = screenX;
    int barY = screenY - 6;
    drawPixelRect(r, barX, barY, barW, barH, 40, 10, 10, 200);
    int currentBarW = (barW * hp) / maxHp;
    if (currentBarW > 0) {
        drawPixelRect(r, barX, barY, currentBarW, barH, 220, 30, 30, 255);
    }
}
