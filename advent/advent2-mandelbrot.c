/* Mandelbrot halmaz */
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

enum { MAXCOL = 255 };
SDL_Color pal[MAXCOL + 1];


/* szinatmenetet kepez az adott szintombon, c1 es c2 indexu szin kozott */
void atmenet(SDL_Color *p, int c1, int c2) {
    int x;
    for (x = c1 + 1; x < c2; x++) {
        p[x].r = p[c1].r + (p[c2].r - p[c1].r) * (x - c1) / (c2 - c1);
        p[x].g = p[c1].g + (p[c2].g - p[c1].g) * (x - c1) / (c2 - c1);
        p[x].b = p[c1].b + (p[c2].b - p[c1].b) * (x - c1) / (c2 - c1);
    }
}


/* kiszamolja a megadott szin indexek kozott az atmenetet. */
void szineket_kiszamol(void) {
    struct {
        int pos;
        SDL_Color col;
    } szinek[] = {
        {  0,       {   0,   0,   0 } },
        { 12,       {   0,   0, 255 } },
        { MAXCOL / 2, {   0, 192, 192 } },
        { MAXCOL,   { 255, 255, 255 } },
        { -1 }
    };
    int i;
    for (i = 0; szinek[i].pos != -1; ++i)
        pal[szinek[i].pos] = szinek[i].col;
    for (i = 0; szinek[i].pos != -1; ++i)
        atmenet(pal, szinek[i].pos, szinek[i + 1].pos);
}


int main(int argc, char *argv[]) {
    SDL_Surface *screen;
    SDL_Event ev;
    int x, y, i;
    enum { MERETX = 640, MERETY = 640 };

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(MERETX, MERETY, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    szineket_kiszamol();

    for (y = 0; y < MERETY; y++) {
        for (x = 0; x < MERETX; x++) {
            /* a konstanst a koordinata hatarozza meg */
            double cr = (x - MERETX / 2) / (double)MERETX * 4;
            double ci = (y - MERETY / 2) / (double)MERETY * 4;
            double zr = 0, zi = 0;
            SDL_Color c;

            /* addig, amig |z|<2, vagy tul sok iteracio nem volt */
            i = 0;
            while (i < MAXCOL && zr * zr + zi * zi < 4) {
                /* ez itt a z=z^2+c */
                double uzr = zr * zr - zi * zi + cr;
                double uzi = 2 * zr * zi + ci; /* http://napirajz.hu/files/zsak.jpg */
                zr = uzr;
                zi = uzi;
                i++;
            }
            /* az iteraciok szama hatarozza meg a keppont szinet */
            c = i > MAXCOL ? pal[MAXCOL] : pal[i];
            pixelRGBA(screen, x, y, c.r, c.g, c.b, 255);
        }
    }
    SDL_Flip(screen);

    while (SDL_WaitEvent(&ev) && (ev.type != SDL_QUIT && ev.type != SDL_KEYDOWN)) {
        /* csak var */
    }
    SDL_Quit();

    return 0;
}
