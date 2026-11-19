#pragma once
#include "types.h"

// ── Basic Drawing ──────────────────────────────────────────────────────
void drawPixelRect(SDL_Renderer* r, int x, int y, int w, int h,
                   Uint8 red, Uint8 green, Uint8 blue, Uint8 a = 255);

// ── Player (16×24) ─────────────────────────────────────────────────────
void drawPlayer(SDL_Renderer* r, float x, float y, Direction dir,
                int animFrame, bool attacking, int attackTimer,
                bool hurt, bool dead, int camX);

// ── Enemies ────────────────────────────────────────────────────────────
void drawSlime(SDL_Renderer* r, float x, float y, int animFrame, int camX);
void drawBat(SDL_Renderer* r, float x, float y, int animFrame, int camX);
void drawSkeleton(SDL_Renderer* r, float x, float y, Direction dir,
                  int animFrame, int camX);
void drawFireSpirit(SDL_Renderer* r, float x, float y, int animFrame, int camX);
void drawBoss(SDL_Renderer* r, float x, float y, Direction dir,
              int animFrame, int bossPhase, int camX);

// ── Tiles (16×16) ──────────────────────────────────────────────────────
void drawTile(SDL_Renderer* r, TileType type, int tileX, int tileY,
              int camX, int levelNum);

// ── Collectibles ───────────────────────────────────────────────────────
void drawGem(SDL_Renderer* r, float x, float y, int camX);
void drawHealthPotion(SDL_Renderer* r, float x, float y, int camX);
void drawExtraLife(SDL_Renderer* r, float x, float y, int camX);

// ── Projectiles & Platforms ────────────────────────────────────────────
void drawProjectile(SDL_Renderer* r, float x, float y, float w, float h,
                    Uint8 red, Uint8 green, Uint8 blue, int camX);
void drawMovingPlatform(SDL_Renderer* r, float x, float y, float w, int camX);
