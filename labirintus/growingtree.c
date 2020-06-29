/* LABIRINTUS GENERALO A GROWING TREE ALGORITMUSAVAL */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

/* KONFIGURACIO */
#define MERETX 35
#define MERETY 35
/* egy cella merete */
#define PIXEL 16
/* kesleltetes */
#define SLEEP 40

typedef enum Cella { Fal, Jarat, Ideiglenes } Cella;

typedef Cella Kep[MERETY][MERETX];

/* globalis valtozo: kepernyo */
SDL_Surface *screen;

/* PONTOKAT TAROLO TIPUS ES DOLGAI ******************************************/
typedef struct Pont {
    int x, y;   /* koordinatak */
} Pont;


enum { PONTOKTOMBMERET = MERETX*MERETY/4 };   /* ennyi eleg kell legyen */
typedef struct Pontok {
    int db;
    Pont pontok[PONTOKTOMBMERET];
} Pontok;


void pontok_init(Pontok *p) {
    p->db = 0;
}


/* kitorli az adott indexu elemet a tombbol */
void pontok_elemet_torol(Pontok *p, int hanyadikat) {
    int i;
    
    /* nincs is annyiadik? */
    assert(hanyadikat < p->db);

    for (i=hanyadikat+1; i<p->db; ++i)
        p->pontok[i-1] = p->pontok[i];
    p->db--;
}


/* hozzafuz egy uj elemet a tombhoz */
void pontok_hozzafuz(Pontok *p, int x, int y) {
    assert(p->db < PONTOKTOMBMERET);
    p->pontok[p->db].x = x;
    p->pontok[p->db].y = y;
    p->db++;
}


/* igazzal ter vissza, ha ures */
int pontok_ures(Pontok *p) {
    return p->db == 0;
}
/* </Pontok> ***********************************************/


/* RAJZOLAS ******************************************/
void rajzol(Kep k, int x, int y, Cella mit) {
    /* globális tömb a színeknek */
    Uint32 szinek[] = { 0xffffffff, 0x000000ff, 0x00ff00ff }; /* fal, jarat, ideiglenes */
    k[y][x]=mit;
    boxColor(screen, x*PIXEL, y*PIXEL, x*PIXEL+PIXEL-1, y*PIXEL+PIXEL-1, szinek[mit]);
}


/* ures palyat (kepet) csinal */
void ures(Kep k) {
    int x, y;

    /* keret, mintha ott jarat lenne, es akkor nem megy oda a rajzolo */
    for (y=0; y<MERETY; y++) {
        rajzol(k, 0, y, Jarat);
        rajzol(k, MERETX-1, y, Jarat);
    }
    for (x=0; x<MERETX; x++) {
        rajzol(k, x, 0, Jarat);
        rajzol(k, x, MERETY-1, Jarat);
    }
    
    /* tobbi telibe fallal */
    for (y=1; y<MERETY-1; y++)
        for (x=1; x<MERETX-1; x++)
            rajzol(k, x, y, Fal);
}


/* A LABIRINTUS GENERALO *****************************************/
void labirintus_rajzol(Kep k, int kx, int ky) {
    typedef enum { balra, jobbra, fel, le } Irany;
    Irany iranyok[4]={ balra, jobbra, fel, le };
    /* iranykoordinatak; indexek megegyeznek az iranyok tombbel! */
    int const dx[4]={ -1, 1, 0, 0 };
    int const dy[4]={ 0, 0, 1, -1 };
    Pontok l;

    pontok_init(&l);
    
    /* a kiindulasi pontot kivajjuk */
    k[ky][kx]=Jarat;
    pontok_hozzafuz(&l, kx, ky);
    
    while (!pontok_ures(&l)) {
        Pont p;
        Irany r;
        int cx, cy;                     /* erre probalunk menni */
        int lehet_meg_valahova;
        int i, veletlen_index;
        
        /* kiveszunk egy elemet a listabol. ezt fogjuk vizsgalni. */
        /* itt lehet allitani, hogy elso (0), utolso (l.db-1)
         * vagy masmelyik elemet vegye ki a tombbol. */
        veletlen_index = rand() % l.db;
        p=l.pontok[veletlen_index];
        
        /* valasztunk egy random iranyt is */
        r=iranyok[rand()%4];
        cx=p.x+dx[r]*2;
        cy=p.y+dy[r]*2;
        
        /* megprobalunk menni arra. */
        if (k[cy][cx]==Fal) {
            rajzol(k, cx, cy, Ideiglenes);      /* uj terem */
            rajzol(k, p.x+dx[r], p.y+dy[r], Jarat);  /* odavezeto ut*/
            pontok_hozzafuz(&l, cx, cy);
            SDL_Flip(screen);
            SDL_Delay(SLEEP);
        }
        /* itt szebb lenne, ha osszegyujtenenk a lehetseges iranyokat,
         * es utana abbol valasztanank veletlenszeruen. mert lehet, hogy
         * lehet menni valamerre, de a random pont nem azt az iranyt
         * talalja ki... osszesen csak 4 irany van, ugyhogy ez a pongyolasag
         * most belefer. */

        /* es akkor most, lehet meg innen vhova menni? */
        lehet_meg_valahova=0;
        for (i=0; i<4; i++) {
            cx=p.x+dx[i]*2;
            cy=p.y+dy[i]*2;
            if (k[cy][cx]==Fal)
                lehet_meg_valahova=1;
        }
        /* ha mar nem, akkor kiszedjuk a tarolobol */
        if (!lehet_meg_valahova) {
            rajzol(k, p.x, p.y, Jarat);
            pontok_elemet_torol(&l, veletlen_index);
            SDL_Flip(screen);
            SDL_Delay(SLEEP);
        }
    }
}


/* foprogram ******************************************/
int main(void) {
    Kep k;

    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(MERETX*PIXEL, MERETY*PIXEL, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Labirintus", "Labirintus");
    
    ures(k);
    
    labirintus_rajzol(k, MERETX/4*2, MERETY/4*2);

    SDL_Delay(5000);
    
    return 0;
}
