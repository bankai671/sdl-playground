#pragma once

#include <SDL.h>

typedef struct Game Game; 

typedef struct {
    const char* description;
} ExamineText;

typedef struct {
    const char* description;
    SDL_Color highlight_color;
} ExamineColored;

typedef struct {
    const char** pages;
    int page_count;
    int current_page;
} ExaminePages;

typedef enum {
    EXAMINE_TYPE_SIMPLE,
    EXAMINE_TYPE_FULLSCREEN
} ExamineType;

typedef struct {
	ExamineType type;
    void (*draw)(Game* g, SDL_Renderer* r, void* data);
    void (*input)(Game* g, void* data, int dir);
} ExamineHandler;

extern const ExamineHandler EXAMINE_HANDLER_TEXT;
extern const ExamineHandler EXAMINE_HANDLER_COLORED;
extern const ExamineHandler EXAMINE_HANDLER_PAGES;
extern const ExamineHandler EXAMINE_HANDLER_PENDANT;

