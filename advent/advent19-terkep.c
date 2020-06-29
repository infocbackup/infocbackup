#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>


SDL_Surface *screen;


enum { MERET = 1<<8 }; /* legyen 2 hatvanya! */
typedef double Magassag[MERET][MERET];


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
void beallit(Magassag k, int x, int y, double szin) {
    if (szin < -1) szin = -1;
    if (szin > +1) szin = +1;
    k[y & (MERET - 1)][x & (MERET - 1)] = szin;
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
    for (lepes = MERET, maxelteres = 2; lepes > 1; lepes /= 2, maxelteres /= 2) {
        int fellepes = lepes / 2;
        /* negyzetek belseje */
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
    
    double min = magassag(k, 0, 0), max = magassag(k, 0, 0);
    for (y = 0; y < MERET; ++y)
        for (x = 0; x < MERET; ++x) {
            double m = magassag(k, x, y);
            if (m < min) min = m;
            if (m > max) max = m;
        }
    for (y = 0; y < MERET; ++y)
        for (x = 0; x < MERET; ++x)
            beallit(k, x, y, (magassag(k, x, y) - min) / (max-min) * 2 - 1);
}


/* szinatmenetet kepez az adott szintombon, c1 es c2 indexu szin kozott */
void atmenet(SDL_Color *p, int c1, int c2) {
    int x;
    for (x=c1+1; x<c2; x++) {
        p[x].r=p[c1].r+(p[c2].r-p[c1].r)*(x-c1)/(c2-c1);
        p[x].g=p[c1].g+(p[c2].g-p[c1].g)*(x-c1)/(c2-c1);
        p[x].b=p[c1].b+(p[c2].b-p[c1].b)*(x-c1)/(c2-c1);
    }
}


/* kiszamolja a megadott szin indexek kozott az atmenetet. */
void szineket_kiszamol(SDL_Color *pal) {
    int i;
    struct {
        int pos;
        SDL_Color col;
    } szinek[] = {
        //~ {  0,       {   0,   0, 0 } },      /* szurke szinekhez ezt kell */
        //~ {  255,       {   255,   255, 255 } },

        {  0,       {   0,   0, 192 } },        /* hegyekhez */
        { 32,       {   0,   0, 255 } },
        { 33,       {   0, 128,   0 } },
        { 140,      {   0,  80,   0 } },
        { 160,      {  64,  48,  10 } },
        { 224,      { 224, 224, 224 } },
        { 255,      { 255, 255, 255 } },

        //~ {  0,       {   0,   0, 255 } },      /* ebbol meg felhok lesznek */
        //~ {  255,       {   255,   255, 255 } },
    };
    for (i = 0; i<sizeof(szinek)/sizeof(szinek[0]); ++i)
        pal[szinek[i].pos] = szinek[i].col;
    for (i = 0; i<sizeof(szinek)/sizeof(szinek[0])-1; ++i)
        atmenet(pal, szinek[i].pos, szinek[i+1].pos);
}


/* kirajzolja a magassagterkepet a kapott palettaval */
void kirajzol(Magassag k, SDL_Color *pal) {
    int x, y;
    for (y = 0; y < MERET; ++y)
        for (x = 0; x < MERET; ++x) {
            int c = (k[y][x]+1)/2 * 255.0;
            pixelRGBA(screen, x, y, pal[c].r, pal[c].g, pal[c].b, 255);
        }
    SDL_Flip(screen);
}


/* foprogram */
int main(int argc, char *argv[]) {
    SDL_Color pal[256];
    Magassag k;
    SDL_Event ev;
    int kilep;
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(MERET, MERET, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    srand(time(NULL));
    szineket_kiszamol(pal);
    terkep_general(k);
    kirajzol(k, pal);

    kilep = 0;
    while (!kilep) {
        SDL_WaitEvent(&ev);
        switch (ev.type) {
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_SPACE) {
                    terkep_general(k);
                    kirajzol(k, pal);
                }
                break;
            case SDL_QUIT:
                kilep = 1;
                break;
        }
    }

    SDL_Quit();

    return 0;
}
