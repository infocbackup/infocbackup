#include <SDL.h> 
#include <SDL_gfxPrimitives.h> 
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>

SDL_Surface *screen;

int const rugohossz = 50;       /* a rugok hossza */
int const rugoelkap = 50;       /* az a tavolsag, amely alatt elengedeskor rugot illeszt be */

double const golyoelkap = 15;   /* pixel tavolsag, amennyi egy klikkelve megfogashoz kell */
double const delta_t = 0.001;   /* szamitas idolepese */

double const rugo_d = 0.25;     /* rugok rugalmassagi tenyezoje */
double const golyo_d = 10;      /* golyok rugalmassagi tenyezoje */
double const golyo_r = 7;       /* golyok sugara */
double const golyo_m = 0.05;    /* golyok tomege */
double const g = 9.81;          /* nehezsegi gyorsulas */

typedef struct Golyo {
    double x, y, vx, vy, fx, fy;  /* helyzet, sebesseg, ra hato ero */
    int fix;                      /* ha igaz, akkor nem mozdul */
} Golyo;

typedef struct Rugo {
    int g1, g2;                   /* ket tombindex - mely golyokat koti ossze */
    struct Rugo *prev, *next;     /* duplan lancolt listahoz */
} Rugo;

typedef struct RugoLista {       /* a rugok duplan lancolt listaja */
    Rugo *eleje, *vege;
} RugoLista;


/* adott g1, g2 tombindexu golyo koze rugot hoz letre */
void rugolista_hozzaad(RugoLista *rl, int g1, int g2) {
    Rugo *iter, *uj;

    /* mar benne van? */
    for (iter = rl->eleje->next; iter != rl->vege; iter = iter->next)
        if ((iter->g1 == g1 && iter->g2 == g2) || (iter->g1 == g2 && iter->g2 == g1))
            return;

    /* ha nincs, uj */
    uj = (Rugo*) malloc(sizeof(Rugo));
    uj->g1 = g1;
    uj->g2 = g2;
    uj->prev = rl->eleje;
    uj->next = rl->eleje->next;
    rl->eleje->next->prev = uj;
    rl->eleje->next = uj;
}


/* egy adott rugot torol a rugok listajabol */
void rugolista_torol(Rugo *rugo) {
    rugo->prev->next = rugo->next;
    rugo->next->prev = rugo->prev;
    free(rugo);
}


/* feltolti a golyok tombjet */
void golyokfeltolt(Golyo golyo[], int golyok) {
    int i;
    for (i = 0; i < golyok; i++) {
        golyo[i].x = i*screen->w/golyok+10;
        golyo[i].y = screen->h-20;
        golyo[i].vx = rand()%10-5;
        golyo[i].vy = rand()%10-5;
        golyo[i].fix = 0;
    }
    golyo[0].fix = 1;
    golyo[0].x = screen->w/2;
    golyo[0].y = 20;
}


/* kiszamolja az eroket, es minden golyo fx,dy-ebe beallitja */
/* a rugokkal ez nem foglalkozik */
void eroszamol(Golyo golyo[], int golyok, int megfogott) {
    int i, j;

    /* nullázás */
    for (i = 0; i < golyok; i++) {
        golyo[i].fx = 0;
        golyo[i].fy = 0;
    }

    /* közegellenállás - súrlódással helyettesítve */
    for (i = 0; i < golyok; i++) {
        double vnegyzet = golyo[i].vx*golyo[i].vx + golyo[i].vy*golyo[i].vy;
        double v = sqrt(vnegyzet);
        if (vnegyzet == 0) continue;     /* 0-val nem osztunk, meg amúgy sincs súrlódás */
        golyo[i].fx -= 0.2 * golyo[i].vx/v;
        golyo[i].fy -= 0.2 * golyo[i].vy/v;
    }

    /* gravitáció */
    for (i = 0; i < golyok; i++)
        golyo[i].fy += g*golyo_m;

    /* ütközés */
    for (i = 0; i < golyok; i++) {
        if (i == megfogott) continue;
        for (j = i+1; j < golyok; j++) {
            double dx, dy, tav;
            
            if (j == megfogott) continue;
            
            dx = golyo[i].x-golyo[j].x;
            dy = golyo[i].y-golyo[j].y;
            tav = sqrt(dx*dx+dy*dy);
            if (tav < golyo_r+golyo_r) {
                double l = golyo_r+golyo_r-tav, f = golyo_d*l;
                double fx = dx/tav*f, fy = dy/tav*f;
                golyo[i].fx += fx;
                golyo[i].fy += fy;
                golyo[j].fx -= fx;
                golyo[j].fy -= fy;
            }
        }
    }

    /* utkozes a palya szelevel */
    for (i = 0; i < golyok; i++) {
        if (golyo[i].x < golyo_r)
            golyo[i].fx += golyo_d * (golyo_r-golyo[i].x);
        if (golyo[i].x > screen->w-golyo_r)
            golyo[i].fx -= golyo_d * (golyo[i].x-(screen->w-golyo_r));
        if (golyo[i].y < golyo_r)
            golyo[i].fy += golyo_d * (golyo_r-golyo[i].y);
        if (golyo[i].y > screen->h-golyo_r)
            golyo[i].fy -= golyo_d * (golyo[i].y-(screen->h-golyo_r));
    }
}


/* mint a fenti, csak ez a rugokat kezeli a listabol */
void rugok_szamol(Golyo golyo[], int darab, RugoLista *rugo) {
    Rugo *iter = rugo->eleje->next;
    while (iter != rugo->vege) {
        Rugo *tmp = iter->next;
        double dx = golyo[iter->g1].x - golyo[iter->g2].x;
        double dy = golyo[iter->g1].y - golyo[iter->g2].y;
        double tav = sqrt(dx*dx + dy*dy);
        if (tav > rugohossz*2)
            rugolista_torol(iter); /* ha a duplajara nyulna, elszakad */
        else {
            double f = rugo_d*(tav-rugohossz), fx = dx/tav*f, fy = dy/tav*f;
            golyo[iter->g1].fx -= fx;
            golyo[iter->g1].fy -= fy;
            golyo[iter->g2].fx += fx;
            golyo[iter->g2].fy += fy;
        }
        iter = tmp;
    }
}


/* a kiszamolt erok alapjan mozgatja a golyokat. */
void mozgas(Golyo golyo[], int golyok, double delta_t, int megfogott) {
    int i;
    for (i = 0; i < golyok; i++) {
        if (golyo[i].fix || i == megfogott) continue;
        golyo[i].x += golyo[i].vx * delta_t;
        golyo[i].y += golyo[i].vy * delta_t;
        golyo[i].vx += golyo[i].fx/golyo_m * delta_t;
        golyo[i].vy += golyo[i].fy/golyo_m * delta_t;
    }
}


/* kirajzolja a rugokat es a golyokat. a megfogott jelzi
 * annak a golyonak a tombindexet, amelyikre klikkelve van - vagy -1 */
void kirajzol(Golyo golyo[], int golyok, RugoLista *rugo, int megfogott, int mouse_x, int mouse_y) {
    int i, gs;
    Rugo *iter;

    boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0x000000FF);
    /* rugó kirajz */
    for (iter = rugo->eleje->next; iter != rugo->vege; iter = iter->next)
        aalineColor(screen, golyo[iter->g1].x, golyo[iter->g1].y, golyo[iter->g2].x, golyo[iter->g2].y, 0x00A000FF);

    /* lehetseges rugo */
    if (megfogott >= 0)
        for (i = 0; i < golyok; ++i) {
            double dx = golyo[i].x-golyo[megfogott].x;
            double dy = golyo[i].y-golyo[megfogott].y;
            if (dx*dx+dy*dy  <= rugoelkap*rugoelkap)
                aalineColor(screen, golyo[i].x, golyo[i].y, golyo[megfogott].x, golyo[megfogott].y, 0x808080FF);
        }

    /* golyok kirajzolasa (lehetseges elkapast is mutatja, amire kattintva mozgatni lehetne azt) */
    gs = 0;
    for (i = 0; i < golyok; i++) {
        Uint32 szin1 = golyo[i].fix ? 0xA00000FF : 0x00A000FF, szin2 = golyo[i].fix ? 0xFF4040FF : 0x00FF00FF;
        double dx = golyo[i].x-mouse_x;
        double dy = golyo[i].y-mouse_y;
        if (megfogott == i || (megfogott == -1 && dx*dx+dy*dy <= golyoelkap*golyoelkap && !gs)) {
            gs = 1;
            szin1 = 0x0000A0FF;
            szin2 = 0x4040FFFF;
        }
        filledCircleColor(screen, golyo[i].x, golyo[i].y, golyo_r, szin1);
        aacircleColor(screen, golyo[i].x, golyo[i].y, golyo_r, szin2);
    }

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
    int const golyok = 30;
    Golyo *golyo;
    Rugo *iter;
    RugoLista rugo;
    int kilep;
    int megfogott, mouse_x, mouse_y;
    int i;
    SDL_TimerID id;

    golyo = (Golyo*) malloc(sizeof(Golyo)*golyok);
    rugo.eleje = (Rugo*) malloc(sizeof(Rugo));
    rugo.vege = (Rugo*) malloc(sizeof(Rugo));
    rugo.eleje->prev = NULL; rugo.eleje->next = rugo.vege;
    rugo.vege->prev = rugo.eleje; rugo.vege->next = NULL;

    /* SDL inicializálása és ablak megnyitása */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(640, 640, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("World of Goo", "World of Goo");

    golyokfeltolt(golyo, golyok);

    id = SDL_AddTimer(40, idozit, NULL);
    kilep = 0;
    megfogott = -1;     /* nincs megfogva semelyik */
    mouse_x = 0;
    mouse_y = 0;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        
        switch (ev.type) {
            case SDL_QUIT:
                kilep = 1;
                break;

            case SDL_USEREVENT:         /* idő múlása */
                for (i = 0; i < 100; ++i) {
                    eroszamol(golyo, golyok, megfogott);
                    rugok_szamol(golyo, golyok, &rugo);
                    mozgas(golyo, golyok, delta_t, megfogott);
                }
                kirajzol(golyo, golyok, &rugo, megfogott, mouse_x, mouse_y);
                break;
                
            case SDL_MOUSEMOTION:       /* egér mozgás */
                mouse_x = ev.motion.x;
                mouse_y = ev.motion.y;
                if (megfogott >= 0) {
                    golyo[megfogott].x = mouse_x;
                    golyo[megfogott].y = mouse_y;
                    golyo[megfogott].vx = 0;
                    golyo[megfogott].vy = 0;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:   /* egér kattintás */
                mouse_x = ev.button.x;
                mouse_y = ev.button.y;
                for (i = 0; i < golyok && megfogott == -1; ++i) {
                    double dx = golyo[i].x-mouse_x;
                    double dy = golyo[i].y-mouse_y;
                    if (dx*dx+dy*dy  <= golyoelkap*golyoelkap) {
                        megfogott = i;
                        if (!golyo[i].fix) {               /* ha nem fix, kiszakitjuk */
                            Rugo *iter = rugo.eleje->next;
                            while (iter != rugo.vege) {
                                Rugo *iternext = iter->next;
                                if (iter->g1 == i || iter->g2 == i)
                                    rugolista_torol(iter);
                                iter = iternext;
                            }
                        }
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:     /* egér elengedés */
                mouse_x = ev.button.x;
                mouse_y = ev.button.y;

                if (megfogott >= 0) {
                    for (i = 0; i < golyok; ++i) {
                        if (i == megfogott) continue;
                        double dx = golyo[i].x-golyo[megfogott].x;
                        double dy = golyo[i].y-golyo[megfogott].y;
                        if (dx*dx+dy*dy  <= rugoelkap*rugoelkap)
                            rugolista_hozzaad(&rugo, i, megfogott);
                    }
                    megfogott = -1;
                }
                break;
        }
    }
    SDL_RemoveTimer(id);

    /* vége: felszabadítás */
    free(golyo);
    for (iter = rugo.eleje; iter != NULL; ) {
        Rugo *kov = iter->next;
        free(iter);
        iter = kov;
    }
    
    SDL_Quit();
    
    return 0;
}
