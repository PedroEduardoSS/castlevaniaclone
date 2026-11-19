#include "level.h"
#include "renderer.h"
#include <cmath>

Level::Level() : levelNum(1), playerStartX(32.0f), playerStartY(180.0f) {
    tiles.resize(LEVEL_H, std::vector<TileType>(LEVEL_W, TileType::AIR));
    doorPos = { 0, 0, (float)TILE_SIZE, (float)TILE_SIZE };
}

void Level::clearTiles() {
    for (int y = 0; y < LEVEL_H; ++y) {
        for (int x = 0; x < LEVEL_W; ++x) {
            tiles[y][x] = TileType::AIR;
        }
    }
    enemies.clear();
    gems.clear();
    healthPotions.clear();
    extraLives.clear();
    tutorialTriggers.clear();
    movingPlats.clear();
}

void Level::fillGround(int startTile, int endTile, int topRow) {
    for (int x = startTile; x <= endTile && x < LEVEL_W; ++x) {
        for (int y = topRow; y < LEVEL_H; ++y) {
            tiles[y][x] = TileType::GROUND;
        }
    }
}

void Level::placeGap(int startTile, int widthTiles, int row) {
    for (int x = startTile; x < startTile + widthTiles && x < LEVEL_W; ++x) {
        for (int y = row; y < LEVEL_H; ++y) {
            tiles[y][x] = TileType::AIR;
        }
    }
}

void Level::placePlatform(int x, int y, int width, TileType type) {
    for (int i = 0; i < width && (x + i) < LEVEL_W; ++i) {
        if (y >= 0 && y < LEVEL_H) {
            tiles[y][x + i] = type;
        }
    }
}

void Level::placeSpikes(int startTile, int row, int width) {
    for (int x = startTile; x < startTile + width && x < LEVEL_W; ++x) {
        if (row >= 0 && row < LEVEL_H) {
            tiles[row][x] = TileType::SPIKES;
        }
    }
}

void Level::placeLava(int startTile, int width) {
    for (int x = startTile; x < startTile + width && x < LEVEL_W; ++x) {
        for (int y = 13; y < LEVEL_H; ++y) {
            tiles[y][x] = TileType::LAVA;
        }
    }
}

void Level::placePillar(int x, int bottomRow, int height) {
    for (int h = 0; h < height; ++h) {
        int y = bottomRow - h;
        if (y >= 0 && y < LEVEL_H && x >= 0 && x < LEVEL_W) {
            tiles[y][x] = TileType::GROUND;
        }
    }
}

void Level::placeStairs(int startX, int startY, int steps, int dir) {
    for (int i = 0; i < steps; ++i) {
        int x = startX + (dir * i);
        int y = startY - i;
        if (x >= 0 && x < LEVEL_W && y >= 0 && y < LEVEL_H) {
            for (int sy = y; sy < LEVEL_H; ++sy) {
                tiles[sy][x] = TileType::GROUND;
            }
        }
    }
}

void Level::placeWall(int x, int topRow, int bottomRow) {
    for (int y = topRow; y <= bottomRow; ++y) {
        if (x >= 0 && x < LEVEL_W && y >= 0 && y < LEVEL_H) {
            tiles[y][x] = TileType::GROUND;
        }
    }
}

void Level::placeDoor(int tileX) {
    if (tileX >= 0 && tileX < LEVEL_W) {
        tiles[12][tileX] = TileType::DOOR;
        doorPos.x = (float)(tileX * TILE_SIZE);
        doorPos.y = (float)(12 * TILE_SIZE);
        doorPos.w = (float)TILE_SIZE;
        doorPos.h = (float)TILE_SIZE;
    }
}

void Level::addGem(int tileX, int tileY) {
    gems.push_back({ (float)(tileX * TILE_SIZE + 4), (float)(tileY * TILE_SIZE + 4), 8.0f, 8.0f });
}

void Level::addHealthPotion(int tileX, int tileY) {
    healthPotions.push_back({ (float)(tileX * TILE_SIZE + 4), (float)(tileY * TILE_SIZE + 3), 8.0f, 10.0f });
}

void Level::addExtraLife(int tileX, int tileY) {
    extraLives.push_back({ (float)(tileX * TILE_SIZE + 4), (float)(tileY * TILE_SIZE + 4), 8.0f, 8.0f });
}

void Level::addEnemy(EnemyType type, int tileX, int tileY) {
    enemies.emplace_back(type, (float)(tileX * TILE_SIZE), (float)(tileY * TILE_SIZE));
}

void Level::addMovingPlatform(float startTileX, float endTileX, int tileY, float speed) {
    MovingPlatform mp;
    mp.pos = { startTileX * TILE_SIZE, (float)(tileY * TILE_SIZE), 32.0f, 8.0f };
    mp.startX = startTileX * TILE_SIZE;
    mp.endX = endTileX * TILE_SIZE;
    mp.speed = speed;
    mp.dir = 1;
    movingPlats.push_back(mp);
}

// ── LEVEL 1: Forest Ruins (Tutorial) ──────────────────────────────────
void Level::generateLevel1() {
    bgColor = { 15, 28, 20, 255 }; // Forest twilight
    fillGround(0, 199, 13);

    // Left & Right world borders
    placeWall(0, 0, 14);
    placeWall(199, 0, 14);

    // Platforms & small obstacles
    placePlatform(15, 10, 4);
    placePlatform(25, 9, 5);
    placePillar(35, 12, 2);

    // Gap to teach jumping
    placeGap(42, 3);
    placePlatform(42, 10, 3);

    // Elevated ruin steps
    placeStairs(55, 12, 3, 1);
    placePlatform(60, 9, 6);
    placeGap(70, 3);

    // Platforms over ruin stones
    placePlatform(85, 10, 4);
    placePlatform(93, 8, 4);
    placePlatform(101, 10, 4);
    placeGap(110, 4);
    placePlatform(111, 11, 2);

    placePlatform(125, 10, 5);
    placePillar(140, 12, 3);
    placePlatform(150, 9, 6);

    placeDoor(190);

    // Tutorial Triggers
    tutorialTriggers.push_back({ 32.0f,  "TUTORIAL: Use ARROW KEYS or A/D to WALK", false });
    tutorialTriggers.push_back({ 300.0f, "TUTORIAL: Press SPACE or W to JUMP across gaps", false });
    tutorialTriggers.push_back({ 500.0f, "TUTORIAL: Press Z or J to ATTACK with your sword", false });
    tutorialTriggers.push_back({ 800.0f, "TUTORIAL: Collect GEMS for score & POTIONS for HP", false });
    tutorialTriggers.push_back({ 1400.0f, "TUTORIAL: Reach the GOTHIC DOOR to clear level!", false });

    // Enemies (Gentle start: Slimes)
    addEnemy(EnemyType::SLIME, 38, 12);
    addEnemy(EnemyType::SLIME, 62, 8);
    addEnemy(EnemyType::SLIME, 95, 7);
    addEnemy(EnemyType::SLIME, 130, 12);
    addEnemy(EnemyType::SLIME, 160, 12);

    // Items
    addGem(16, 8);
    addGem(27, 7);
    addHealthPotion(63, 7);
    addGem(87, 8);
    addGem(95, 6);
    addHealthPotion(127, 8);
    addGem(152, 7);
    addGem(175, 11);
}

// ── LEVEL 2: Dark Caves ───────────────────────────────────────────────
void Level::generateLevel2() {
    bgColor = { 10, 12, 30, 255 }; // Deep subterranean blue
    fillGround(0, 199, 13);
    placeWall(0, 0, 14);
    placeWall(199, 0, 14);

    // Ceiling stalactites / spikes
    placeSpikes(20, 0, 5);
    placeSpikes(50, 0, 6);
    placeSpikes(80, 0, 6);

    // Floor gaps with spikes at the bottom
    placeGap(25, 4);
    placeSpikes(25, 14, 4);
    placePlatform(25, 10, 4);

    placeStairs(40, 12, 3, 1);
    placePlatform(46, 9, 6);
    placePillar(55, 12, 3);

    placeGap(65, 5);
    placePlatform(66, 11, 3);

    // Cave tiering
    placePlatform(80, 9, 5);
    placePlatform(90, 7, 5);
    placePlatform(100, 9, 5);

    placeGap(115, 6);
    placeSpikes(115, 14, 6);
    placePlatform(116, 10, 4);

    placePillar(135, 12, 4);
    placePlatform(145, 9, 6);
    placeGap(160, 4);
    placePlatform(161, 10, 2);

    placeDoor(190);

    // Enemies: Slimes + Agile Bats
    addEnemy(EnemyType::SLIME, 32, 12);
    addEnemy(EnemyType::BAT, 48, 5);
    addEnemy(EnemyType::SLIME, 75, 12);
    addEnemy(EnemyType::BAT, 92, 4);
    addEnemy(EnemyType::BAT, 120, 5);
    addEnemy(EnemyType::SLIME, 148, 8);
    addEnemy(EnemyType::BAT, 165, 4);
    addEnemy(EnemyType::SLIME, 178, 12);

    // Collectibles
    addGem(26, 8);
    addHealthPotion(48, 7);
    addGem(82, 7);
    addGem(92, 5);
    addGem(102, 7);
    addHealthPotion(135, 7);
    addGem(162, 8);
    addExtraLife(170, 11);
}

// ── LEVEL 3: Castle Halls ─────────────────────────────────────────────
void Level::generateLevel3() {
    bgColor = { 25, 20, 32, 255 }; // Gothic castle dusk
    fillGround(0, 199, 13);
    placeWall(0, 0, 14);
    placeWall(199, 0, 14);

    // Castle hanging chains & decorations
    for (int cx = 10; cx < 190; cx += 20) {
        tiles[2][cx] = TileType::CHAIN;
        tiles[3][cx] = TileType::CHAIN;
        tiles[4][cx] = TileType::CHAIN;
    }

    // Moving platforms across vast castle chasms
    placeGap(25, 8);
    addMovingPlatform(26.0f, 32.0f, 10, 1.0f);

    placePlatform(40, 9, 6);
    placePillar(50, 12, 3);

    placeGap(60, 10);
    placeSpikes(60, 14, 10);
    addMovingPlatform(62.0f, 68.0f, 9, 1.2f);

    // High castle chandeliers & battlements
    placePlatform(80, 8, 4);
    placePlatform(90, 6, 5);
    placePlatform(102, 8, 4);

    placeGap(120, 9);
    addMovingPlatform(122.0f, 128.0f, 10, 1.4f);

    placeStairs(140, 12, 4, 1);
    placePlatform(148, 8, 8);
    placePillar(165, 12, 3);

    placeDoor(190);

    // Enemies: Skeletons + Bats + Slimes
    addEnemy(EnemyType::SKELETON, 43, 8);
    addEnemy(EnemyType::BAT, 55, 4);
    addEnemy(EnemyType::SLIME, 75, 12);
    addEnemy(EnemyType::SKELETON, 92, 5);
    addEnemy(EnemyType::BAT, 110, 5);
    addEnemy(EnemyType::SKELETON, 152, 7);
    addEnemy(EnemyType::BAT, 170, 4);

    // Items
    addGem(42, 7);
    addHealthPotion(92, 4);
    addGem(103, 6);
    addExtraLife(150, 6);
    addGem(166, 8);
    addHealthPotion(180, 11);
}

// ── LEVEL 4: Lava Depths ──────────────────────────────────────────────
void Level::generateLevel4() {
    bgColor = { 35, 10, 8, 255 }; // Volcanic inferno
    fillGround(0, 199, 13);
    placeWall(0, 0, 14);
    placeWall(199, 0, 14);

    // Massive bubbling lava oceans!
    placeLava(20, 12);
    addMovingPlatform(22.0f, 30.0f, 9, 1.2f);

    placePlatform(38, 9, 6);
    placePillar(48, 12, 3);

    placeLava(55, 16);
    addMovingPlatform(57.0f, 63.0f, 8, 1.4f);
    placePlatform(66, 8, 3);
    addMovingPlatform(70.0f, 76.0f, 9, 1.4f);

    placePlatform(85, 9, 6);
    placeSpikes(95, 12, 4);

    placeLava(105, 18);
    addMovingPlatform(107.0f, 114.0f, 9, 1.5f);
    addMovingPlatform(116.0f, 122.0f, 8, 1.3f);

    placePlatform(135, 9, 7);
    placePillar(150, 12, 3);

    placeLava(160, 12);
    addMovingPlatform(162.0f, 170.0f, 9, 1.6f);

    placeDoor(190);

    // Enemies: Fire Spirits + Skeletons + Bats
    addEnemy(EnemyType::FIRE_SPIRIT, 30, 4);
    addEnemy(EnemyType::SKELETON, 40, 8);
    addEnemy(EnemyType::FIRE_SPIRIT, 68, 4);
    addEnemy(EnemyType::BAT, 88, 4);
    addEnemy(EnemyType::FIRE_SPIRIT, 115, 3);
    addEnemy(EnemyType::SKELETON, 138, 8);
    addEnemy(EnemyType::FIRE_SPIRIT, 165, 4);

    // Items
    addGem(40, 7);
    addHealthPotion(71, 6);
    addGem(87, 7);
    addGem(137, 7);
    addHealthPotion(140, 7);
    addExtraLife(151, 8);
}

// ── LEVEL 5: Dark Tower (Boss Arena) ──────────────────────────────────
void Level::generateLevel5() {
    bgColor = { 20, 8, 25, 255 }; // Dread violet citadel
    fillGround(0, 199, 13);
    placeWall(0, 0, 14);
    placeWall(199, 0, 14);

    // The Gauntlet before the Boss Chamber
    placeSpikes(15, 12, 4);
    placePlatform(16, 9, 4);

    placeGap(25, 8);
    placeLava(25, 8);
    addMovingPlatform(26.0f, 32.0f, 9, 1.5f);

    placePlatform(40, 8, 5);
    placeSpikes(48, 12, 4);

    placeGap(58, 10);
    placeLava(58, 10);
    addMovingPlatform(60.0f, 67.0f, 9, 1.6f);

    placeStairs(75, 12, 4, 1);
    placePlatform(83, 8, 6);

    placeGap(95, 12);
    placeLava(95, 12);
    addMovingPlatform(97.0f, 105.0f, 8, 1.7f);

    placePlatform(115, 8, 8);

    // Boss Arena Entrance Wall with doorway
    placeWall(140, 0, 8);
    placeWall(140, 11, 14); // Arched threshold

    // BOSS ARENA: Flat battleground tiles 142 - 195
    placeDoor(192);

    // Gauntlet enemies
    addEnemy(EnemyType::BAT, 20, 4);
    addEnemy(EnemyType::SKELETON, 42, 7);
    addEnemy(EnemyType::FIRE_SPIRIT, 65, 4);
    addEnemy(EnemyType::SLIME, 85, 7);
    addEnemy(EnemyType::FIRE_SPIRIT, 100, 3);
    addEnemy(EnemyType::SKELETON, 118, 7);

    // FINAL BOSS!
    addEnemy(EnemyType::BOSS, 175, 9);

    // High tier items
    addGem(18, 7);
    addHealthPotion(42, 6);
    addGem(85, 6);
    addHealthPotion(118, 6);
    addExtraLife(120, 6);
    addHealthPotion(145, 11); // Before boss fight
}

void Level::generate(int num) {
    levelNum = num;
    clearTiles();

    playerStartX = 32.0f;
    playerStartY = 180.0f;

    switch (num) {
        case 1: generateLevel1(); break;
        case 2: generateLevel2(); break;
        case 3: generateLevel3(); break;
        case 4: generateLevel4(); break;
        case 5: generateLevel5(); break;
        default: generateLevel1(); break;
    }
}

void Level::update() {
    // Update moving platforms
    for (auto& mp : movingPlats) {
        mp.pos.x += mp.speed * mp.dir;
        if (mp.pos.x >= mp.endX) {
            mp.pos.x = mp.endX;
            mp.dir = -1;
        } else if (mp.pos.x <= mp.startX) {
            mp.pos.x = mp.startX;
            mp.dir = 1;
        }
    }
}

std::vector<Rect> Level::getMovingPlatRects() const {
    std::vector<Rect> rects;
    for (const auto& mp : movingPlats) {
        rects.push_back(mp.pos);
    }
    return rects;
}

// ── Background Parallax Rendering ──────────────────────────────────────
void Level::drawStars(SDL_Renderer* r, int camX) {
    // Fixed seeded starry night
    for (int i = 0; i < 40; ++i) {
        int sx = (i * 47 - (camX / 4)) % (INTERNAL_W + 50);
        if (sx < 0) sx += INTERNAL_W + 50;
        int sy = (i * 31) % 110;
        drawPixelRect(r, sx, sy, 1, 1, 230, 230, 255, 180);
    }
}

void Level::drawDistantTrees(SDL_Renderer* r, int camX) {
    for (int i = 0; i < 15; ++i) {
        int bx = i * 45 - (camX / 3);
        drawPixelRect(r, bx + 10, 140, 20, 70, 10, 22, 15);
        drawPixelRect(r, bx + 18, 200, 4, 20, 25, 15, 10);
    }
}

void Level::drawDistantMountains(SDL_Renderer* r, int camX) {
    for (int i = 0; i < 8; ++i) {
        int mx = i * 90 - (camX / 5);
        drawPixelRect(r, mx, 110, 80, 100, 20, 18, 38);
        drawPixelRect(r, mx + 20, 85, 40, 25, 25, 22, 45);
    }
}

void Level::drawCastleBG(SDL_Renderer* r, int camX) {
    for (int i = 0; i < 6; ++i) {
        int cx = i * 110 - (camX / 3);
        drawPixelRect(r, cx, 80, 50, 130, 35, 25, 40);
        drawPixelRect(r, cx + 5, 60, 15, 20, 40, 30, 45);
        drawPixelRect(r, cx + 30, 60, 15, 20, 40, 30, 45);
    }
}

void Level::drawLavaGlow(SDL_Renderer* r, int camX) {
    (void)camX;
    // Ambient red glow across lower screen
    drawPixelRect(r, 0, 160, INTERNAL_W, 80, 80, 15, 5, 60);
}

void Level::drawTowerBG(SDL_Renderer* r, int camX) {
    for (int i = 0; i < 5; ++i) {
        int tx = i * 130 - (camX / 4);
        drawPixelRect(r, tx, 40, 60, 170, 28, 12, 35);
        drawPixelRect(r, tx + 15, 20, 30, 20, 35, 16, 42);
    }
}

void Level::renderBG(SDL_Renderer* r, int camX) {
    drawStars(r, camX);

    switch (levelNum) {
        case 1: drawDistantTrees(r, camX); break;
        case 2: drawDistantMountains(r, camX); break;
        case 3: drawCastleBG(r, camX); break;
        case 4: drawLavaGlow(r, camX); break;
        case 5: drawTowerBG(r, camX); break;
    }
}

void Level::render(SDL_Renderer* r, int camX) {
    int startCol = std::max(0, (camX / TILE_SIZE) - 1);
    int endCol   = std::min(LEVEL_W - 1, ((camX + INTERNAL_W) / TILE_SIZE) + 1);

    // Draw tiles in visible window
    for (int y = 0; y < LEVEL_H; ++y) {
        for (int x = startCol; x <= endCol; ++x) {
            if (tiles[y][x] != TileType::AIR) {
                drawTile(r, tiles[y][x], x, y, camX, levelNum);
            }
        }
    }

    // Draw moving platforms
    for (const auto& mp : movingPlats) {
        drawMovingPlatform(r, mp.pos.x, mp.pos.y, mp.pos.w, camX);
    }

    // Draw items
    for (const auto& g : gems) {
        drawGem(r, g.x, g.y, camX);
    }
    for (const auto& hp : healthPotions) {
        drawHealthPotion(r, hp.x, hp.y, camX);
    }
    for (const auto& l : extraLives) {
        drawExtraLife(r, l.x, l.y, camX);
    }
}
