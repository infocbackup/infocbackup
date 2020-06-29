#include <SDL_gfxPrimitives.h>
#include <SDL.h>
#include <stdlib.h>
#include <time.h>

int const w = 1024, h = 768;       /* ablak méretei */
double const fold = 7.0/8.0;      /* a talaj helye */

/* a fák alakjait, színét itt lehet állítani */
typedef struct Intervallum {
    double min, max;
} Intervallum;

Intervallum const doles = { 0.3, 0.5 };
Intervallum const helyzet = { 0.4, 0.6 };
Intervallum const helyzetfelso = { 0.4, 0.6 };
Intervallum const rovidules = { 0.2, 0.5 };
/* a középső ág (tulajdonképpen a törzs) rövidülése. erősen befolyásolja a magasságot! */
Intervallum const roviduleskozepso = { 0.2, 0.3 };
Intervallum const szinrandom = { 0.7, 0.9 };
Intervallum const szinfuzzy = { -0.1, 0.1 };
Intervallum const dolesrand = { -0.1, 0.1 };
Intervallum const meret = { 60, 80 };
int const melyseg = 8;

/* egy kiinduló ág adatai */
typedef struct AgAdat {
    double doles;       /* merre dől az eredeti ághoz képest (radián) */
    double helyzet;     /* honnan indul ki (0 = legalul, 1 = legfelül) */
    double rovidules;   /* mennyit rövidül (0 = semennyit, 1 = lenullázódik) */
} AgAdat;


/* ad egy véletlenszámot az adott intervallumból */
double myrand(Intervallum i) {
    double rand1 = rand() / (double) RAND_MAX;  /* ez 0 és 1 között lesz */
    return i.min + (i.max-i.min)*rand1;         /* beskálázva min és max közé */
}


/* húz egy egymást átfedő körökből álló vonalat. az sdl_gfx thickline függvénye bugos.
 * ez pedig jobban kezeli a tört szám sugarat is. */
void myline(SDL_Surface *screen, double x1, double y1, double x2, double y2, Uint32 color, double r) {
    double dx = x2-x1, dy = y2-y1;
    double lepes = sqrt(dx*dx + dy*dy)/r;
    if (lepes != 0) {
        dx /= lepes;
        dy /= lepes;
    }
    for (double i = 0; i <= lepes; ++i)
        filledCircleColor(screen, x1 + dx*i, y1 + dy*i, r, color);
}


/* ez rajzolja a fa ágait. agak[agszam] tömb adatai alapján, n lépés van vissza a rekurzióból. */
void fat_rajzol(SDL_Surface *screen, AgAdat agak[], int agszam, int n,
                double x, double y,
                double hossz, double szog,
                double atlatszosag, double zold, double r)
{
    if (n < 0)
        return;
    int atlb = atlatszosag * 255.0, vorosb = atlb * 2/3, zoldb = (0xFF-atlb) * (zold + myrand(szinfuzzy));
    myline(screen, x, y, x+hossz*cos(szog), y-hossz*sin(szog), vorosb<<24 | zoldb<<16 | 0<<8 | atlb<<0, r);
    for (int i = 0; i < agszam; ++i) {
        fat_rajzol(screen, agak, agszam, n-1,
                   x + hossz*agak[i].helyzet*cos(szog), y - hossz*agak[i].helyzet*sin(szog),
                   hossz * (1-agak[i].rovidules), szog + agak[i].doles + myrand(dolesrand),
                   atlatszosag * 0.8, zold, r * 0.88);
    }
    /* ha ez benne van, látszik a rajzolás! */
    /* if (n == 1) SDL_Flip(screen); */
}


/* random fát rajzol x, y helyről indulva. */
void randomfa(SDL_Surface *screen, double x, double y) {
    enum { AGSZAM = 3 };
    AgAdat agak[AGSZAM];
    double d1 = -myrand(doles), d2 = myrand(doles);
    for (int i = 0; i < AGSZAM; ++i) {
        agak[i].doles = d1 + (d2-d1)/(AGSZAM-1)*i;  /* nem random, hanem d1-tol d2-ig */
        agak[i].helyzet = myrand(helyzet);
        agak[i].rovidules = myrand(rovidules);
    }
    agak[AGSZAM/2].helyzet = 1.0;   /* hogy a törzs (középső ág) mindig nőjön tovább */
    agak[AGSZAM/2].rovidules = myrand(roviduleskozepso);
    fat_rajzol(screen, agak, AGSZAM, melyseg, x, y, myrand(meret), 90*3.1415/180, 1, myrand(szinrandom), 3);
    SDL_Flip(screen);
}


/* letörli az ablakot: megrajzolja az eget és a talajt. */
void clear(SDL_Surface *screen) {
    boxColor(screen, 0, 0, w-1, h-1, 0x4040FFFF);
    boxColor(screen, 0, h*fold, w-1, h-1, 0x308030FF);
    SDL_Flip(screen);
}


int main(int argc, char *argv[]) {
    /* mindenféle inicializálás */
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Surface *screen = SDL_SetVideoMode(w, h, 0, SDL_ANYFORMAT | SDL_FULLSCREEN);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    clear(screen);

    /* eseményvezérelt */
    int quit = 0;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                randomfa(screen, event.button.x, h*fold);
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE)
                    clear(screen);
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
                break;
        }
    }
    
    SDL_Quit();
    return 0;
}
