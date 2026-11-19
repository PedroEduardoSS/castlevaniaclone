#pragma once
#include "types.h"
#include "player.h"
#include "enemy.h"
#include "level.h"
#include <vector>
#include <string>

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* target; // render target for pixel-perfect scaling
    GameState state;
    Player player;
    Level level;
    int currentLevel;
    int camX;
    std::vector<Particle> particles;
    int levelCompleteTimer;
    int menuSelection;
    bool running;
    int screenShake;

    // Tutorial
    std::string activeTutorialText;
    int tutorialTimer;

    // Menu animation
    int menuBlinkTimer;

    Game();
    ~Game();
    bool init();
    void run();
    void handleEvents();
    void update();
    void render();
    void renderHUD();
    void renderMenu();
    void renderGameOver();
    void renderWin();
    void renderPause();
    void renderLevelComplete();
    void startLevel(int num);
    void spawnParticles(float x, float y, Uint8 r, Uint8 g, Uint8 b, int count);
    void updateParticles();
    void renderParticles();
    void drawText(const char* text, int x, int y, SDL_Color color, int size = 1);

private:
    // 5x7 pixel font data: each char is 5 columns of 7-bit bitmaps
    static const unsigned char FONT_DATA[][5];
    int getFontIndex(char c) const;
    void drawChar(char c, int x, int y, SDL_Color color, int size);
};
