CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Isrc
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)

SRCS := src/main.cpp \
        src/renderer.cpp \
        src/player.cpp \
        src/enemy.cpp \
        src/level.cpp \
        src/game.cpp

OBJS := $(SRCS:.cpp=.o)
TARGET := shadow_knight

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
