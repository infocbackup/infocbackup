#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#define MERETX 35
#define MERETY 25

/* egy cella merete */
#define PIXEL 16
/* kesleltetes */
#define SLEEP 50

typedef enum Cella { Fal, Jarat, Kijarat, Zsakutca, Kerdeses } Cella;
typedef Cella Palya[MERETY][MERETX];

/* globális tömb a színeknek */
struct {
    unsigned r, g, b;
} szinek[] = {
    { 255, 255, 255 }, /* Fal */
    { 0,   0,   0   }, /* Jarat */
    { 0,   255, 0   }, /* Kijarat */
    { 255, 0,   0   }, /* Zsakutca */
    { 0,   0,   255 }, /* Kerdeses */
};

/* globális változó: képernyő */
SDL_Surface *screen;


void rajzol(Palya p, int x, int y, Cella mit) {
    p[y][x]=mit;
    boxRGBA(screen, x*PIXEL, y*PIXEL, x*PIXEL+PIXEL-1, y*PIXEL+PIXEL-1, 
        szinek[mit].r, szinek[mit].g, szinek[mit].b, 255);
}


/* az egesz palyat Fallal tolti ki */
void ures(Palya p) {
    int x, y;

    for (y=0; y<MERETY; ++y)
        for (x=0; x<MERETX; ++x)
            rajzol(p, x, y, Fal);
    SDL_Flip(screen);
}


void labirintus(Palya p, int x, int y) {
    typedef enum { fel, le, jobbra, balra } Irany;
    Irany iranyok[4]={fel, le, jobbra, balra};
    int i;

    /* erre a pontra hivtak minket, ide lerakunk egy darabka Jaratot. */
    rajzol(p, x, y, Jarat);
    /* kicsit varunk, hogy viccesen nezzen ki */
    SDL_Flip(screen);
    SDL_Delay(SLEEP);

    /* a tomb keverese */
    for (i=0; i<4; ++i) {           /* mindegyiket... */
        int r=rand()%4;             /* egy veletlenszeruen valasztottal... */
        Irany temp=iranyok[i];      /* megcsereljuk. */
        iranyok[i]=iranyok[r];
        iranyok[r]=temp;
    }

    /* a kevert iranyok szerint mindenfele probalunk menni, ha lehet. */
    for (i=0; i<4; ++i)
        switch (iranyok[i]) {
            case fel:
                if (y>=2 && p[y-2][x]!=Jarat) {
                    rajzol(p, x, y-1, Jarat); /* elinditjuk arrafele a Jaratot */
                    labirintus(p, x, y-2);  /* es rekurzive megyunk tovabb */
                }
                break;
            case balra:
                if (x>=2 && p[y][x-2]!=Jarat) {
                    rajzol(p, x-1, y, Jarat);
                    labirintus(p, x-2, y);
                }
                break;
            case le:
                if (y<MERETY-2 && p[y+2][x]!=Jarat) {
                    rajzol(p, x, y+1, Jarat);
                    labirintus(p, x, y+2);
                }
                break;
            case jobbra:
                if (x<MERETX-2 && p[y][x+2]!=Jarat) {
                    rajzol(p, x+1, y, Jarat);
                    labirintus(p, x+2, y);
                }
                break;
        }
}


/* igazzal ter vissza, ha arrafele megtalalta,
 * hamissal, ha nem */
int megfejt(Palya p, int x, int y, int celx, int cely) {
    int megtalalt;

    /* egyelore nem tudjuk, itt mi van, de jeloljuk meg,
     * hogy ide mar ne lepjunk. */
    rajzol(p, x, y, Kerdeses);
    SDL_Flip(screen);
    SDL_Delay(SLEEP);

    /* egyelore nem talaljuk a Kijaratot */
    megtalalt=0;

    /* pont a celnal allunk? */
    if (x==celx && y==cely)
        megtalalt=1;

    /* ha meg nem talaltuk meg a Kijaratot... ES ha tudunk jobbra menni... */
    if (!megtalalt && x<MERETX-1 && p[y][x+1]==Jarat) {
        /* ha arra van a megfejtes */
        if(megfejt(p, x+1, y, celx, cely))
            megtalalt=1;
    }
    /* balra */
    if (!megtalalt && x>0 && p[y][x-1]==Jarat) {
        if (megfejt(p, x-1, y, celx, cely))
            megtalalt=1;
    }
    if (!megtalalt && y>0 && p[y-1][x]==Jarat) {
        if (megfejt(p, x, y-1, celx, cely))
            megtalalt=1;
    }
    if (!megtalalt && y<MERETY-1 && p[y+1][x]==Jarat) {
        if (megfejt(p, x, y+1, celx, cely))
            megtalalt=1;
    }

    /* a hivottakbol kiderult, hogy ez a Kijarathoz
     * vezeto ut, vagy nem. */
    rajzol(p, x, y, megtalalt ? Kijarat:Zsakutca);
    SDL_Flip(screen);
    SDL_Delay(SLEEP);

    /* jelezzuk a hivonak, hogy valahonnan errefele indulva
     * lehet-e Kijaratot talalni */
    return megtalalt;
}


int main(void) {
    Palya p;
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(MERETX*PIXEL, MERETY*PIXEL, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Labirintus", "Labirintus");
    srand(time(0));

    /* generalas */
    ures(p);
    labirintus(p, 1, 1);
    rajzol(p, 0, 1, Jarat);
    rajzol(p, MERETX-1, MERETY-2, Jarat);

    SDL_Delay(1000);

    /* megfejtes */
    megfejt(p, 0, 1, MERETX-1, MERETY-2);

    SDL_Delay(5000);

    return 0;
}
