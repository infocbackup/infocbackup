#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>

SDL_Surface *screen;

/* Teknoc interfesz */
typedef struct Teknoc {
    double x, y;   /* pozicio */
    double szog;   /* szog */
    int rajzol;    /* rajzol-e mozgas kozben */
    unsigned long szin;      /* szin */
} Teknoc;

/* inicializal egy teknocot: kepernyo kozepe, jobbra nez, nem rajzol */
void teknoc_init(Teknoc *t);
/* beallitja a ceruza szinet, rgb: voros, zold, kek komponensek 0..255 */
void teknoc_szin(Teknoc *t, Uint8 r, Uint8 g, Uint8 b);
/* leteszi a ceruzat */
void teknoc_rajzol(Teknoc *t);
/* felemeli a ceruzat */
void teknoc_nemrajzol(Teknoc *t);
/* elore lep valahany keppontot (kozben rajzolhat) */
void teknoc_elore(Teknoc *t, double r);
/* balra fordul valahany fokkal */
void teknoc_fordul(Teknoc *t, double dszog);
/* teleportal az adott kepernyokoordinatakra es iranyba */
void teknoc_teleport(Teknoc *t, double ujx, double ujy, double ujszog);


/* Foprogram */
void fraktal(Teknoc *t, int mennyi, double mekkora) {
    if (mennyi==0)
        teknoc_elore(t, mekkora);
    else {
        fraktal(t, mennyi-1, mekkora);
        teknoc_fordul(t, 60);
        fraktal(t, mennyi-1, mekkora);
        teknoc_fordul(t, -120);
        fraktal(t, mennyi-1, mekkora);
        teknoc_fordul(t, 60);
        fraktal(t, mennyi-1, mekkora);
    }
}

void sokszog(Teknoc *t, int csucsok, double oldalhossz) {
    int i;
    
    for (i=0; i<csucsok; ++i) {
        teknoc_elore(t, oldalhossz);
        teknoc_fordul(t, 360.0/csucsok);
    }
}

int main(int argc, char *argv[]) {
    Teknoc t;
    int i;
    SDL_Event ev;
    
    /* SDL inicializálása és ablak megnyitása */
    SDL_Init(SDL_INIT_VIDEO);
    screen=SDL_SetVideoMode(512, 512, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Teknocgrafika", "Teknocgrafika");
    boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0xFFFFFFFF);
 
    /* rajzolás */
    teknoc_init(&t);
    teknoc_rajzol(&t);
    
    /* spiral */
    teknoc_teleport(&t, 128, 128, 0);
    for (i=0; i<128; ++i) {
        teknoc_szin(&t, i*2, i*2, 255);
        teknoc_elore(&t, i*1.5);
        teknoc_fordul(&t, 89);
    }

    /* fraktalok */
    teknoc_szin(&t, 255, 0, 0);
    teknoc_teleport(&t, 10, screen->h-160, 0);
    fraktal(&t, 1, 80);
    teknoc_teleport(&t, 10, screen->h-90, 0);
    fraktal(&t, 2, 26.8);
    teknoc_teleport(&t, 10, screen->h-20, 0);
    fraktal(&t, 3, 9);
    
    /* hopehely */
    teknoc_szin(&t, 0, 0, 0);
    teknoc_teleport(&t, screen->w*2/3, screen->h/2, 0);
    for (i=0; i<6; ++i) {
        fraktal(&t, 2, 10);
        teknoc_fordul(&t, 60);
    }
    
    /* sokszogekbol */
    teknoc_szin(&t, 0, 128, 0);
    teknoc_teleport(&t, screen->w*3/4, screen->w*3/4, 0);
    for (i=0; i<7; ++i) {
        sokszog(&t, 4, 60);
        teknoc_fordul(&t, 360.0/7);
    }
    SDL_Flip(screen);
 
    while (SDL_WaitEvent(&ev) && ev.type!=SDL_QUIT)
        ; /* üres */
    SDL_Quit();
 
    return 0;
}


/* Teknoc implementacio */
void teknoc_init(Teknoc *t) {
    t->x=screen->w/2;
    t->y=screen->h/2;
    t->szog=0;
    t->rajzol=0;
    t->szin=0x000000FF;
}

void teknoc_szin(Teknoc *t, Uint8 r, Uint8 g, Uint8 b) {
    t->szin=r<<24 | g<<16 | b<<8 | 255;
}

void teknoc_rajzol(Teknoc *t) {
    t->rajzol=1;
}

void teknoc_nemrajzol(Teknoc *t) {
    t->rajzol=0;
}

void teknoc_teleport(Teknoc *t, double ujx, double ujy, double ujszog) {
    t->x=ujx;
    t->y=ujy;
    t->szog=ujszog;
}

void teknoc_elore(Teknoc *t, double r) {
    double const deg2rad=3.14159265358979323846/180.0;
    double ujx=t->x+r*cos(t->szog*deg2rad),
           ujy=t->y-r*sin(t->szog*deg2rad);
    if (t->rajzol)
        aalineColor(screen, t->x, t->y, ujx, ujy, t->szin);
    t->x=ujx;
    t->y=ujy;
}

void teknoc_fordul(Teknoc *t, double dszog) {
    t->szog+=dszog;
}
