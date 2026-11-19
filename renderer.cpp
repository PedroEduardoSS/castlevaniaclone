#include "renderer.h"
#include <algorithm>
#include <cmath>

void drawPixelRect(SDL_Renderer* r, int x, int y, int w, int h,
                   Uint8 red, Uint8 green, Uint8 blue, Uint8 a) {
    if (a < 255) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    }
    SDL_SetRenderDrawColor(r, red, green, blue, a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(r, &rect);
    if (a < 255) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }
}

// ── Draw Tile ──────────────────────────────────────────────────────────
void drawTile(SDL_Renderer* r, TileType type, int tileX, int tileY, int camX, int levelNum) {
    int sx = tileX * TILE_SIZE - camX;
    int sy = tileY * TILE_SIZE;

    if (sx + TILE_SIZE < 0 || sx > INTERNAL_W) return;

    switch (type) {
        case TileType::GROUND: {
            // Level-specific thematic ground
            Uint8 baseR = 70, baseG = 65, baseB = 80;
            Uint8 topR = 90, topG = 85, topB = 100;
            Uint8 darkR = 50, darkG = 45, darkB = 60;

            if (levelNum == 1) { // Forest Ruins: mossy stone
                baseR = 60; baseG = 75; baseB = 50;
                topR  = 75; topG = 130; topB = 45;
                darkR = 40; darkG = 50; darkB = 35;
            } else if (levelNum == 2) { // Dark Caves: cold blue rock
                baseR = 45; baseG = 50; baseB = 70;
                topR  = 65; topG = 75; topB = 95;
                darkR = 30; darkG = 35; darkB = 50;
            } else if (levelNum == 3) { // Castle: cut stone bricks
                baseR = 75; baseG = 70; baseB = 75;
                topR  = 105; topG = 100; topB = 105;
                darkR = 45; darkG = 40; darkB = 45;
            } else if (levelNum == 4) { // Lava Depths: scorched dark volcanic rock
                baseR = 60; baseG = 30; baseB = 30;
                topR  = 95; topG = 45; topB = 35;
                darkR = 35; darkG = 18; darkB = 18;
            } else if (levelNum == 5) { // Dark Tower: obsidian violet stone
                baseR = 50; baseG = 35; baseB = 65;
                topR  = 80; topG = 55; topB = 95;
                darkR = 30; darkG = 20; darkB = 45;
            }

            // Main body
            drawPixelRect(r, sx, sy, TILE_SIZE, TILE_SIZE, baseR, baseG, baseB);
            // Grass / Highlight top rim
            drawPixelRect(r, sx, sy, TILE_SIZE, 3, topR, topG, topB);
            // Brick seam lines for depth
            drawPixelRect(r, sx, sy + 7, TILE_SIZE, 1, darkR, darkG, darkB);
            drawPixelRect(r, sx + 8, sy + 1, 1, 6, darkR, darkG, darkB);
            drawPixelRect(r, sx + 4, sy + 8, 1, 8, darkR, darkG, darkB);
            drawPixelRect(r, sx + 12, sy + 8, 1, 8, darkR, darkG, darkB);
            break;
        }
        case TileType::PLATFORM: {
            // Wooden or ornate carved platform
            drawPixelRect(r, sx, sy, TILE_SIZE, 4, 150, 100, 60);
            drawPixelRect(r, sx, sy + 4, TILE_SIZE, 2, 90, 55, 30);
            // Wood plank notches
            drawPixelRect(r, sx + 5, sy, 1, 4, 120, 75, 45);
            drawPixelRect(r, sx + 11, sy, 1, 4, 120, 75, 45);
            break;
        }
        case TileType::SPIKES: {
            // 4 sharp metal spikes per tile
            for (int i = 0; i < 4; ++i) {
                int px = sx + i * 4;
                drawPixelRect(r, px + 1, sy + 8, 2, 8, 160, 160, 175);
                drawPixelRect(r, px + 1, sy + 4, 2, 4, 200, 200, 215);
                drawPixelRect(r, px + 1, sy + 1, 1, 3, 240, 240, 255);
                drawPixelRect(r, px + 2, sy + 2, 1, 2, 130, 130, 145);
            }
            break;
        }
        case TileType::LAVA: {
            // Animated boiling magma
            static int lavaAnim = 0;
            lavaAnim = (lavaAnim + 1) % 60;
            int offset = ((lavaAnim / 15) + tileX) % 3;

            drawPixelRect(r, sx, sy, TILE_SIZE, TILE_SIZE, 210, 50, 10);
            drawPixelRect(r, sx, sy, TILE_SIZE, 4, 255, 170 + offset * 25, 30);
            drawPixelRect(r, sx + 2 + offset * 3, sy + 6, 4, 3, 255, 220, 80);
            drawPixelRect(r, sx + 8 - offset * 2, sy + 10, 5, 2, 255, 100, 20);
            break;
        }
        case TileType::DOOR: {
            // Gothic arched doorway with golden glow
            drawPixelRect(r, sx, sy, TILE_SIZE, TILE_SIZE, 40, 30, 50); // Frame
            drawPixelRect(r, sx + 2, sy + 2, TILE_SIZE - 4, TILE_SIZE - 2, 20, 15, 25); // Inside dark
            // Golden runes inside
            drawPixelRect(r, sx + 4, sy + 5, 8, 2, 255, 215, 80);
            drawPixelRect(r, sx + 7, sy + 5, 2, 8, 255, 215, 80);
            drawPixelRect(r, sx + 3, sy + 11, 10, 2, 220, 180, 50);
            break;
        }
        case TileType::DECO: {
            // Cobweb or cracked stone background decoration
            drawPixelRect(r, sx + 2, sy + 2, 3, 1, 90, 85, 95);
            drawPixelRect(r, sx + 4, sy + 3, 4, 1, 90, 85, 95);
            drawPixelRect(r, sx + 7, sy + 4, 1, 4, 90, 85, 95);
            break;
        }
        case TileType::CHAIN: {
            // Castle wall background hanging iron chain
            for (int cy = sy; cy < sy + TILE_SIZE; cy += 4) {
                drawPixelRect(r, sx + 7, cy, 2, 3, 130, 130, 140);
                drawPixelRect(r, sx + 8, cy + 1, 1, 1, 70, 70, 80);
            }
            break;
        }
        default:
            break;
    }
}

// ── Draw Collectibles ──────────────────────────────────────────────────
void drawGem(SDL_Renderer* r, float x, float y, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 8 < 0 || sx > INTERNAL_W) return;

    // Diamond gem shape
    drawPixelRect(r, sx + 2, sy, 4, 2, 255, 240, 120);     // Top apex
    drawPixelRect(r, sx + 1, sy + 2, 6, 2, 255, 215, 0);   // Upper body
    drawPixelRect(r, sx, sy + 4, 8, 2, 230, 180, 0);       // Mid widest
    drawPixelRect(r, sx + 1, sy + 6, 6, 2, 190, 140, 0);   // Lower taper
    drawPixelRect(r, sx + 3, sy + 8, 2, 1, 150, 100, 0);   // Bottom point
    // Sparkle highlight
    drawPixelRect(r, sx + 2, sy + 2, 2, 2, 255, 255, 240);
}

void drawHealthPotion(SDL_Renderer* r, float x, float y, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 8 < 0 || sx > INTERNAL_W) return;

    // Cork stopper
    drawPixelRect(r, sx + 3, sy, 2, 2, 180, 120, 70);
    // Bottle neck
    drawPixelRect(r, sx + 2, sy + 2, 4, 2, 200, 220, 230);
    // Rounded glass body with bubbling red elixir
    drawPixelRect(r, sx + 1, sy + 4, 6, 5, 230, 30, 40);
    drawPixelRect(r, sx + 2, sy + 9, 4, 1, 180, 20, 30);
    // Glass shine reflection
    drawPixelRect(r, sx + 2, sy + 4, 1, 3, 255, 180, 190);
    drawPixelRect(r, sx + 4, sy + 6, 1, 1, 255, 255, 255);
}

void drawExtraLife(SDL_Renderer* r, float x, float y, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 8 < 0 || sx > INTERNAL_W) return;

    // Glowing blue mystic heart
    drawPixelRect(r, sx + 1, sy, 2, 2, 90, 180, 255);
    drawPixelRect(r, sx + 5, sy, 2, 2, 90, 180, 255);
    drawPixelRect(r, sx, sy + 2, 8, 3, 60, 140, 240);
    drawPixelRect(r, sx + 1, sy + 5, 6, 2, 40, 100, 210);
    drawPixelRect(r, sx + 2, sy + 7, 4, 2, 20, 70, 170);
    drawPixelRect(r, sx + 3, sy + 9, 2, 1, 10, 40, 130);
    // White glint
    drawPixelRect(r, sx + 2, sy + 2, 2, 2, 220, 240, 255);
}

void drawProjectile(SDL_Renderer* r, float x, float y, float w, float h,
                    Uint8 red, Uint8 green, Uint8 blue, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    int iw = (int)w;
    int ih = (int)h;
    if (sx + iw < 0 || sx > INTERNAL_W) return;

    // Outer glow
    drawPixelRect(r, sx - 1, sy - 1, iw + 2, ih + 2, red, green, blue, 128);
    // Core
    drawPixelRect(r, sx, sy, iw, ih, red, green, blue, 255);
    // Inner hot white center
    if (iw > 3 && ih > 3) {
        drawPixelRect(r, sx + 1, sy + 1, iw - 2, ih - 2, 255, 255, 240, 255);
    }
}

void drawMovingPlatform(SDL_Renderer* r, float x, float y, float w, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    int iw = (int)w;
    if (sx + iw < 0 || sx > INTERNAL_W) return;

    // Mechanical hovering stone platform
    drawPixelRect(r, sx, sy, iw, 5, 110, 125, 145);      // Metal top edge
    drawPixelRect(r, sx, sy + 5, iw, 3, 75, 85, 105);    // Stone underbody
    // Glowing runic engine underneath
    drawPixelRect(r, sx + 4, sy + 3, iw - 8, 2, 60, 200, 255);
    drawPixelRect(r, sx + 8, sy + 7, iw - 16, 1, 30, 140, 220);
}

// ── Draw Player (Knight) ───────────────────────────────────────────────
void drawPlayer(SDL_Renderer* r, float x, float y, Direction dir,
                int animFrame, bool attacking, int attackTimer,
                bool hurt, bool dead, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 24 < 0 || sx > INTERNAL_W) return;

    if (hurt && (attackTimer % 4 < 2)) return; // Hurt blink

    bool facingRight = (dir == Direction::RIGHT);

    // Armor palette
    Uint8 armorR = 70, armorG = 75, armorB = 140;
    Uint8 armorLightR = 120, armorLightG = 130, armorLightB = 210;
    Uint8 capeR = 180, capeG = 30, capeB = 40;
    Uint8 goldR = 240, goldG = 190, goldB = 40;

    if (dead) {
        // Fallen knight pose
        drawPixelRect(r, sx, sy + 16, 24, 8, armorR, armorG, armorB);
        drawPixelRect(r, sx + 4, sy + 18, 6, 4, goldR, goldG, goldB);
        drawPixelRect(r, sx + 12, sy + 17, 10, 5, capeR, capeG, capeB);
        return;
    }

    // Helmet & Plume
    drawPixelRect(r, sx + 4, sy, 8, 2, capeR, capeG, capeB); // Red plume
    drawPixelRect(r, sx + 3, sy + 2, 10, 7, armorR, armorG, armorB); // Steel Helm
    drawPixelRect(r, sx + 4, sy + 2, 2, 3, armorLightR, armorLightG, armorLightB);
    // Helmet Visor / Eye slit
    int visorX = facingRight ? (sx + 8) : (sx + 4);
    drawPixelRect(r, visorX, sy + 5, 4, 2, 255, 230, 100); // Glowing gaze

    // Cape behind
    int capeX = facingRight ? (sx + 1) : (sx + 11);
    drawPixelRect(r, capeX, sy + 8, 4, 11 + (animFrame % 2), capeR, capeG, capeB);

    // Breastplate / Torso
    drawPixelRect(r, sx + 4, sy + 9, 8, 7, armorR, armorG, armorB);
    drawPixelRect(r, sx + 7, sy + 9, 2, 7, goldR, goldG, goldB); // Gold heraldry cross
    drawPixelRect(r, sx + 5, sy + 11, 6, 2, goldR, goldG, goldB);

    // Belt / Tassets
    drawPixelRect(r, sx + 4, sy + 16, 8, 2, 60, 45, 30);
    drawPixelRect(r, sx + 7, sy + 16, 2, 2, goldR, goldG, goldB);

    // Greaves / Legs (animated walking)
    int legOffset = (animFrame % 2 == 1) ? 2 : 0;
    if (facingRight) {
        drawPixelRect(r, sx + 4 + legOffset, sy + 18, 3, 6, armorR, armorG, armorB);
        drawPixelRect(r, sx + 9 - legOffset, sy + 18, 3, 6, armorR, armorG, armorB);
    } else {
        drawPixelRect(r, sx + 4 - legOffset, sy + 18, 3, 6, armorR, armorG, armorB);
        drawPixelRect(r, sx + 9 + legOffset, sy + 18, 3, 6, armorR, armorG, armorB);
    }

    // Weapon / Sword
    if (attacking) {
        int swordX = facingRight ? (sx + 14) : (sx - 14);
        int swordY = sy + 7;
        // Blade
        drawPixelRect(r, swordX, swordY, 14, 3, 220, 225, 240);
        // Golden Crossguard & hilt
        int hiltX = facingRight ? (sx + 12) : (sx + 2);
        drawPixelRect(r, hiltX, swordY - 3, 2, 9, goldR, goldG, goldB);
        // Swing swoosh arc
        int arcX = facingRight ? (sx + 18) : (sx - 10);
        drawPixelRect(r, arcX, swordY - 6, 8, 2, 255, 255, 255, 180);
        drawPixelRect(r, arcX + 2, swordY + 5, 8, 2, 200, 220, 255, 180);
    } else {
        // Sheathed sword
        int sheathX = facingRight ? (sx + 1) : (sx + 13);
        drawPixelRect(r, sheathX, sy + 10, 2, 10, 160, 165, 180);
        drawPixelRect(r, sheathX - 1, sy + 9, 4, 2, goldR, goldG, goldB);
    }
}

// ── Draw Enemies ───────────────────────────────────────────────────────
void drawSlime(SDL_Renderer* r, float x, float y, int animFrame, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 14 < 0 || sx > INTERNAL_W) return;

    bool squish = (animFrame % 2 == 1);
    int h = squish ? 7 : 9;
    int w = squish ? 14 : 12;
    int ox = squish ? -1 : 0;
    int oy = squish ? 2 : 0;

    // Green jelly body
    drawPixelRect(r, sx + ox + 2, sy + oy, w - 4, 2, 90, 240, 90);
    drawPixelRect(r, sx + ox, sy + oy + 2, w, h - 3, 40, 190, 40);
    drawPixelRect(r, sx + ox + 1, sy + oy + h - 1, w - 2, 1, 20, 130, 25);
    // Big curious eyes
    drawPixelRect(r, sx + ox + 3, sy + oy + 2, 2, 3, 255, 255, 255);
    drawPixelRect(r, sx + ox + 4, sy + oy + 3, 1, 2, 10, 40, 10);
    drawPixelRect(r, sx + ox + 7, sy + oy + 2, 2, 3, 255, 255, 255);
    drawPixelRect(r, sx + ox + 8, sy + oy + 3, 1, 2, 10, 40, 10);
}

void drawBat(SDL_Renderer* r, float x, float y, int animFrame, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 16 < 0 || sx > INTERNAL_W) return;

    bool wingsUp = (animFrame % 2 == 0);

    // Bat Head & Body
    drawPixelRect(r, sx + 5, sy + 3, 4, 5, 60, 25, 40);
    // Glowing red eyes
    drawPixelRect(r, sx + 5, sy + 4, 1, 1, 255, 40, 40);
    drawPixelRect(r, sx + 8, sy + 4, 1, 1, 255, 40, 40);
    // Ears
    drawPixelRect(r, sx + 5, sy + 1, 1, 2, 80, 35, 55);
    drawPixelRect(r, sx + 8, sy + 1, 1, 2, 80, 35, 55);

    // Wings
    if (wingsUp) {
        drawPixelRect(r, sx + 1, sy, 4, 3, 120, 40, 50);
        drawPixelRect(r, sx, sy + 2, 3, 3, 90, 30, 40);
        drawPixelRect(r, sx + 9, sy, 4, 3, 120, 40, 50);
        drawPixelRect(r, sx + 11, sy + 2, 3, 3, 90, 30, 40);
    } else {
        drawPixelRect(r, sx + 1, sy + 4, 4, 4, 120, 40, 50);
        drawPixelRect(r, sx, sy + 6, 2, 3, 90, 30, 40);
        drawPixelRect(r, sx + 9, sy + 4, 4, 4, 120, 40, 50);
        drawPixelRect(r, sx + 12, sy + 6, 2, 3, 90, 30, 40);
    }
}

void drawSkeleton(SDL_Renderer* r, float x, float y, Direction dir, int animFrame, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 18 < 0 || sx > INTERNAL_W) return;

    bool facingRight = (dir == Direction::RIGHT);
    Uint8 boneR = 220, boneG = 220, boneB = 210;
    Uint8 shadowR = 150, shadowG = 150, shadowB = 145;

    // Skull
    drawPixelRect(r, sx + 4, sy, 7, 7, boneR, boneG, boneB);
    int eyeX1 = facingRight ? (sx + 7) : (sx + 5);
    int eyeX2 = facingRight ? (sx + 9) : (sx + 7);
    drawPixelRect(r, eyeX1, sy + 2, 2, 2, 20, 10, 10); // Hollow eye sockets
    drawPixelRect(r, eyeX2, sy + 2, 2, 2, 20, 10, 10);
    drawPixelRect(r, eyeX1 + 1, sy + 2, 1, 1, 255, 30, 30); // Crimson soul pinpoint
    // Teeth / Jaw
    drawPixelRect(r, sx + 5, sy + 7, 5, 2, boneR, boneG, boneB);
    drawPixelRect(r, sx + 6, sy + 8, 1, 1, 20, 10, 10);
    drawPixelRect(r, sx + 8, sy + 8, 1, 1, 20, 10, 10);

    // Spine & Ribs
    drawPixelRect(r, sx + 7, sy + 9, 2, 7, boneR, boneG, boneB);
    drawPixelRect(r, sx + 4, sy + 10, 7, 1, boneR, boneG, boneB);
    drawPixelRect(r, sx + 4, sy + 12, 7, 1, boneR, boneG, boneB);
    drawPixelRect(r, sx + 5, sy + 14, 5, 1, shadowR, shadowG, shadowB);

    // Pelvis
    drawPixelRect(r, sx + 5, sy + 15, 5, 2, boneR, boneG, boneB);

    // Legs
    int legStep = (animFrame % 2 == 1) ? 2 : 0;
    drawPixelRect(r, sx + 5 + legStep, sy + 17, 2, 5, boneR, boneG, boneB);
    drawPixelRect(r, sx + 8 - legStep, sy + 17, 2, 5, boneR, boneG, boneB);

    // Bone weapon in hand
    int handX = facingRight ? (sx + 12) : (sx);
    drawPixelRect(r, handX, sy + 11, 3, 3, boneR, boneG, boneB);
    drawPixelRect(r, handX - 1, sy + 10, 5, 1, shadowR, shadowG, shadowB);
}

void drawFireSpirit(SDL_Renderer* r, float x, float y, int animFrame, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 14 < 0 || sx > INTERNAL_W) return;

    int flicker = (animFrame % 3);

    // Outer flame flare
    drawPixelRect(r, sx + 1 + flicker, sy, 8 - flicker, 3, 255, 70, 10);
    drawPixelRect(r, sx, sy + 3, 10, 6, 255, 140, 20);
    drawPixelRect(r, sx + 1, sy + 9, 8, 4, 255, 60, 10);

    // Inner bright core
    drawPixelRect(r, sx + 2, sy + 4, 6, 4, 255, 240, 60);
    drawPixelRect(r, sx + 3, sy + 5, 4, 2, 255, 255, 200);

    // Menacing glowing eyes inside fire
    drawPixelRect(r, sx + 2, sy + 5, 2, 2, 180, 20, 0);
    drawPixelRect(r, sx + 6, sy + 5, 2, 2, 180, 20, 0);
}

void drawBoss(SDL_Renderer* r, float x, float y, Direction dir,
              int animFrame, int bossPhase, int camX) {
    int sx = (int)x - camX;
    int sy = (int)y;
    if (sx + 36 < 0 || sx > INTERNAL_W) return;

    bool facingRight = (dir == Direction::RIGHT);

    // Phase colors: 1 = Dread Knight, 2 = Fiery Demon, 3 = Shadow Archon
    Uint8 helmR = 80, helmG = 20, helmB = 100;
    Uint8 capeR = 140, capeG = 15, capeB = 40;
    Uint8 glowR = 255, glowG = 60, glowB = 180;

    if (bossPhase == 2) {
        helmR = 130; helmG = 30; helmB = 20;
        capeR = 200; capeG = 70; capeB = 10;
        glowR = 255; glowG = 200; glowB = 30;
    } else if (bossPhase == 3) {
        helmR = 30; helmG = 10; helmB = 45;
        capeR = 90; capeG = 10; capeB = 120;
        glowR = 200; glowG = 50; glowB = 255;
    }

    // Horns / Crown
    drawPixelRect(r, sx + 6, sy, 3, 6, glowR, glowG, glowB);
    drawPixelRect(r, sx + 19, sy, 3, 6, glowR, glowG, glowB);
    drawPixelRect(r, sx + 8, sy + 2, 12, 4, 30, 20, 40);

    // Head / Face
    drawPixelRect(r, sx + 7, sy + 6, 14, 8, helmR, helmG, helmB);
    // Piercing Visor / Eyes
    int eyeX = facingRight ? (sx + 13) : (sx + 8);
    drawPixelRect(r, eyeX, sy + 9, 6, 2, glowR, glowG, glowB);

    // Huge Spiked Pauldrons (Shoulders)
    drawPixelRect(r, sx + 2, sy + 12, 7, 6, helmR + 30, helmG + 20, helmB + 20);
    drawPixelRect(r, sx + 19, sy + 12, 7, 6, helmR + 30, helmG + 20, helmB + 20);

    // Massive Torso / Heavy Armor
    drawPixelRect(r, sx + 8, sy + 14, 12, 10, helmR, helmG, helmB);
    // Dark core rune
    drawPixelRect(r, sx + 12, sy + 16, 4, 5, glowR, glowG, glowB);

    // Flowing Cape
    int capeX = facingRight ? (sx + 2) : (sx + 20);
    drawPixelRect(r, capeX, sy + 14, 6, 16 + (animFrame % 2) * 2, capeR, capeG, capeB);

    // Lower armor & armored legs
    drawPixelRect(r, sx + 8, sy + 24, 12, 3, 40, 25, 45);
    int legMove = (animFrame % 2 == 1) ? 2 : 0;
    drawPixelRect(r, sx + 8 + legMove, sy + 27, 4, 5, helmR, helmG, helmB);
    drawPixelRect(r, sx + 16 - legMove, sy + 27, 4, 5, helmR, helmG, helmB);

    // Giant Greatsword
    int swordX = facingRight ? (sx + 24) : (sx - 4);
    drawPixelRect(r, swordX, sy + 4, 4, 26, 220, 220, 240);
    drawPixelRect(r, swordX - 2, sy + 10, 8, 3, glowR, glowG, glowB); // Guard
}
