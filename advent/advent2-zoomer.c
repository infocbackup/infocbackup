/* Mandelbrot halmaz - mozgatasi es zoomolasi lehetoseggel.
 * A kurzorbillentyuk mozgatnak, a Q billentyu nagyit, az A
 * pedig kicsinyit (a kep kozeppontjahoz kepest). */
#include <SDL.h>
#include <stdio.h>

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


Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


void fraktal(SDL_Surface *screen) {
    int rajzol, quit;
    double ex = 0, ey = 0, n = screen->w / 4.0; /* eltolas es nagyitas */
    Uint8 *keystate;
    SDL_TimerID id;
    SDL_Surface *bmp;

    /* kep rajzolni */
    bmp = SDL_CreateRGBSurface(0, screen->w, screen->h, 32, 0, 0, 0, 0);
    /* billentyuk allapotait tartalmazo tomb */
    keystate = SDL_GetKeyState(NULL);
    rajzol = 1;
    quit = 0;
    id = SDL_AddTimer(40, idozit, NULL);
    while (!quit) {
        SDL_Event ev;

        /* megvarjuk es feldolgozzuk az elso esemenyt; feldolgozzuk a tobbi esemenyt is */
        SDL_WaitEvent(&ev);
        do {
            switch (ev.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;

                    /* ez az idozito. ha epp van lenyomva gomb,
                     * akkor valtozik a pozicio, es uj rajz is kell. */
                case SDL_USEREVENT:
                    if (keystate[SDLK_UP]) {
                        ey -= 2;
                        rajzol = 1;
                    }
                    if (keystate[SDLK_DOWN]) {
                        ey += 2;
                        rajzol = 1;
                    }
                    if (keystate[SDLK_RIGHT]) {
                        ex += 2;
                        rajzol = 1;
                    }
                    if (keystate[SDLK_LEFT]) {
                        ex -= 2;
                        rajzol = 1;
                    }
                    if (keystate[SDLK_q]) {
                        n *= 1.01;
                        ex *= 1.01;
                        ey *= 1.01;
                        rajzol = 1;
                    }
                    if (keystate[SDLK_a]) {
                        n /= 1.01;
                        ex /= 1.01;
                        ey /= 1.01;
                        rajzol = 1;
                    }
                    break;
            }
        } while (SDL_PollEvent(&ev));

        if (rajzol) {
            int y;
            /* nyolcasaval parhuzamositva a sorokat */
            #pragma omp parallel for schedule(dynamic, 8)
            for (y = 0; y < bmp->h; y++) {
                int x;
                Uint32 *sor = (Uint32 *)((char *) bmp->pixels + y * bmp->pitch);

                for (x = 0; x < bmp->w; x++) {
                    /* Mandelbrot: a konstanst a koordinata hatarozza meg; a kiindulasi ertek fix */
                    /*
                    double cr=(x + ex - bmp->w/2)/n;
                    double ci=(y + ey - bmp->h/2)/n;
                    double zr=0, zi=0;
                    */
                    /* Julia: a kiindulasi ertek a koordinata; a hozzaadott konstans fix */
                    double cr = -0.4, ci = 0.6;
                    double zr = (x + ex - bmp->w / 2) / n;
                    double zi = (y + ey - bmp->h / 2) / n;
                    SDL_Color c;
                    int i;

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
                    /* a "nyers" putpixel, hogy jo gyors legyen */
                    sor[x] = c.r << bmp->format->Rshift | c.g << bmp->format->Gshift | c.b << bmp->format->Bshift;
                }
            }
            SDL_BlitSurface(bmp, NULL, screen, NULL);
            SDL_Flip(screen);
            rajzol = 0;
        } /* rajzol */
    }
    SDL_RemoveTimer(id);
    SDL_FreeSurface(bmp);
}

int main(int argc, char *argv[]) {
    SDL_Surface *screen;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(480, 480, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    szineket_kiszamol();

    fraktal(screen);

    SDL_Quit();

    return 0;
}
