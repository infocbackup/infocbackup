#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>

enum { MERET=32 }; 

SDL_Surface *screen;

typedef struct Pont {
    double x, y, z;     /* eredeti 3d koordináta */
    double xf, yf, zf;  /* forgatott 3d koordináta */
    int xk, yk;         /* képernyőn koordináta */
} Pont;

typedef struct Negyszog {
    Pont *sarkok[4];
    Uint32 szin;
} Negyszog;


int negyszogrendez(void const *vn1, void const *vn2) {
    Negyszog const *n1 = (Negyszog const *) vn1;
    Negyszog const *n2 = (Negyszog const *) vn2;
    if (n1->sarkok[0]->zf < n2->sarkok[0]->zf)
        return +1;
    if (n1->sarkok[0]->zf > n2->sarkok[0]->zf)
        return -1;
    return 0;
}


void negyszog_rajzol(Negyszog n) {
    Sint16 x[4] = { n.sarkok[0]->xk, n.sarkok[1]->xk, n.sarkok[2]->xk, n.sarkok[3]->xk },
           y[4] = { n.sarkok[0]->yk, n.sarkok[1]->yk, n.sarkok[2]->yk, n.sarkok[3]->yk };
    filledPolygonColor(screen, x, y, 4, n.szin);
    aapolygonColor(screen, x, y, 4, 0x00000080);
}


static Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


int main(int argc, char *argv[]) {
    SDL_Event ev;
    int x, y, i;
    Pont *pontok;
    Negyszog *negyszogek;
    Uint8 *keystate;
    SDL_TimerID id;
    double t = 0;
    double fx = -0.5, fy = -0.1;
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(800, 600, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    srand(time(NULL));
    pontok = (Pont *) malloc(sizeof(Pont)*MERET*MERET);
    negyszogek = (Negyszog *) malloc(sizeof(Pont)*(MERET-1)*(MERET-1));

    for (y=0; y<MERET; ++y)
        for (x=0; x<MERET; ++x) {
            i = y*MERET + x;
            pontok[i].x = 500*(x/(double)MERET-0.5);
            pontok[i].z = 500*(y/(double)MERET-0.5);
        }
    for (y=0; y<MERET-1; ++y)
        for (x=0; x<MERET-1; ++x) {
            i = y*(MERET-1) + x;
            negyszogek[i].sarkok[0] = pontok + y*MERET + x;
            negyszogek[i].sarkok[1] = pontok + y*MERET + x+1;
            negyszogek[i].sarkok[2] = pontok + (y+1)*MERET + x+1;
            negyszogek[i].sarkok[3] = pontok + (y+1)*MERET + x;
            negyszogek[i].szin = (rand()%65536 << 16) | rand()%65536;
        }

    keystate = SDL_GetKeyState(NULL);

    id = SDL_AddTimer(40, idozit, NULL);
    while (SDL_WaitEvent(&ev) && (ev.type!=SDL_QUIT)) {
        if (ev.type==SDL_USEREVENT) {
            boxRGBA(screen, 0, 0, screen->w, screen->h, 0, 0, 0, 255);

            t += 0.02;
            for (y=0; y<MERET; ++y)
                for (x=0; x<MERET; ++x) {
                    i = y*MERET + x;
                    pontok[i].y = 64*sin(t+x/5.0)*cos(t+y/5.0);
                }

            /* forgatott koordináták számolása */
            for (i=0; i<MERET*MERET; ++i) {
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

            /* gombnyomások? */
            if (keystate[SDLK_UP])
                fx+=0.02;
            if (keystate[SDLK_DOWN])
                fx-=0.02;
            if (keystate[SDLK_LEFT])
                fy-=0.02;
            if (keystate[SDLK_RIGHT])
                fy+=0.02;
        }
    }
    SDL_RemoveTimer(id);

    SDL_Quit();

    return 0;
}
