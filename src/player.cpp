#include "player.h"

// ============================================================
// Construction / Reset
// ============================================================

Player::Player() {
    pos = {0, 0, 16, 24};
    vx = vy = 0;
    hp = maxHp = 5;
    lives = 3;
    score = 0;
    dir = Direction::RIGHT;
    onGround = false;
    attacking = false;
    hurt = false;
    dead = false;
    attackTimer = 0;
    hurtTimer = 0;
    deathTimer = 0;
    animFrame = 0;
    animTimer = 0;
    hasSpecial = false;
    specialCooldown = 0;
    specialAttacking = false;
    specialTimer = 0;
    jumpHeld = false;
    attackPressed = false;
    specialPressed = false;
}

void Player::reset(float startX, float startY) {
    pos.x = startX;
    pos.y = startY;
    pos.w = 16;
    pos.h = 24;
    vx = vy = 0;
    hp = maxHp;
    dir = Direction::RIGHT;
    onGround = false;
    attacking = false;
    hurt = false;
    dead = false;
    attackTimer = 0;
    hurtTimer = 0;
    deathTimer = 0;
    animFrame = 0;
    animTimer = 0;
    specialCooldown = 0;
    specialAttacking = false;
    specialTimer = 0;
    jumpHeld = false;
    attackPressed = false;
    specialPressed = false;
}

// ============================================================
// Input
// ============================================================

void Player::handleInput(const Uint8* keys) {
    if (dead || hurt) return;

    // --- Horizontal movement with acceleration / deceleration ---
    const float accel = 0.6f;
    const float decel = 0.5f;
    const float maxSpeed = 2.0f;

    bool moveLeft  = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
    bool moveRight = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];

    if (moveLeft && !moveRight) {
        vx -= accel;
        if (vx < -maxSpeed) vx = -maxSpeed;
        dir = Direction::LEFT;
    } else if (moveRight && !moveLeft) {
        vx += accel;
        if (vx > maxSpeed) vx = maxSpeed;
        dir = Direction::RIGHT;
    } else {
        // Decelerate toward zero
        if (vx > 0) {
            vx -= decel;
            if (vx < 0) vx = 0;
        } else if (vx < 0) {
            vx += decel;
            if (vx > 0) vx = 0;
        }
    }

    // --- Jump (variable height) ---
    bool jumpKey = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_SPACE];
    if (jumpKey && onGround && !jumpHeld) {
        vy = -6.5f;
        onGround = false;
        jumpHeld = true;
    }
    if (!jumpKey) {
        jumpHeld = false;
        // Cut the jump short when button released early
        if (vy < -2.0f) {
            vy = -2.0f;
        }
    }

    // --- Normal attack (edge-triggered) ---
    bool atkKey = keys[SDL_SCANCODE_J] || keys[SDL_SCANCODE_Z];
    if (atkKey && !attackPressed && !attacking && !specialAttacking) {
        attacking = true;
        attackTimer = 15;
    }
    attackPressed = atkKey;

    // --- Special attack (edge-triggered) ---
    bool specKey = keys[SDL_SCANCODE_K] || keys[SDL_SCANCODE_X];
    if (specKey && !specialPressed && hasSpecial && specialCooldown <= 0
        && !attacking && !specialAttacking) {
        specialAttacking = true;
        specialTimer = 20;        // slightly longer animation
        specialCooldown = 120;    // cooldown frames
    }
    specialPressed = specKey;
}

// ============================================================
// Tile helpers
// ============================================================

bool Player::isSolid(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const {
    if (tx < 0 || tx >= LEVEL_W || ty < 0 || ty >= LEVEL_H) return false;
    return tiles[ty][tx] == TileType::GROUND;
}

bool Player::isPlatform(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const {
    if (tx < 0 || tx >= LEVEL_W || ty < 0 || ty >= LEVEL_H) return false;
    return tiles[ty][tx] == TileType::PLATFORM;
}

bool Player::isDangerous(const std::vector<std::vector<TileType>>& tiles, int tx, int ty) const {
    if (tx < 0 || tx >= LEVEL_W || ty < 0 || ty >= LEVEL_H) return false;
    return tiles[ty][tx] == TileType::SPIKES || tiles[ty][tx] == TileType::LAVA;
}

// ============================================================
// Collision resolution
// ============================================================

void Player::resolveCollisionX(const std::vector<std::vector<TileType>>& tiles) {
    // Determine the tile range the player overlaps after moving on X
    int left   = (int)(pos.x) / TILE_SIZE;
    int right  = (int)(pos.x + pos.w - 1) / TILE_SIZE;
    int top    = (int)(pos.y) / TILE_SIZE;
    int bottom = (int)(pos.y + pos.h - 1) / TILE_SIZE;

    for (int ty = top; ty <= bottom; ++ty) {
        for (int tx = left; tx <= right; ++tx) {
            if (!isSolid(tiles, tx, ty)) continue;

            Rect tileRect = {(float)(tx * TILE_SIZE), (float)(ty * TILE_SIZE),
                             (float)TILE_SIZE, (float)TILE_SIZE};
            if (rectsOverlap(pos, tileRect)) {
                if (vx > 0) {
                    // Moving right → push left
                    pos.x = tileRect.x - pos.w;
                } else if (vx < 0) {
                    // Moving left → push right
                    pos.x = tileRect.x + tileRect.w;
                }
                vx = 0;
            }
        }
    }
}

void Player::resolveCollisionY(const std::vector<std::vector<TileType>>& tiles) {
    int left   = (int)(pos.x) / TILE_SIZE;
    int right  = (int)(pos.x + pos.w - 1) / TILE_SIZE;
    int top    = (int)(pos.y) / TILE_SIZE;
    int bottom = (int)(pos.y + pos.h - 1) / TILE_SIZE;

    for (int ty = top; ty <= bottom; ++ty) {
        for (int tx = left; tx <= right; ++tx) {
            bool solid = isSolid(tiles, tx, ty);
            bool plat  = isPlatform(tiles, tx, ty);

            if (!solid && !plat) continue;

            Rect tileRect = {(float)(tx * TILE_SIZE), (float)(ty * TILE_SIZE),
                             (float)TILE_SIZE, (float)TILE_SIZE};
            if (!rectsOverlap(pos, tileRect)) continue;

            if (plat) {
                // One-way platform: only collide when falling down and
                // feet were above the platform top last frame.
                if (vy > 0) {
                    float prevBottom = (pos.y + pos.h) - vy;
                    if (prevBottom <= tileRect.y + 2) { // small epsilon
                        pos.y = tileRect.y - pos.h;
                        vy = 0;
                        onGround = true;
                    }
                }
                continue;
            }

            // Solid ground tile
            if (vy > 0) {
                pos.y = tileRect.y - pos.h;
                vy = 0;
                onGround = true;
            } else if (vy < 0) {
                pos.y = tileRect.y + tileRect.h;
                vy = 0;
            }
        }
    }
}

void Player::checkHazards(const std::vector<std::vector<TileType>>& tiles) {
    int left   = (int)(pos.x) / TILE_SIZE;
    int right  = (int)(pos.x + pos.w - 1) / TILE_SIZE;
    int top    = (int)(pos.y) / TILE_SIZE;
    int bottom = (int)(pos.y + pos.h - 1) / TILE_SIZE;

    for (int ty = top; ty <= bottom; ++ty) {
        for (int tx = left; tx <= right; ++tx) {
            if (!isDangerous(tiles, tx, ty)) continue;
            Rect tileRect = {(float)(tx * TILE_SIZE), (float)(ty * TILE_SIZE),
                             (float)TILE_SIZE, (float)TILE_SIZE};
            if (rectsOverlap(pos, tileRect)) {
                if (tiles[ty][tx] == TileType::LAVA) {
                    // Instant kill
                    takeDamage(hp);
                } else {
                    // Spikes deal 1 damage
                    takeDamage(1);
                }
                return; // only one hit per frame
            }
        }
    }
}

// ============================================================
// Moving platforms
// ============================================================

void Player::handleMovingPlatforms(const std::vector<Rect>& movingPlats) {
    // Check if the player is standing on any moving platform
    for (const auto& mp : movingPlats) {
        // Player's feet are at pos.y + pos.h
        // Check if feet are right at the top of the platform
        Rect feetRect = {pos.x, pos.y + pos.h, pos.w, 2.0f};
        if (rectsOverlap(feetRect, mp)) {
            if (vy >= 0) {
                pos.y = mp.y - pos.h;
                vy = 0;
                onGround = true;
            }
        }
    }
}

// ============================================================
// Attack hitbox
// ============================================================

Rect Player::getAttackHitbox() const {
    if (!attacking && !specialAttacking) {
        return {0, 0, 0, 0};
    }

    float atkW = specialAttacking ? 20.0f : 14.0f;
    float atkH = 20.0f;
    float atkY = pos.y + (pos.h - atkH) / 2.0f;
    float atkX;

    if (dir == Direction::RIGHT) {
        atkX = pos.x + pos.w;
    } else {
        atkX = pos.x - atkW;
    }

    return {atkX, atkY, atkW, atkH};
}

// ============================================================
// Damage / Death
// ============================================================

void Player::takeDamage(int dmg) {
    if (hurt || dead || hurtTimer > 0) return;

    hp -= dmg;
    if (hp <= 0) {
        hp = 0;
        dead = true;
        deathTimer = 30;
        vy = -4.0f; // little hop on death
        return;
    }

    hurt = true;
    hurtTimer = 30; // 0.5 seconds of invulnerability (30 frames at 60 FPS)

    // Knockback: push away from last facing direction
    vx = (dir == Direction::RIGHT) ? -3.0f : 3.0f;
    vy = -2.5f;
}

// ============================================================
// Animation
// ============================================================

void Player::updateAnimation() {
    animTimer++;

    if (dead) {
        // Death animation: single frame, handled by deathTimer
        animFrame = 0;
        return;
    }

    if (attacking || specialAttacking) {
        // 3 attack frames spread across attack duration
        int totalFrames = attacking ? 15 : 20;
        int currentTimer = attacking ? attackTimer : specialTimer;
        int elapsed = totalFrames - currentTimer;
        if (elapsed < totalFrames / 3)       animFrame = 0;
        else if (elapsed < 2 * totalFrames / 3) animFrame = 1;
        else                                    animFrame = 2;
        return;
    }

    if (hurt) {
        animFrame = 0;
        return;
    }

    if (!onGround) {
        // Jump frame
        animFrame = 0;
        return;
    }

    // Run animation: 2 frames at 8-frame intervals
    if (std::abs(vx) > 0.1f) {
        if (animTimer % 8 == 0) {
            animFrame = (animFrame + 1) % 2;
        }
    } else {
        // Idle
        animFrame = 0;
    }
}

// ============================================================
// Update (called once per frame)
// ============================================================

void Player::update(const std::vector<std::vector<TileType>>& tiles,
                    const std::vector<Rect>& movingPlats) {
    // --- Death sequence ---
    if (dead) {
        deathTimer--;
        vy += GRAVITY;
        if (vy > MAX_FALL) vy = MAX_FALL;
        pos.y += vy;
        if (deathTimer <= 0) {
            lives--;
            if (lives > 0) {
                dead = false;
                // The game class will call reset() with the spawn position
            }
            // If lives <= 0 the game class handles GAME_OVER state
        }
        updateAnimation();
        return;
    }

    // --- Hurt / invulnerability timer ---
    if (hurt) {
        hurtTimer--;
        if (hurtTimer <= 0) {
            hurt = false;
            hurtTimer = 0;
        }
    }

    // --- Attack timers ---
    if (attacking) {
        attackTimer--;
        if (attackTimer <= 0) {
            attacking = false;
            attackTimer = 0;
        }
    }

    if (specialAttacking) {
        specialTimer--;
        if (specialTimer <= 0) {
            specialAttacking = false;
            specialTimer = 0;
        }
    }

    if (specialCooldown > 0) {
        specialCooldown--;
    }

    // --- Apply gravity ---
    vy += GRAVITY;
    if (vy > MAX_FALL) vy = MAX_FALL;

    // --- Move on X axis, then resolve ---
    pos.x += vx;
    resolveCollisionX(tiles);

    // --- Move on Y axis, then resolve ---
    onGround = false;
    pos.y += vy;
    resolveCollisionY(tiles);

    // --- Moving platforms ---
    handleMovingPlatforms(movingPlats);

    // --- Check spikes / lava ---
    if (!hurt && !dead) {
        checkHazards(tiles);
    }

    // --- Clamp to level bounds ---
    if (pos.x < 0) { pos.x = 0; vx = 0; }
    if (pos.x + pos.w > LEVEL_W * TILE_SIZE) {
        pos.x = LEVEL_W * TILE_SIZE - pos.w;
        vx = 0;
    }

    // --- Fall into pit = death ---
    if (pos.y > LEVEL_H * TILE_SIZE + 32) {
        takeDamage(hp);
    }

    // --- Animation ---
    updateAnimation();
}

// ============================================================
// Rendering
// ============================================================

void Player::render(SDL_Renderer* r, int camX) {
    // Don't render if off-screen
    int screenX = (int)pos.x - camX;
    int screenY = (int)pos.y;
    if (screenX + (int)pos.w < 0 || screenX > INTERNAL_W) return;

    // Flashing when hurt (skip render every other frame)
    if (hurt && (hurtTimer % 4 < 2)) return;

    drawPlayerSprite(r, screenX, screenY);
}

void Player::drawPlayerSprite(SDL_Renderer* r, int screenX, int screenY) {
    bool facingRight = (dir == Direction::RIGHT);
    SDL_Rect rect;

    // --- Cape (drawn behind the body) ---
    // Cape: 6×14, offset from top of player body
    {
        int capeW = 6;
        int capeH = 14;
        int capeOffsetY = 6;
        int capeX = facingRight ? screenX - 2 : screenX + (int)pos.w - capeW + 2;
        int capeY = screenY + capeOffsetY;

        // Cape billows slightly during run
        if (onGround && std::abs(vx) > 0.1f) {
            capeX += facingRight ? -(animFrame) : (animFrame);
        }

        SDL_SetRenderDrawColor(r, 180, 30, 30, 255); // red cape
        rect = {capeX, capeY, capeW, capeH};
        SDL_RenderFillRect(r, &rect);
    }

    // --- Body (torso + legs) ---
    // Body: 10×14 centered in the 16-wide hitbox
    {
        int bodyW = 10;
        int bodyH = 14;
        int bodyX = screenX + ((int)pos.w - bodyW) / 2;
        int bodyY = screenY + 4;

        SDL_SetRenderDrawColor(r, 60, 60, 180, 255); // blue armor
        rect = {bodyX, bodyY, bodyW, bodyH};
        SDL_RenderFillRect(r, &rect);

        // Legs: two 4×6 rectangles
        int legW = 4;
        int legH = 6;
        int legY = screenY + 18;

        int legSpread = 0;
        if (onGround && std::abs(vx) > 0.1f) {
            legSpread = (animFrame == 0) ? -1 : 1;
        }
        if (!onGround) {
            legSpread = 1; // legs apart in air
        }

        int leftLegX  = bodyX + 1 - legSpread;
        int rightLegX  = bodyX + bodyW - legW - 1 + legSpread;

        SDL_SetRenderDrawColor(r, 50, 50, 150, 255); // darker blue for legs
        rect = {leftLegX, legY, legW, legH};
        SDL_RenderFillRect(r, &rect);
        rect = {rightLegX, legY, legW, legH};
        SDL_RenderFillRect(r, &rect);
    }

    // --- Head ---
    // Head: 8×7 centered
    {
        int headW = 8;
        int headH = 7;
        int headX = screenX + ((int)pos.w - headW) / 2;
        int headY = screenY;

        // Helmet (blue)
        SDL_SetRenderDrawColor(r, 60, 60, 180, 255);
        rect = {headX, headY, headW, headH - 2};
        SDL_RenderFillRect(r, &rect);

        // Visor / face (skin tone)
        SDL_SetRenderDrawColor(r, 210, 170, 130, 255);
        int visorX = facingRight ? headX + 3 : headX + 1;
        rect = {visorX, headY + 2, 4, 3};
        SDL_RenderFillRect(r, &rect);

        // Eyes
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        int eyeX = facingRight ? headX + 5 : headX + 2;
        rect = {eyeX, headY + 3, 2, 1};
        SDL_RenderFillRect(r, &rect);
    }

    // --- Sword / Attack ---
    if (attacking || specialAttacking) {
        // Sword drawn according to attack frame
        int swordLen = specialAttacking ? 18 : 12;
        int swordH = specialAttacking ? 4 : 3;
        int swordY = screenY + 8 + (int)pos.h / 2 - 12;

        // Attack frames: 0 = wind up, 1 = swing, 2 = follow through
        int swordOffsetY = 0;
        if (animFrame == 0) {
            swordOffsetY = -3;    // raised
            swordLen -= 2;
        } else if (animFrame == 2) {
            swordOffsetY = 3;     // low follow-through
            swordLen -= 1;
        }

        int swordX;
        if (facingRight) {
            swordX = screenX + (int)pos.w;
        } else {
            swordX = screenX - swordLen;
        }

        // Blade (silver)
        SDL_SetRenderDrawColor(r, 200, 200, 220, 255);
        rect = {swordX, swordY + swordOffsetY, swordLen, swordH};
        SDL_RenderFillRect(r, &rect);

        // Guard / handle highlight
        int guardX = facingRight ? swordX : swordX + swordLen - 2;
        SDL_SetRenderDrawColor(r, 160, 140, 60, 255); // gold guard
        rect = {guardX, swordY + swordOffsetY - 1, 2, swordH + 2};
        SDL_RenderFillRect(r, &rect);

        // Special attack glow effect
        if (specialAttacking) {
            SDL_SetRenderDrawColor(r, 100, 180, 255, 128); // blue glow
            rect = {swordX - 1, swordY + swordOffsetY - 1, swordLen + 2, swordH + 2};
            SDL_RenderFillRect(r, &rect);
        }
    } else {
        // Idle / running: small sword at side
        int swordLen = 8;
        int swordH = 2;
        int swordY = screenY + 10;
        int swordX;
        if (facingRight) {
            swordX = screenX + (int)pos.w - 2;
        } else {
            swordX = screenX - swordLen + 2;
        }
        SDL_SetRenderDrawColor(r, 200, 200, 220, 255);
        rect = {swordX, swordY, swordLen, swordH};
        SDL_RenderFillRect(r, &rect);
    }

    // --- Death overlay: player falls and turns gray ---
    if (dead) {
        // Gray overlay
        SDL_SetRenderDrawColor(r, 100, 100, 100, 180);
        rect = {screenX, screenY, (int)pos.w, (int)pos.h};
        SDL_RenderFillRect(r, &rect);
    }
}
