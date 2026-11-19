#pragma once
#include "types.h"
#include "enemy.h"
#include <vector>
#include <string>

class Level {
public:
    int levelNum; // 1-5
    std::vector<std::vector<TileType>> tiles;
    std::vector<Enemy> enemies;
    std::vector<Rect> gems;
    std::vector<Rect> healthPotions;
    std::vector<Rect> extraLives;
    std::vector<TutorialTrigger> tutorialTriggers;

    // Moving platforms
    struct MovingPlatform {
        Rect pos;
        float startX, endX;
        float speed;
        int dir; // 1 = right, -1 = left
    };
    std::vector<MovingPlatform> movingPlats;

    SDL_Color bgColor;
    float playerStartX, playerStartY;
    Rect doorPos;

    Level();
    void generate(int num);
    void update();
    std::vector<Rect> getMovingPlatRects() const;
    void render(SDL_Renderer* r, int camX);
    void renderBG(SDL_Renderer* r, int camX);

private:
    // Helper methods for level generation
    void clearTiles();
    void fillGround(int startTile, int endTile, int topRow = 13);
    void placeGap(int startTile, int widthTiles, int row = 13);
    void placePlatform(int x, int y, int width, TileType type = TileType::PLATFORM);
    void placeSpikes(int startTile, int row, int width);
    void placeLava(int startTile, int width);
    void placePillar(int x, int bottomRow, int height);
    void placeStairs(int startX, int startY, int steps, int dir);
    void placeWall(int x, int topRow, int bottomRow);
    void placeDoor(int tileX);
    void addGem(int tileX, int tileY);
    void addHealthPotion(int tileX, int tileY);
    void addExtraLife(int tileX, int tileY);
    void addEnemy(EnemyType type, int tileX, int tileY);
    void addMovingPlatform(float startTileX, float endTileX, int tileY, float speed);

    // Per-level generators
    void generateLevel1();
    void generateLevel2();
    void generateLevel3();
    void generateLevel4();
    void generateLevel5();

    // Background decoration helpers
    void drawStars(SDL_Renderer* r, int camX);
    void drawDistantTrees(SDL_Renderer* r, int camX);
    void drawDistantMountains(SDL_Renderer* r, int camX);
    void drawCastleBG(SDL_Renderer* r, int camX);
    void drawLavaGlow(SDL_Renderer* r, int camX);
    void drawTowerBG(SDL_Renderer* r, int camX);
};
