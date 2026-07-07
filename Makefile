CC = gcc

CFLAGS = -Wall -Wextra -std=c11 \
		 -I./src \
         $(shell pkg-config --cflags sdl2) \

LDFLAGS = \
         $(shell pkg-config --libs sdl2) \
		 -lm 

C_SRC = main.c \
	  core/font.c \
	  game/game.c \
	  game/combat.c \
	  game/ai.c \
	  game/entity_factory.c \
	  game/world_factory.c \
	  game/inventory.c \
	  game/inventory_utils.c \
	  game/examine_system.c \
	  game/item_use_system.c \
	  game/item_db.c \
	  engine/engine.c \
	  input/input.c \
	  render/render.c \
	  render/render_world.c \
	  render/render_ui.c

C_SRCS = $(addprefix src/, $(C_SRC))

OUT = ./build/main

all:
	mkdir -p ./build
	$(CC) $(CFLAGS) $(C_SRCS) -o $(OUT) $(LDFLAGS)

run: all
	@clear
	@./$(OUT)

