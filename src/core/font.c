#include "font.h"
#include <string.h>

static const uint8_t font_bits[][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00}, // 0:  ' '
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // 1:  '!'
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 2:  '0'
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 3:  '1' 
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 4:  '2' 
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 5:  '3' 
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 6:  '4' 
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 7:  '5'
    {0x7c, 0x12, 0x11, 0x12, 0x7c}, // 8:  'A' 
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // 9:  'B' 
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // 10: 'C'
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // 11: 'D'
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // 12: 'E'
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // 13: 'F'
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // 14: 'G'
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // 15: 'H'
    {0x41, 0x41, 0x7f, 0x41, 0x41}, // 16: 'I'
    {0x40, 0x40, 0x7e, 0x00, 0x00}, // 17: 'J'
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // 18: 'K'
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // 19: 'L'
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // 20: 'M'
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 21: 'N'
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // 22: 'O'
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // 23: 'P'
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // 24: 'Q'
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // 25: 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 26: 'S'
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // 27: 'T'
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 28: 'U'
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // 29: 'V'
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // 30: 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 31: 'X'
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 32: 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 33: 'Z'
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 34: '-'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 35: ':'
};

static int get_glyph_index(char ch) 
{
    if (ch == ' ')  return 0;
    if (ch == '!')  return 1;
    if (ch >= '0' && ch <= '5') return 2 + (ch - '0');
    if (ch >= 'A' && ch <= 'Z') return 8 + (ch - 'A');
    if (ch == '-')  return 34;
    if (ch == ':')  return 35;
    return 0;
}

void draw_text(SDL_Renderer* r, const char* text, int x, int y, int scale, SDL_Color color)
{
    if (!font_texture) return;

    SDL_SetTextureColorMod(font_texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(font_texture, color.a);

    int cur_x = x;

    for (size_t i = 0; i < strlen(text); i++) {
        char ch = text[i];
        if (ch >= 'a' && ch <= 'z') ch -= 32;

        int glyph_idx = get_glyph_index(ch);

        SDL_Rect src_rect = { glyph_idx * 6, 0, 5, 7 };

        SDL_Rect dst_rect = { cur_x, y, 5 * scale, 7 * scale };

        SDL_RenderCopy(r, font_texture, &src_rect, &dst_rect);

        cur_x += 6 * scale;
    }}

SDL_Texture* font_texture = NULL;

void init_font(SDL_Renderer* r) {
    font_texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 216, 7);
    SDL_SetTextureBlendMode(font_texture, SDL_BLENDMODE_BLEND);

    Uint32* pixels;
    int pitch;
    SDL_LockTexture(font_texture, NULL, (void**)&pixels, &pitch);

    for (int i = 0; i < 216 * 7; i++) pixels[i] = 0x00000000;

    for (int glyph = 0; glyph < 36; glyph++) {
        for (int col = 0; col < 5; col++) {
            uint8_t bits = font_bits[glyph][col];
            for (int row = 0; row < 7; row++) {
                if (bits & (1 << row)) {
                    int tex_x = (glyph * 6) + col;
                    int tex_y = row;
                    pixels[tex_y * (pitch / 4) + tex_x] = 0xFFFFFFFF;
                }
            }
        }
    }

    SDL_UnlockTexture(font_texture);
}

void free_font() {
    if (font_texture) SDL_DestroyTexture(font_texture);
}
