#ifndef H_PERIPH_
#define H_PERIPH_

struct canvas_t;
struct BITMAP;

void
periph_circ_draw(struct canvas_t *canvas, int canvasx, int canvasy, int radius,
                 int color, int angle);
void
periph_dot_draw(struct canvas_t *canvas, int canvasx, int canvasy, int radius,
                int color, int angle);

#endif

