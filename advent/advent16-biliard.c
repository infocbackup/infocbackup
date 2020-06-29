#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum { ABLAK_SZ = 640, ABLAK_M = 320 };
double const golyo_r=13;       /* golyó sugara */
double const golyo_m=0.05;     /* golyó tömege */
double const g=9.81;           /* nehézségi gyorsulás */
double const golyo_d=10;       /* falhoz, egymáshoz nyomódás állandója */
double const mu=0.15;          /* súrlódási tényező */


typedef struct Golyo {
    double x, y;             /* helyzet */
    double vx, vy;           /* sebesség */
    double fx, fy;           /* erő, számítás közben haszált */
    unsigned char r, g, b;   /* szín */
} Golyo;


/* feltölti a golyók tömbjét a kezdeti helyzetekkel */
void golyokfeltolt(Golyo golyo[], int golyok) {
    int i;
    int r=golyo_r+1;
    int dx=0, dy=0, sor=0;

    /* feher */
    golyo[0].x=ABLAK_SZ/4;
    golyo[0].y=ABLAK_M/2;
    golyo[0].vx=golyo[0].vy=0;
    golyo[0].r=255;
    golyo[0].g=255;
    golyo[0].b=255;

    /* tobbi */
    for (i=1; i<golyok; i++) {
        golyo[i].x=ABLAK_SZ*2/3 + dx*1.75*r;
        golyo[i].y=ABLAK_M/2 + (dy*2-dx)*0.98*r;
        golyo[i].vx=golyo[i].vy=0;
        golyo[i].r=64+rand()%192;
        golyo[i].g=64+rand()%192;
        golyo[i].b=64+rand()%192;

        /* ettől lesznek háromszög alakban */
        dy++;
        if (dy>sor) {
            dx++;
            dy=0;
            sor++;
        }
    }
}


/* kiszámolja a golyókra ható erőket */
void eroszamol(Golyo golyo[], int golyok) {
    int i, j;

    /* erők nullázása */
    for (i=0; i<golyok; i++) {
        golyo[i].fx=0;
        golyo[i].fy=0;
    }

    /* súrlódás */
    for (i=0; i<golyok; i++) {
        double v=sqrt(golyo[i].vx*golyo[i].vx+golyo[i].vy*golyo[i].vy);
        /* álló golyónál nem számolunk */
        if (v==0) continue;
        golyo[i].fx-=mu*g*golyo_m*golyo[i].vx/v;
        golyo[i].fy-=mu*g*golyo_m*golyo[i].vy/v;
    }

    /* ütközések egymással - O(n^2) algoritmus, de sebaj */
    for (i=0; i<golyok; i++) {
        for (j=i+1; j<golyok; j++) {
            double dx=golyo[i].x-golyo[j].x;
            double dy=golyo[i].y-golyo[j].y;
            double tav=sqrt(dx*dx+dy*dy);
            if (tav<golyo_r+golyo_r) {
                double l=golyo_r+golyo_r-tav;
                double f=golyo_d*l;
                dx=dx/tav*f; /* egységvektor*f */
                dy=dy/tav*f;
                golyo[i].fx+=dx;
                golyo[i].fy+=dy;
                golyo[j].fx-=dx;
                golyo[j].fy-=dy;
            }
        }
    }

    /* ütközés fallal */
    for (i=0; i<golyok; i++) {
        if (golyo[i].x<golyo_r)
            golyo[i].fx+=golyo_d * (golyo_r-golyo[i].x);
        if (golyo[i].x>ABLAK_SZ-golyo_r)
            golyo[i].fx-=golyo_d * (golyo[i].x-(ABLAK_SZ-golyo_r));
        if (golyo[i].y<golyo_r)
            golyo[i].fy+=golyo_d * (golyo_r-golyo[i].y);
        if (golyo[i].y>ABLAK_M-golyo_r)
            golyo[i].fy-=golyo_d * (golyo[i].y-(ABLAK_M-golyo_r));
    }
}


/* golyók mozgatása a kiszámolt sebességek alapján */
void mozgas(Golyo golyo[], int golyok, int megfogva, double delta_t) {
    int i;
    for (i=0; i<golyok; i++) {
        if (megfogva && i==0) continue;   /* ha epp fogja, akkor kihagyjuk */
        golyo[i].x+=golyo[i].vx * delta_t;
        golyo[i].y+=golyo[i].vy * delta_t;
        golyo[i].vx+=golyo[i].fx/golyo_m * delta_t;
        golyo[i].vy+=golyo[i].fy/golyo_m * delta_t;
    }
}


/* kirajzolás - zöld terepre a golyók. ha épp meg van fogva a fehér golyó,
 * akkor a keletkező sebességvektort is mutatja egy vonallal */
void kirajzol(SDL_Surface *screen, Golyo golyo[], int golyok, int megfogva, int mouse_x, int mouse_y) {
    int i;

    boxRGBA(screen, 0, 0, screen->w-1, screen->h-1, 0, 96, 0, 255);       /* zöld asztal */
    for (i=0; i<golyok; i++) {
        filledCircleRGBA(screen, golyo[i].x, golyo[i].y, golyo_r, golyo[i].r, golyo[i].g, golyo[i].b, 255);
        aacircleRGBA(screen, golyo[i].x, golyo[i].y, golyo_r, 0, 0, 0, 255);
    }
    if (megfogva)
        aalineRGBA(screen, mouse_x, mouse_y, golyo[0].x, golyo[0].y, 0, 0, 0, 255);
    SDL_Flip(screen);
}


/* a főprogram által használt időzítő */
Uint32 idozit(Uint32 ms, void* param) {
     SDL_Event ev;
     ev.type = SDL_USEREVENT;
     SDL_PushEvent(&ev);
     return ms;
}


int main(int argc, char *argv[]) {
    enum { GOLYOK = 16 };
    Golyo golyo[GOLYOK];
    int megfogva, mouse_x, mouse_y;     /* egér kezeléséhez */
    SDL_TimerID id;
    SDL_Surface *screen;
    int quit, i;

    /* SDL inicializálás */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(ABLAK_SZ, ABLAK_M, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    golyokfeltolt(golyo, GOLYOK);

    /* az eseménykezelő hurok */
    id = SDL_AddTimer(40, idozit, NULL);
    quit = 0;
    megfogva = 0;
    mouse_x = 0;
    mouse_y = 0;
    while (!quit) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        
        switch (ev.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_USEREVENT:         /* idő múlása */
                /* 100 időlépésnyit */
                for (i=0; i<100; ++i) {
                    eroszamol(golyo, GOLYOK);
                    mozgas(golyo, GOLYOK, megfogva, 0.001);     /* időlépés */
                }
                kirajzol(screen, golyo, GOLYOK, megfogva, mouse_x, mouse_y);
                break;
            case SDL_MOUSEMOTION:       /* egér mozgás */
                mouse_x = ev.motion.x;
                mouse_y = ev.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:   /* egér kattintás */
                mouse_x = ev.button.x;
                mouse_y = ev.button.y;
                {
                    double dx=golyo[0].x-mouse_x;
                    double dy=golyo[0].y-mouse_y;
                    if (dx*dx+dy*dy <= golyo_r*golyo_r)
                        megfogva=1;
                }
              break;
            case SDL_MOUSEBUTTONUP:     /* egér elengedés */
                mouse_x = ev.button.x;
                mouse_y = ev.button.y;

                /* az új sebesség a húzás irányából és nagyságából */
                if (megfogva) {
                    golyo[0].vx=golyo[0].x-mouse_x;
                    golyo[0].vy=golyo[0].y-mouse_y;
                    megfogva=0;
                }
                break;
        }
    }
    SDL_RemoveTimer(id);
    SDL_Quit();

    return 0;
}
