/* LABIRINTUS GENERALO ELLER ALGORITMUSAVAL - SDL VERZIO */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

/* KONFIGURACIO */
/* allitsd 1-be, es akkor a termekbe bele lesz irva a halmaz szama */
#define SORSZAMOK 1
/* egy cella merete */
#define PIXEL 17
/* kesleltetes */
#define SLEEP 40
/* ezekkel eltolhato a kepen a labirintus, a youtube videohoz hasznaltam */
#define X_ELTOL 0
#define Y_ELTOL 0


/* kirajzolt cellak (az elso ketto a tenyleges labirintus, a masodik ketto az animaciohoz van) */
typedef enum Cella { Fal, Jarat, Egyesites, Egyesites_Helye } Cella;


/* RAJZOLAS ****************************************************************************/

/* globalis valtozo: kepernyo */
SDL_Surface *screen;


/* kirajzol egy cellat */
void rajzol(int x, int y, Cella c) {
    Uint32 szinek[] = {
        0xeeeeffff, 0x336699ff, 0xeb680eff, 0x00ff00ff  /* az enum cella sorrendjeben */
    };

    boxColor(screen, X_ELTOL+x*PIXEL, Y_ELTOL+y*PIXEL, X_ELTOL+x*PIXEL+PIXEL-1, Y_ELTOL+y*PIXEL+PIXEL-1, szinek[c]);
}


/* kiir egy intet a megadott cellaba */
void print_int(int x, int y, int mit) {
#if SORSZAMOK
    char str[12];
    sprintf(str, "%d", mit);
    stringColor(screen, X_ELTOL+1+x*PIXEL, Y_ELTOL+y*PIXEL+(PIXEL-8)/2, str, 0x222222ff);
#endif
}


/* HALMAZOK KEZELESE ****************************************************************************/
/* megszamolja, hogy egy adott halmaz hany elemu */
int halmaz_hany_elemu(int *hovatartozik, int szel, int melyik) {
    int x, db;
    
    db=0;
    for (x=0; x<szel; x++)
        if (hovatartozik[x]==melyik)
            db++;
    
    return db;
}


/* ad egy uj indexet, ami jo lesz egy ures halmazhoz */
int ures_halmaz_index() {
    static int ujindex=1;
    return ujindex++;
}


/* halmazok uniojat kepzi */
void halmazok_unioja(int *hovatartozik, int szel, int mit, int mivel) {
    int kisebb, x;
    
    kisebb=mit<mivel? mit:mivel;
    
    for (x=0; x<szel; x++)
        if (hovatartozik[x]==mivel || hovatartozik[x]==mit)
            hovatartozik[x]=kisebb;
}


/* kis animacio az egyesiteshez.
 * megkapja a halmazokat, a kepet, az egyesites koordinatait */
void halmazok_unioja_rajzol(int *hovatartozik, int szel, int x, int y) {
    int xx;
    int melyiket=hovatartozik[x];   /* ez az egyesitett halmaz */
    
    rajzol(x*2+2, y*2+1, Egyesites_Helye);
    for (xx=0; xx<szel; xx++)
        if (hovatartozik[xx]==melyiket) {
            rajzol(xx*2+1, y*2+1, Egyesites);
            print_int(xx*2+1, y*2+1, hovatartozik[xx]);
        }
    SDL_Flip(screen);
    SDL_Delay(5*SLEEP);
    rajzol(x*2+2, y*2+1, Jarat);
    for (xx=0; xx<szel; xx++)
        if (hovatartozik[xx]==melyiket) {
            rajzol(xx*2+1, y*2+1, Jarat);
            print_int(xx*2+1, y*2+1, hovatartozik[xx]);
        }
}


/* LABIRINTUS GENERALO ***************************************************************************/
void maze(int szel, int mag) {
    int hovatartozik[szel];      /* hogy melyik terem hova tartozik */
    int x, y;

    for (y=0; y<mag*2+1; y++)
        for (x=0; x<szel*2+1; x++)
            rajzol(x, y, Fal);
    
    /* eloszor minden terem kulon van. */
    for (x=0; x<szel; x++) {
        hovatartozik[x]=ures_halmaz_index();
        print_int(x*2+1, 0, hovatartozik[x]);
        rajzol(x*2+1, 1, Jarat);
        print_int(x*2+1, 1, hovatartozik[x]);
    }
    SDL_Flip(screen);
    SDL_Delay(10*SLEEP);
    
    /* vegigmegyunk a sorokon, az utolso sor kulonleges! */
    for (y=0; y<mag-1; y++) {
        /* termek rajzolasa, uniok elott. hogy az animacio jol nezzen ki :) */
        for (x=0; x<szel; x++) {
            rajzol(x*2+1, y*2+1, Jarat);
            print_int(x*2+1, y*2+1, hovatartozik[x]);
        }
        SDL_Flip(screen);
        SDL_Delay(5*SLEEP);

        /* vizszintes osszekottetesek generalasa */
        for (x=0; x<szel-1; x++)
            /* csak akkor szabad osszekotni oket, ha nincsenek egy halmazban */
            if (hovatartozik[x]!=hovatartozik[x+1])
                /* veletlenszeruen eldontjuk, osszekotjuk-e */
                if (rand()%2==0) {
                    /* osszekotodtek - kozos halmaz */
                    rajzol(x*2+2, y*2+1, Jarat);
                    halmazok_unioja(hovatartozik, szel, hovatartozik[x], hovatartozik[x+1]);
                    halmazok_unioja_rajzol(hovatartozik, szel, x, y);
                }
        
        /* a fuggoleges osszekottetesek elott kirajzoljuk
         * a kovetkezo sor termeit es indexeit, hogy latszodjon ahogy
         * valtozik */
        for (x=0; x<szel; x++) {
            /* ez a kovetkezo sor termei */
            rajzol(x*2+1, y*2+3, Jarat);
        }
        SDL_Flip(screen);
        SDL_Delay(5*SLEEP);

        /* fuggoleges osszekottetesek */
        for (x=0; x<szel; x++) {
            int bekotjuk;

            /* ha egy elemu, muszaj bekotni */
            if (halmaz_hany_elemu(hovatartozik, szel, hovatartozik[x])==1)
                bekotjuk=1;
            else {
                /* amugy eldonthetjuk, hogy bekotjuk-e. */
                if (rand()%2==0)
                    bekotjuk=1;
                else
                    bekotjuk=0;
            }

            if (bekotjuk) {
                /* ha bekotjuk, akkor semmi erdekes nem tortenik, csak animalunk */
                rajzol(x*2+1, y*2+2, Egyesites_Helye);
                print_int(x*2+1, y*2+2, hovatartozik[x]);
                SDL_Flip(screen);
                SDL_Delay(3*SLEEP);
                rajzol(x*2+1, y*2+2, Jarat);
                print_int(x*2+1, y*2+2, hovatartozik[x]);
            } else {
                /* a nem kotjuk be, akkor viszont uj halmaz keletkezik! */
                hovatartozik[x]=ures_halmaz_index(hovatartozik, szel);

                print_int(x*2+1, y*2+2, hovatartozik[x]);
                SDL_Flip(screen);
                SDL_Delay(3*SLEEP);
                print_int(x*2+1, y*2+2, hovatartozik[x]);
                /* akkor megvaltozott az alatta levo terem halmaza is */
                rajzol(x*2+1, y*2+3, Jarat);
            }
            
            /* kovetkezo terem szama mostanra tuti */
            print_int(x*2+1, y*2+3, hovatartozik[x]);
            SDL_Flip(screen);
            SDL_Delay(2*SLEEP);
        }
    }
    
    /* az utolso sor: minden kulonallot egyesiteni kell */
    y=mag-1;
    for (x=0; x<szel-1; x++)
        if (hovatartozik[x]!=hovatartozik[x+1]) {
            rajzol(x*2+2, y*2+1, Jarat);
            halmazok_unioja(hovatartozik, szel, hovatartozik[x], hovatartozik[x+1]);
            halmazok_unioja_rajzol(hovatartozik, szel, x, y);
        }
    for (x=0; x<szel; x++) {        /* termek rajzolasa, uniok utan */
        rajzol(x*2+1, y*2+1, Jarat);
        print_int(x*2+1, y*2+1, hovatartozik[x]);
    }
}


/* foprogram ******************************************/
int main(void) {
    int szel=15, mag=15;
    
    srand(time(0));
        
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode((szel*2+1)*PIXEL, (mag*2+1)*PIXEL, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Labirintus", "Labirintus");

    maze(szel, mag);
    SDL_Delay(20*SLEEP);

    return 0;
}
