#ifndef RESIZE_H_
#define RESIZE_H_

void resize_bitmap(BITMAP *src, BITMAP *dest);
BITMAP* reduce_mipmap(BITMAP *src, int factorx, int factory);

#endif

