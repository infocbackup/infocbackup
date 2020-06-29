#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

SDL_Surface *screen;


enum { MERET = 1<<7 }; /* legyen 2 hatvanya! */
typedef double Magassag[MERET][MERET];

typedef struct Pont {
    double x, y, z;     /* eredeti 3d koordináta */
    double xf, yf, zf;  /* forgatott 3d koordináta */
    int xk, yk;         /* képernyőn koordináta */
} Pont;

typedef struct Negyszog {
    Pont *sarkok[4];
    Uint32 szin;
} Negyszog;


/* veletlenszam -1 es +1 kozott */
double frand(void) {
    return rand()/(double)RAND_MAX * 2 - 1;
}


/* kiolvassa a magassagterkeprol az adott koordinatat.
 * x es y koordinata a ket oldalt "tulpordul" */
double magassag(Magassag k, int x, int y) {
    return k[y & (MERET - 1)][x & (MERET - 1)];
}
 

/* beallitja az adott koordinatan a magassagot.
 * x es y ket oldalt "tulpordul" */
void beallit(Magassag k, int x, int y, double eltol) {
    if (eltol<-1) eltol=-1;
    if (eltol>1) eltol=1;
    k[y & (MERET - 1)][x & (MERET - 1)] = eltol;
}


/* negyzet alaku atlagolas. az x, y a cel koordinata, a
 * meret azt mondja meg, hogy mekkora negyzetbol vegye az atlagot */
void negyzet(Magassag k, int x, int y, int meret, double maxelteres) {
    int fel = meret / 2;
    double a = magassag(k, x - fel, y - fel);
    double b = magassag(k, x + fel, y - fel);
    double c = magassag(k, x - fel, y + fel);
    double d = magassag(k, x + fel, y + fel);
 
    beallit(k, x, y, (a + b + c + d)/4.0 + frand()*maxelteres);
}
 

/* gyemant alaku atlagolas (45 fokkal elforgatott negyzet).
 * az x, y a cel koordinata, a meret azt mondja, mekkora negyzetbol veszi az atlagot */
void gyemant(Magassag k, int x, int y, int meret, double maxelteres) {
    int fel = meret / 2;
    double a = magassag(k, x - fel, y);
    double b = magassag(k, x + fel, y);
    double c = magassag(k, x, y - fel);
    double d = magassag(k, x, y + fel);
 
    beallit(k, x, y, (a + b + c + d)/4.0 + frand()*maxelteres);
}


/* Letrehoz egy uj, veletlenszeru terkepet */
void terkep_general(Magassag k) {
    int x, y;
    int lepes;
    double maxelteres;
    
    beallit(k, 0, 0, 0);  /* kiindulopont - legyen mindig kozepmagassag */

    /* mindig megfelezve a darabokat, ujabb pontokat generalunk */
    for (lepes = MERET, maxelteres = 1; lepes > 1; lepes /= 2, maxelteres /= 2) {
        int fellepes = lepes / 2;
        /* negyszogek belseje */
        for (y = fellepes; y < MERET + fellepes; y += lepes)
            for (x = fellepes; x < MERET + fellepes; x += lepes)
                negyzet(k, x, y, lepes, maxelteres);
        /* gyemantok belseje */
        for (y = 0; y < MERET; y += lepes)
            for (x = 0; x < MERET; x += lepes) {
                gyemant(k, x + fellepes, y, lepes, maxelteres);
                gyemant(k, x, y + fellepes, lepes, maxelteres);
            }
    }
}


/* szinatmenetet kepez az adott szintombon, c1 es c2 indexu szin kozott */
void atmenet(SDL_Color p[], int c1, int c2) {
    int x;
    for (x=c1+1; x<c2; x++) {
        p[x].r=p[c1].r+(p[c2].r-p[c1].r)*(x-c1)/(c2-c1);
        p[x].g=p[c1].g+(p[c2].g-p[c1].g)*(x-c1)/(c2-c1);
        p[x].b=p[c1].b+(p[c2].b-p[c1].b)*(x-c1)/(c2-c1);
    }
}


/* kiszamolja a megadott szin indexek kozott az atmenetet. */
void szineket_kiszamol(SDL_Color pal[]) {
    struct {
        int pos;
        SDL_Color col;
    } szinek[] = {
        {  0,       {   0,   0, 192 } },        /* hegyekhez */
        { 32,       {   0,   0, 255 } },
        { 33,       {   0, 128,   0 } },
        { 140,      {   0,  80,   0 } },
        { 160,      {  64,  48,  10 } },
        { 224,      { 224, 224, 224 } },
        { 255,      { 255, 255, 255 } },
    };
    int i;
    for (i = 0; i<sizeof(szinek)/sizeof(szinek[0]); ++i)
        pal[szinek[i].pos] = szinek[i].col;
    for (i = 0; i<sizeof(szinek)/sizeof(szinek[0])-1; ++i)
        atmenet(pal, szinek[i].pos, szinek[i+1].pos);
}


/* uj hegyet general. ehhez keszit egy uj terkepet, es abbol kiindulva
 * meghatarozza a magassagokat */
void hegy_general(Pont *pontok, Negyszog *negyszogek, SDL_Color pal[]) {
    Magassag *k;
    int x, y;
    
    /* itt most ilyen furcsan foglalodik a 2d tomb, egyszerre. */
    /* csak azert kell igy, mert ha tul nagy, nem fer a verembe! */
    k = (Magassag *) malloc(sizeof(Magassag));

    terkep_general(*k);

    for (y = 0; y < MERET; ++y)
        for (x = 0; x < MERET; ++x) {
            int i = y*MERET + x;
            pontok[i].x = 500 * (x / (double)MERET - 0.5);
            pontok[i].y = 128 * ((*k)[y][x]) - 32;
            pontok[i].z = 500 * (y / (double)MERET - 0.5);
        }
    for (y = 0; y < MERET-1; ++y)
        for (x = 0; x < MERET-1; ++x) {
            int i = y*(MERET-1) + x;
            int c = (magassag(*k, x, y) + frand()/32)*127+128;
            if (c < 0) c = 0;
            if (c > 255) c = 255;
            negyszogek[i].sarkok[0] = &pontok[y*MERET+x];
            negyszogek[i].sarkok[1] = &pontok[y*MERET+x+1];
            negyszogek[i].sarkok[2] = &pontok[(y+1)*MERET+x+1];
            negyszogek[i].sarkok[3] = &pontok[(y+1)*MERET+x];
            negyszogek[i].szin = pal[c].r<<24 | pal[c].g<<16 | pal[c].b<<8 | 0xFF;
        }
    free(*k);
}


/* qsort() osszehasonlito fuggveny: a negyszogeket a forgatott koordinatak
 * szerint, csokkeno tavolsag szerint rendezi */
int negyszogrendez(void const *vn1, void const *vn2) {
    Negyszog const *n1 = (Negyszog const *) vn1;
    Negyszog const *n2 = (Negyszog const *) vn2;
    if (n1->sarkok[0]->zf < n2->sarkok[0]->zf)
        return +1;
    if (n1->sarkok[0]->zf > n2->sarkok[0]->zf)
        return -1;
    return 0;
}


/* kirajzol egy negyszogeet, amit az altala hivatkozott negy pont
 * kepernyokoordinatai hataroznak meg */
void negyszog_rajzol(Negyszog n) {
    Sint16 x[4] = { n.sarkok[0]->xk, n.sarkok[1]->xk, n.sarkok[2]->xk, n.sarkok[3]->xk },
           y[4] = { n.sarkok[0]->yk, n.sarkok[1]->yk, n.sarkok[2]->yk, n.sarkok[3]->yk };
    filledPolygonColor(screen, x, y, 4, n.szin);
    /* aapolygonColor(screen, x, y, 4, 0x40); */    /* racs */
}


/* ez vegzi a hegy kirajzolasat: elobb elforgatja oket fx, fy szoggel,
 * utana sorbarendezi, vegul pedig kirajzolj a negyszogeket */
void kirajzol(Pont pontok[], Negyszog negyszogek[], double fx, double fy) {
    int i;

    boxRGBA(screen, 0, 0, screen->w, screen->h, 0, 0, 0, 255);

    /* forgatott koordináták számolása */
    for (i = 0; i < MERET*MERET; ++i) {
        Pont p1 = pontok[i];
        Pont p2 = { p1.x*cos(fy)-p1.z*sin(fy), p1.y, p1.x*sin(fy)+p1.z*cos(fy) };
        Pont p3 = { p2.x, p2.y*cos(fx)-p2.z*sin(fx), p2.y*sin(fx)+p2.z*cos(fx) };
        pontok[i].xf = p3.x;
        pontok[i].yf = p3.y;
        pontok[i].zf = p3.z;
        pontok[i].xk = screen->w/2 + 600 * pontok[i].xf / (pontok[i].zf+600);
        pontok[i].yk = screen->h/2 - 600 * pontok[i].yf / (pontok[i].zf+600);
    }
    /* rendezés és kirajzolás */
    qsort(negyszogek, (MERET-1)*(MERET-1), sizeof(negyszogek[0]), negyszogrendez);
    for (i = 0; i<(MERET-1)*(MERET-1); ++i)
        negyszog_rajzol(negyszogek[i]);

    SDL_Flip(screen);
}


Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


int main(int argc, char *argv[]) {
    /* fix a meret, de dinamikusan kell foglalni, mert nem fernek a verembe */
    Pont *pontok;
    Negyszog *negyszogek;
    SDL_Color pal[256];
    SDL_Event ev;
    Uint8 *keystate;
    SDL_TimerID id;
    double fx = -0.5, fy = -0.1;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(800, 600, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    srand(time(NULL));
    szineket_kiszamol(pal);
    pontok = (Pont *) malloc(sizeof(Pont)*MERET*MERET);
    negyszogek = (Negyszog *) malloc(sizeof(Negyszog)*(MERET-1)*(MERET-1));
    hegy_general(pontok, negyszogek, pal);

    id = SDL_AddTimer(80, idozit, NULL);
    keystate = SDL_GetKeyState(NULL);

    while (SDL_WaitEvent(&ev) && (ev.type!=SDL_QUIT)) {
        switch (ev.type) {
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym==SDLK_SPACE)
                    hegy_general(pontok, negyszogek, pal);
                break;

            case SDL_USEREVENT:
                kirajzol(pontok, negyszogek, fx, fy);

                /* gombnyomások? */
                if (keystate[SDLK_UP])
                    fx+=0.1;
                if (keystate[SDLK_DOWN])
                    fx-=0.1;
                if (keystate[SDLK_LEFT])
                    fy-=0.1;
                if (keystate[SDLK_RIGHT])
                    fy+=0.1;
                break;
            
        }
    }
    SDL_RemoveTimer(id);

    SDL_Quit();
    
    free(pontok);
    free(negyszogek);

    return 0;
}
