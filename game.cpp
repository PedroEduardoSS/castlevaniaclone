#include "game.h"
#include "renderer.h"
#include <cstdio>
#include <cstring>
#include <cmath>

// ── 5x7 Pixel Font Bitmap Data ─────────────────────────────────────────
// Each character is 5 columns wide, where each byte represents 7 vertical pixels
// Supports ASCII 32 (' ') through 90 ('Z')
const unsigned char Game::FONT_DATA[][5] = {
    // 32 ' '
    {0x00, 0x00, 0x00, 0x00, 0x00},
    // 33 '!'
    {0x00, 0x00, 0x5F, 0x00, 0x00},
    // 34 '"'
    {0x00, 0x07, 0x00, 0x07, 0x00},
    // 35 '#'
    {0x14, 0x7F, 0x14, 0x7F, 0x14},
    // 36 '$'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12},
    // 37 '%'
    {0x23, 0x13, 0x08, 0x64, 0x62},
    // 38 '&'
    {0x36, 0x49, 0x55, 0x22, 0x50},
    // 39 '\''
    {0x00, 0x05, 0x03, 0x00, 0x00},
    // 40 '('
    {0x00, 0x1C, 0x22, 0x41, 0x00},
    // 41 ')'
    {0x00, 0x41, 0x22, 0x1C, 0x00},
    // 42 '*'
    {0x14, 0x08, 0x3E, 0x08, 0x14},
    // 43 '+'
    {0x08, 0x08, 0x3E, 0x08, 0x08},
    // 44 ','
    {0x00, 0x50, 0x30, 0x00, 0x00},
    // 45 '-'
    {0x08, 0x08, 0x08, 0x08, 0x08},
    // 46 '.'
    {0x00, 0x60, 0x60, 0x00, 0x00},
    // 47 '/'
    {0x20, 0x10, 0x08, 0x04, 0x02},
    // 48 '0'
    {0x3E, 0x51, 0x49, 0x45, 0x3E},
    // 49 '1'
    {0x00, 0x42, 0x7F, 0x40, 0x00},
    // 50 '2'
    {0x42, 0x61, 0x51, 0x49, 0x46},
    // 51 '3'
    {0x21, 0x41, 0x45, 0x4B, 0x31},
    // 52 '4'
    {0x18, 0x14, 0x12, 0x7F, 0x10},
    // 53 '5'
    {0x27, 0x45, 0x45, 0x45, 0x39},
    // 54 '6'
    {0x3C, 0x4A, 0x49, 0x49, 0x30},
    // 55 '7'
    {0x01, 0x71, 0x09, 0x05, 0x03},
    // 56 '8'
    {0x36, 0x49, 0x49, 0x49, 0x36},
    // 57 '9'
    {0x06, 0x49, 0x49, 0x29, 0x1E},
    // 58 ':'
    {0x00, 0x36, 0x36, 0x00, 0x00},
    // 59 ';'
    {0x00, 0x56, 0x36, 0x00, 0x00},
    // 60 '<'
    {0x08, 0x14, 0x22, 0x41, 0x00},
    // 61 '='
    {0x14, 0x14, 0x14, 0x14, 0x14},
    // 62 '>'
    {0x00, 0x41, 0x22, 0x14, 0x08},
    // 63 '?'
    {0x02, 0x01, 0x51, 0x09, 0x06},
    // 64 '@'
    {0x32, 0x49, 0x79, 0x41, 0x3E},
    // 65 'A'
    {0x7E, 0x11, 0x11, 0x11, 0x7E},
    // 66 'B'
    {0x7F, 0x49, 0x49, 0x49, 0x36},
    // 67 'C'
    {0x3E, 0x41, 0x41, 0x41, 0x22},
    // 68 'D'
    {0x7F, 0x41, 0x41, 0x22, 0x1C},
    // 69 'E'
    {0x7F, 0x49, 0x49, 0x49, 0x41},
    // 70 'F'
    {0x7F, 0x09, 0x09, 0x09, 0x01},
    // 71 'G'
    {0x3E, 0x41, 0x49, 0x49, 0x7A},
    // 72 'H'
    {0x7F, 0x08, 0x08, 0x08, 0x7F},
    // 73 'I'
    {0x00, 0x41, 0x7F, 0x41, 0x00},
    // 74 'J'
    {0x20, 0x40, 0x41, 0x3F, 0x01},
    // 75 'K'
    {0x7F, 0x08, 0x14, 0x22, 0x41},
    // 76 'L'
    {0x7F, 0x40, 0x40, 0x40, 0x40},
    // 77 'M'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    // 78 'N'
    {0x7F, 0x04, 0x08, 0x10, 0x7F},
    // 79 'O'
    {0x3E, 0x41, 0x41, 0x41, 0x3E},
    // 80 'P'
    {0x7F, 0x09, 0x09, 0x09, 0x06},
    // 81 'Q'
    {0x3E, 0x41, 0x51, 0x21, 0x5E},
    // 82 'R'
    {0x7F, 0x09, 0x19, 0x29, 0x46},
    // 83 'S'
    {0x46, 0x49, 0x49, 0x49, 0x31},
    // 84 'T'
    {0x01, 0x01, 0x7F, 0x01, 0x01},
    // 85 'U'
    {0x3F, 0x40, 0x40, 0x40, 0x3F},
    // 86 'V'
    {0x1F, 0x20, 0x40, 0x20, 0x1F},
    // 87 'W'
    {0x3F, 0x40, 0x38, 0x40, 0x3F},
    // 88 'X'
    {0x63, 0x14, 0x08, 0x14, 0x63},
    // 89 'Y'
    {0x07, 0x08, 0x70, 0x08, 0x07},
    // 90 'Z'
    {0x61, 0x51, 0x49, 0x45, 0x43}
};

int Game::getFontIndex(char c) const {
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    if (c >= 32 && c <= 90) return c - 32;
    return 0; // fallback to space
}

void Game::drawChar(char c, int x, int y, SDL_Color color, int size) {
    int idx = getFontIndex(c);
    for (int col = 0; col < 5; ++col) {
        unsigned char colData = FONT_DATA[idx][col];
        for (int row = 0; row < 7; ++row) {
            if (colData & (1 << row)) {
                drawPixelRect(renderer, x + col * size, y + row * size, size, size, color.r, color.g, color.b, color.a);
            }
        }
    }
}

void Game::drawText(const char* text, int x, int y, SDL_Color color, int size) {
    int curX = x;
    int len = (int)strlen(text);
    for (int i = 0; i < len; ++i) {
        drawChar(text[i], curX, y, color, size);
        curX += 6 * size;
    }
}

// ── Constructor / Destructor ───────────────────────────────────────────
Game::Game()
    : window(nullptr), renderer(nullptr), target(nullptr),
      state(GameState::MENU), currentLevel(1), camX(0),
      levelCompleteTimer(0), menuSelection(0), running(true),
      screenShake(0), tutorialTimer(0), menuBlinkTimer(0) {}

Game::~Game() {
    if (target) SDL_DestroyTexture(target);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "SHADOW KNIGHT: Castlevania Chronicles",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        // Fallback to software renderer if needed
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
            return false;
        }
    }

    // Render target for true pixel art scaling
    target = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        INTERNAL_W, INTERNAL_H
    );
    if (!target) {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        return false;
    }

    state = GameState::MENU;
    return true;
}

void Game::startLevel(int num) {
    currentLevel = num;
    level.generate(num);
    player.reset(level.playerStartX, level.playerStartY);
    if (num >= 3) {
        player.hasSpecial = true;
    }
    camX = 0;
    particles.clear();
    activeTutorialText.clear();
    tutorialTimer = 0;
    state = GameState::PLAYING;
}

void Game::spawnParticles(float x, float y, Uint8 r, Uint8 g, Uint8 b, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = x;
        p.y = y;
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float speed = 0.5f + (float)(rand() % 30) / 10.0f;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed - 1.0f;
        p.life = 15 + (rand() % 15);
        p.r = r;
        p.g = g;
        p.b = b;
        particles.push_back(p);
    }
}

void Game::updateParticles() {
    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.vy += 0.15f; // particle gravity
        p.life--;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0; }),
        particles.end());
}

void Game::renderParticles() {
    for (const auto& p : particles) {
        int sx = (int)p.x - camX;
        int sy = (int)p.y;
        if (sx >= 0 && sx < INTERNAL_W && sy >= 0 && sy < INTERNAL_H) {
            drawPixelRect(renderer, sx, sy, 2, 2, p.r, p.g, p.b);
        }
    }
}

// ── Main Loop ──────────────────────────────────────────────────────────
void Game::run() {
    Uint32 frameTime = 1000 / FPS;

    while (running) {
        Uint32 startTick = SDL_GetTicks();

        handleEvents();
        update();
        render();

        Uint32 elapsed = SDL_GetTicks() - startTick;
        if (elapsed < frameTime) {
            SDL_Delay(frameTime - elapsed);
        }
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        } else if (e.type == SDL_KEYDOWN) {
            switch (state) {
                case GameState::MENU:
                    if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_w) {
                        menuSelection = (menuSelection - 1 + 2) % 2;
                    } else if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_s) {
                        menuSelection = (menuSelection + 1) % 2;
                    } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_z) {
                        if (menuSelection == 0) {
                            startLevel(1);
                        } else {
                            running = false;
                        }
                    }
                    break;

                case GameState::PLAYING:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        state = GameState::PAUSED;
                    }
                    break;

                case GameState::PAUSED:
                    if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_RETURN) {
                        state = GameState::PLAYING;
                    }
                    break;

                case GameState::LEVEL_COMPLETE:
                    if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_z) {
                        if (currentLevel < 5) {
                            startLevel(currentLevel + 1);
                        } else {
                            state = GameState::WIN;
                        }
                    }
                    break;

                case GameState::GAME_OVER:
                case GameState::WIN:
                    if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_z) {
                        state = GameState::MENU;
                    }
                    break;
            }
        }
    }
}

void Game::update() {
    menuBlinkTimer++;

    if (state != GameState::PLAYING) return;

    if (screenShake > 0) screenShake--;

    // 1. Player Input & Physics
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    player.handleInput(keys);
    player.update(level.tiles, level.getMovingPlatRects());

    // 2. Camera tracking
    int targetCamX = (int)player.pos.x - (INTERNAL_W / 2) + 8;
    int maxCamX = LEVEL_W * TILE_SIZE - INTERNAL_W;
    camX = std::max(0, std::min(maxCamX, targetCamX));

    // 3. Level Moving Platforms
    level.update();

    // 4. Tutorial Triggers (Level 1)
    if (currentLevel == 1) {
        for (auto& trig : level.tutorialTriggers) {
            if (!trig.shown && player.pos.x >= trig.x) {
                trig.shown = true;
                activeTutorialText = trig.text;
                tutorialTimer = 200; // ~3.3 seconds display
            }
        }
    }
    if (tutorialTimer > 0) {
        tutorialTimer--;
        if (tutorialTimer == 0) {
            activeTutorialText.clear();
        }
    }

    // 5. Enemies & Combat
    Rect attackHitbox = player.getAttackHitbox();
    bool isAttacking = player.attacking;

    for (auto& enemy : level.enemies) {
        if (!enemy.alive) continue;

        enemy.update(level.tiles, player.pos.x, player.pos.y);

        // Player sword strikes enemy
        if (isAttacking && rectsOverlap(attackHitbox, enemy.pos)) {
            int dmg = player.specialAttacking ? 3 : 1;
            enemy.takeDamage(dmg);
            spawnParticles(enemy.pos.x + enemy.pos.w * 0.5f, enemy.pos.y + enemy.pos.h * 0.5f, 255, 230, 80, 8);
            if (!enemy.alive) {
                player.score += (enemy.type == EnemyType::BOSS) ? 2000 : 150;
                spawnParticles(enemy.pos.x + enemy.pos.w * 0.5f, enemy.pos.y + enemy.pos.h * 0.5f, 255, 60, 60, 16);
            }
        }

        // Enemy touches player
        if (enemy.alive && rectsOverlap(player.pos, enemy.pos)) {
            if (!player.hurt && !player.dead) {
                player.takeDamage(enemy.damage);
                screenShake = 6;
                spawnParticles(player.pos.x + 8.0f, player.pos.y + 12.0f, 255, 40, 40, 6);
            }
        }

        // Enemy Projectiles vs Player
        for (auto& proj : enemy.projectiles) {
            if (proj.active && rectsOverlap(player.pos, proj.pos)) {
                proj.active = false;
                if (!player.hurt && !player.dead) {
                    player.takeDamage(proj.damage);
                    screenShake = 6;
                    spawnParticles(player.pos.x + 8.0f, player.pos.y + 12.0f, 255, 120, 30, 8);
                }
            }
        }
    }

    // 6. Collectibles Pickup
    // Gems
    for (auto it = level.gems.begin(); it != level.gems.end(); ) {
        if (rectsOverlap(player.pos, *it)) {
            player.score += 100;
            spawnParticles(it->x + 4.0f, it->y + 4.0f, 255, 220, 40, 10);
            it = level.gems.erase(it);
        } else {
            ++it;
        }
    }
    // Health Potions
    for (auto it = level.healthPotions.begin(); it != level.healthPotions.end(); ) {
        if (rectsOverlap(player.pos, *it)) {
            if (player.hp < player.maxHp) player.hp++;
            spawnParticles(it->x + 4.0f, it->y + 4.0f, 50, 255, 120, 10);
            it = level.healthPotions.erase(it);
        } else {
            ++it;
        }
    }
    // Extra Lives
    for (auto it = level.extraLives.begin(); it != level.extraLives.end(); ) {
        if (rectsOverlap(player.pos, *it)) {
            player.lives++;
            spawnParticles(it->x + 4.0f, it->y + 4.0f, 100, 180, 255, 14);
            it = level.extraLives.erase(it);
        } else {
            ++it;
        }
    }

    // 7. Check Door Exit
    if (rectsOverlap(player.pos, level.doorPos)) {
        if (currentLevel == 5) {
            // Must defeat boss to exit level 5
            bool bossAlive = false;
            for (const auto& en : level.enemies) {
                if (en.type == EnemyType::BOSS && en.alive) {
                    bossAlive = true;
                    break;
                }
            }
            if (!bossAlive) {
                state = GameState::WIN;
            }
        } else {
            state = GameState::LEVEL_COMPLETE;
            levelCompleteTimer = 0;
        }
    }

    // 8. Player Death / Game Over check
    if (player.dead && player.deathTimer >= 30) {
        player.lives--;
        if (player.lives <= 0) {
            state = GameState::GAME_OVER;
        } else {
            player.reset(level.playerStartX, level.playerStartY);
        }
    }

    updateParticles();
}

// ── Render Routines ────────────────────────────────────────────────────
void Game::renderHUD() {
    // Top Bar Background
    drawPixelRect(renderer, 0, 0, INTERNAL_W, 16, 10, 10, 20, 180);

    // HP Hearts
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color red   = { 230, 40, 40, 255 };
    SDL_Color gold  = { 255, 215, 0, 255 };

    drawText("HP:", 6, 4, white);
    for (int i = 0; i < player.maxHp; ++i) {
        if (i < player.hp) {
            drawPixelRect(renderer, 28 + i * 9, 5, 7, 7, 230, 30, 40);
            drawPixelRect(renderer, 29 + i * 9, 6, 2, 2, 255, 200, 200);
        } else {
            drawPixelRect(renderer, 28 + i * 9, 5, 7, 7, 60, 40, 50); // Empty heart slot
        }
    }

    // Lives
    char livesBuf[16];
    snprintf(livesBuf, sizeof(livesBuf), "LIVES:%d", player.lives);
    drawText(livesBuf, 100, 4, white);

    // Score
    char scoreBuf[32];
    snprintf(scoreBuf, sizeof(scoreBuf), "SCORE:%05d", player.score);
    drawText(scoreBuf, 175, 4, gold);

    // Level Title
    char lvlBuf[16];
    snprintf(lvlBuf, sizeof(lvlBuf), "STAGE %d", currentLevel);
    drawText(lvlBuf, 265, 4, white);

    // Tutorial text banner (Level 1)
    if (!activeTutorialText.empty()) {
        int bannerH = 18;
        int bannerY = INTERNAL_H - bannerH - 4;
        drawPixelRect(renderer, 8, bannerY, INTERNAL_W - 16, bannerH, 15, 15, 30, 220);
        drawPixelRect(renderer, 8, bannerY, INTERNAL_W - 16, 1, 255, 215, 50, 255); // Top border
        SDL_Color yellow = { 255, 235, 70, 255 };
        drawText(activeTutorialText.c_str(), 14, bannerY + 5, yellow);
    }
}

void Game::renderMenu() {
    // Castle Night Silhouette Background
    drawPixelRect(renderer, 0, 0, INTERNAL_W, INTERNAL_H, 15, 10, 25);

    // Distant moon
    drawPixelRect(renderer, 230, 25, 28, 28, 240, 240, 210);
    drawPixelRect(renderer, 233, 27, 24, 24, 255, 255, 230);

    // Castle towers silhouette
    drawPixelRect(renderer, 20, 100, 50, 140, 8, 5, 14);
    drawPixelRect(renderer, 15, 85, 60, 20, 8, 5, 14);
    drawPixelRect(renderer, 85, 130, 160, 110, 8, 5, 14);
    drawPixelRect(renderer, 130, 60, 50, 180, 8, 5, 14);
    drawPixelRect(renderer, 125, 45, 60, 20, 8, 5, 14);

    // Title
    SDL_Color red   = { 220, 30, 40, 255 };
    SDL_Color gold  = { 255, 215, 0, 255 };
    SDL_Color white = { 240, 240, 240, 255 };
    SDL_Color gray  = { 140, 140, 160, 255 };

    drawText("SHADOW KNIGHT", 68, 50, red, 2);
    drawText("A CASTLEVANIA-STYLE ACTION PLATFORMER", 48, 80, gold, 1);

    // Options
    int arrowY = (menuSelection == 0) ? 130 : 150;
    if ((menuBlinkTimer / 15) % 2 == 0) {
        drawText(">", 105, arrowY, gold, 1);
    }

    drawText("START GAME", 125, 130, (menuSelection == 0) ? white : gray, 1);
    drawText("QUIT", 125, 150, (menuSelection == 1) ? white : gray, 1);

    drawText("CONTROLS: ARROWS/WASD=MOVE  SPACE=JUMP  Z=ATTACK", 16, 200, gray, 1);
    drawText("PRESS ENTER TO SELECT", 96, 215, white, 1);
}

void Game::renderPause() {
    drawPixelRect(renderer, 0, 0, INTERNAL_W, INTERNAL_H, 0, 0, 0, 150);
    SDL_Color gold  = { 255, 215, 0, 255 };
    SDL_Color white = { 255, 255, 255, 255 };
    drawText("PAUSED", 126, 100, gold, 2);
    drawText("PRESS ESC OR ENTER TO RESUME", 70, 135, white, 1);
}

void Game::renderLevelComplete() {
    drawPixelRect(renderer, 0, 0, INTERNAL_W, INTERNAL_H, 0, 0, 0, 180);
    SDL_Color gold  = { 255, 215, 0, 255 };
    SDL_Color white = { 255, 255, 255, 255 };

    char msg[32];
    snprintf(msg, sizeof(msg), "STAGE %d CLEARED!", currentLevel);
    drawText(msg, 90, 80, gold, 2);

    char scoreMsg[32];
    snprintf(scoreMsg, sizeof(scoreMsg), "CURRENT SCORE: %d", player.score);
    drawText(scoreMsg, 95, 120, white, 1);

    drawText("PRESS ENTER TO ADVANCE TO NEXT STAGE", 46, 155, gold, 1);
}

void Game::renderGameOver() {
    drawPixelRect(renderer, 0, 0, INTERNAL_W, INTERNAL_H, 20, 0, 0, 200);
    SDL_Color red   = { 240, 30, 30, 255 };
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gold  = { 255, 215, 0, 255 };

    drawText("GAME OVER", 100, 80, red, 2);

    char scoreMsg[32];
    snprintf(scoreMsg, sizeof(scoreMsg), "FINAL SCORE: %d", player.score);
    drawText(scoreMsg, 105, 120, gold, 1);

    drawText("PRESS ENTER TO RETURN TO TITLE", 65, 160, white, 1);
}

void Game::renderWin() {
    drawPixelRect(renderer, 0, 0, INTERNAL_W, INTERNAL_H, 10, 15, 30, 220);
    SDL_Color gold  = { 255, 215, 0, 255 };
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color cyan  = { 100, 220, 255, 255 };

    drawText("VICTORY!", 112, 60, gold, 2);
    drawText("YOU VANQUISHED THE DARK ARCHON!", 60, 95, cyan, 1);
    drawText("PEACE HAS RETURNED TO THE REALM", 64, 115, white, 1);

    char scoreMsg[32];
    snprintf(scoreMsg, sizeof(scoreMsg), "FINAL SCORE: %d", player.score);
    drawText(scoreMsg, 105, 145, gold, 1);

    drawText("PRESS ENTER TO CONTINUE", 88, 185, white, 1);
}

void Game::render() {
    // 1. Render to internal 320x240 texture
    SDL_SetRenderTarget(renderer, target);

    if (state == GameState::MENU) {
        renderMenu();
    } else {
        // Clear background with level theme
        SDL_SetRenderDrawColor(renderer, level.bgColor.r, level.bgColor.g, level.bgColor.b, 255);
        SDL_RenderClear(renderer);

        int renderCamX = camX;
        if (screenShake > 0) {
            renderCamX += (rand() % 5) - 2;
        }

        // Render game layers
        level.renderBG(renderer, renderCamX);
        level.render(renderer, renderCamX);

        for (auto& enemy : level.enemies) {
            enemy.render(renderer, renderCamX);
            enemy.renderProjectiles(renderer, renderCamX);
        }

        player.render(renderer, renderCamX);
        renderParticles();
        renderHUD();

        // State Overlays
        if (state == GameState::PAUSED) {
            renderPause();
        } else if (state == GameState::LEVEL_COMPLETE) {
            renderLevelComplete();
        } else if (state == GameState::GAME_OVER) {
            renderGameOver();
        } else if (state == GameState::WIN) {
            renderWin();
        }
    }

    // 2. Present scaled texture to window
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, target, NULL, NULL);
    SDL_RenderPresent(renderer);
}
