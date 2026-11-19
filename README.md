# castlevaniaclone

A gothic 2D action platformer built with **C++17** and **SDL2**. All visual sprites and assets are rendered programmatically using pure pixel art blocks (no external image files required).

---

## 🎮 Game Features

- **Action Platformer Combat**: Fluid movement, jumping, sword slashing with directional hitboxes, knockback, screen shake, and damage invulnerability flashing.
- **Pure Pixel Art**: Every visual asset (player knight, slimes, bats, skeletons, fire spirits, the multi-phase boss, platforms, spikes, lava, gems, potions, and fonts) is rendered directly using colored pixel rectangles.
- **5 Increasing Difficulty Levels**:
  1. **Level 1: Forest Ruins (Tutorial)** — Gentle platforming with live tutorial banners guiding controls, jumping, sword combat, collecting gems, and reaching the exit door. Enemies: Forest Slimes.
  2. **Level 2: Dark Caves** — Cave stalactites, pit traps with spikes, narrower jumps. Enemies: Cave Slimes + Swooping Bats in sine wave trajectories.
  3. **Level 3: Castle Halls** — Moving mechanical platforms, high battlements, hanging chains. Enemies: Skeletons throwing bones + Bats + Slimes. Unlocks the Special Slash attack.
  4. **Level 4: Lava Depths** — Treacherous pools of instant-death boiling lava, requiring timing jumps on multiple moving platforms. Enemies: Fire Spirits firing fireballs + Skeletons + Bats.
  5. **Level 5: Dark Tower (The Gauntlet & Boss Arena)** — Precision platforming followed by the throne room of the Dread Archon. Multi-phase boss fight with changing combat behaviors and attack spreads.
- **Interactive Retro UI**:
  - Embedded 5×7 custom pixel font bitmap engine (no TTF dependencies).
  - Health hearts, lives counter, score tracking, stage indicator, and real-time tutorial banners.
  - Title screen with menu selection, pause overlay, level complete screen, game over, and victory screens.

---

## 🕹️ Controls

| Action | Primary Key | Alternate Key |
|---|---|---|
| **Move Left / Right** | `←` / `→` (Arrow Keys) | `A` / `D` |
| **Jump** | `Space` | `W` / `↑` |
| **Attack (Sword Slash)** | `Z` | `J` |
| **Special Attack (Lvl 3+)**| `X` | `K` |
| **Pause Game** | `ESC` | — |
| **Select / Confirm Menu** | `Enter` | `Space` / `Z` |

---

## 🚀 How to Run the Game

The game binary `shadow_knight` is already compiled and ready in the project root:

```bash
cd /home/pedroeduardo/Documentos/castlvaniaclone
./shadow_knight
```

---

## 🛠️ Building from Source

### Using Make:
```bash
make
```

### Using CMake:
```bash
cmake -B build
cmake --build build
```

---

## 📁 Source Architecture

```
├── CMakeLists.txt     # CMake configuration
├── Makefile           # Direct Makefile build
├── README.md          # Project documentation
├── shadow_knight      # Compiled executable
└── src/
    ├── types.h        # Constants, enums, collision helpers, resolution definitions
    ├── renderer.h     # Pixel art rendering declarations
    ├── renderer.cpp   # Pixel art drawing routines for sprites, tiles, and collectibles
    ├── player.h       # Player class interface (physics, combat, animation)
    ├── player.cpp     # Player implementation (AABB collision, sword hitboxes)
    ├── enemy.h        # Enemy class hierarchy (Slimes, Bats, Skeletons, Fire Spirits, Boss)
    ├── enemy.cpp      # Enemy AI, patrol routines, projectile mechanics, boss phases
    ├── level.h        # Level generator interface & tile definitions
    ├── level.cpp      # 5 handcrafted levels, moving platforms, tutorial triggers
    ├── game.h         # Game engine, states, 5x7 pixel font, particle system
    ├── game.cpp       # Game loop, state machine, event handling, HUD rendering
    └── main.cpp       # Application entry point
```
