#ifndef SCREEN_H_
#define SCREEN_H_

struct BITMAP;

int screen_init(int width, int height, int depth_bits, int windowed);
void screen_draw(void);
extern struct BITMAP *screen_buf;

#endif

