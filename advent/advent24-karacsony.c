/* wuw such code */
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>


#define SZELES 1024
#define MAGAS 600

SDL_Surface *screen, *kep;


/* very random */
double randdouble(double min, double max) {
    return (double)rand()/RAND_MAX*(max-min)+min;
}


#define MAXPEHELY 1500
#define MINSEB 1.2
#define MAXSEB 2.5
#define MAXSZEL 5

/*       _\/ . \/_
        . _\/ \/_ .
       .  ' \_/ '  .
       ->->->_<-<-<-
       '  ._/ \_.  '
        . _/\ /\_ .
    jgs   /\ ' /\       */
typedef struct Pehely {
    double x;
    double y;
    double s;
} Pehely;


Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    Uint32 *p = (Uint32*) ((Uint8 *)surface->pixels + y * surface->pitch + x * sizeof(Uint32));
    return *p;
}

/* so bits */
int megakad(SDL_Surface *surface, int x, int y) {
    Uint32 pix = getpixel(surface, x, y);
    return ((pix >> surface->format->Rshift << surface->format->Rloss) & 0xFF) > 0x40
        || ((pix >> surface->format->Gshift << surface->format->Gloss) & 0xFF) > 0x40;
}

/*                  .------,
      .\/.          |______|
    _\_}{_/_       _|_Ll___|_
     / }{ \       [__________]          .\/.
      '/\'        /          \        _\_\/_/_
                 ()  o  o    ()        / /\ \
                  \ ~~~   .  /          '/\'
             _\/   \ '...'  /    \/_
              \\   {`------'}    //
               \\  /`---/',`\\  //
                \/'  o  | |\ \`//
                /'      | | \/ /\
   __,. -- ~~ ~|    o   `\|      |~ ~~ -- . __
               |                 |
          jgs  \    o            /
                `._           _.'
                   ^~- . -  ~^                   */
void ujpehely(Pehely *f) {
    f->x = randdouble(1, screen->w - 2);
    f->y = 0;
    f->s = randdouble(MINSEB, MAXSEB);
}

/* much winter */
void mozgat(Pehely pelyhek[], int *db) {
    static double szelirany = 0, szeliranycel = 0;
    static Uint32 const marad = 0xFFFFFF30, pehely = 0xFFFFFFC0;

    if (*db < MAXPEHELY) {
        ujpehely(&pelyhek[*db]);
        *db += 1;
    }
    int i;
    for (i = 0; i < *db; i++) {
        pelyhek[i].y += pelyhek[i].s;   /* lefele */
        if (pelyhek[i].y >= kep->h) {
            pixelColor(kep, pelyhek[i].x, kep->h-1, marad);
            ujpehely(&pelyhek[i]);
            continue;
        }
        if (megakad(kep, pelyhek[i].x, pelyhek[i].y) && rand() % 10 == 0) {            /* v */
            /* <- */
            if (pelyhek[i].x>0 && !megakad(kep, pelyhek[i].x-1, pelyhek[i].y-1) && !megakad(kep, pelyhek[i].x-1, pelyhek[i].y) && rand()%10 != 0)
                pelyhek[i].x--;
            else
            /* -> */
            if (pelyhek[i].x<kep->w-1 && !megakad(kep, pelyhek[i].x+1, pelyhek[i].y-1) && !megakad(kep, pelyhek[i].x+1, pelyhek[i].y) && rand()%10 != 0)
                pelyhek[i].x++;
            else {
                pixelColor(kep, pelyhek[i].x, pelyhek[i].y-1, marad);
                ujpehely(&pelyhek[i]);
            }
        }

        double m = pelyhek[i].x;    /* oldalra mozgas */
        pelyhek[i].x += szelirany + randdouble(-1.5, +1.5);
        if (pelyhek[i].x < 1)
            pelyhek[i].x += kep->w-2;
        if (pelyhek[i].x > kep->w-1)
            pelyhek[i].x -= kep->w-2;
        if (rand() % 2 == 0 && megakad(kep, pelyhek[i].x, pelyhek[i].y))
            pelyhek[i].x = m;
    }

    /*                 _..._
                      |     |
                      |     |
                      |_..._|
                   __.;.---.;.__
                   '-/       \-'
           ._  ,    |  o _ o  |    . _
             \|     |   (_>   | ,  |/
             _\\  . '\ '---' /   \//--.
            `  \\/  |`'-----;`\-.//_
             .--\\ .'-.____.|-(.//  ,
                 \)      _  \  \/ '-'\
                 |      (_)  |  ||.__/
                 |       _   \  \|
      """"     ""|      (_)   \_.;""      """
           "" jgs \             /     ""
      ""      ...  '._       _.'  ""     ...
         ..           '-----'
                  """        ..    """          */
    SDL_BlitSurface(kep, NULL, screen, NULL);
    for (i = 0; i < *db; i++) {
        pixelColor(screen, pelyhek[i].x, pelyhek[i].y, pehely);
    }
    SDL_Flip(screen);
    
    if (rand()%100 == 0)    /* szelirany valtas, 1:100 hogy valtunk */
        szeliranycel = randdouble(-MAXSZEL, MAXSZEL);
    /* valtunk vagy nem, a konkret szeliranyt lassan kozelitjuk a megcelzott ertekhez */
    if (szeliranycel > szelirany)
        szelirany += 0.01;
    if (szeliranycel < szelirany)
        szelirany -= 0.01;
}





/*             |
             '.'.'
            -= o =-
             .'.'.
               |
               ,
              / \
            .'. o'.
           / 6 s ^.\
          /.-.o *.-.\
          `/. '.'9  \`
         .'6. *  s o '.
        /.--.s .6 .--.\
        `/ s '. .' * .\`
       .' o 6 .` .^ 6 s'.
      /.---. * ^ o .----.\
      `/s * `.^ s.' ^ * \`
     .' o , 6 `.' ^ o  6 '.
    /,-^--,  o ^ * s ,----,\
    `'-._s.;-,_6_^,-;._o.-'
         jgs |   |
             `"""`          */
#define AGSZAM 3

typedef struct AgAdat {
    double doles;
    double helyzet;
    double rovidules;
} AgAdat;

void myline(SDL_Surface *bmp, double x1, double y1, double x2, double y2, Uint32 color, double r) {
    double dx = x2-x1, dy = y2-y1;
    int lepes = sqrt(dx*dx + dy*dy)/r*2;
    dx /= (lepes == 0) ? 1 : lepes;
    dy /= (lepes == 0) ? 1 : lepes;
    int i;
    for (i = 0; i <= lepes; ++i)
        filledCircleColor(bmp, x1 + dx*i, y1 + dy*i, r, color);
}

/* very rekursive */
/* wow */
/* so amaze */
void fat_rajzol(SDL_Surface *bmp, double x, double y, double hossz, double szog, double atlatszosag, AgAdat agak[], int n, double zold, double r, int disz_vagy_fa) {
    if (n < 0)
        return;
    int atl = atlatszosag * 255.0;
    double zold1 = zold + randdouble(-0.2, 0.2);
    if (disz_vagy_fa) {     /* fa */
        myline(bmp, x, y, x + hossz*cos(szog), y - hossz*sin(szog),
                             (atl*1/2)<<24 | (int)((0xFF-atl)*zold1)<<16 | 0<<8 | atl, r);
    }
    int i;
    for (i = 0; i < AGSZAM; ++i) {
        fat_rajzol(bmp, x + hossz*agak[i].helyzet*cos(szog), y - hossz*agak[i].helyzet*sin(szog),
                           hossz * (1-agak[i].rovidules),
                           szog + agak[i].doles + randdouble(-0.03, 0.03),
                           atlatszosag * 0.85,
                           agak, n-1,
                           zold, r * 0.9,
                           disz_vagy_fa);
    }
    
    int torol = rand()%7;
    int sr = torol&1 ? 64 : rand()%64+192;
    int sg = torol&2 ? 64 : rand()%64+192;
    int sb = torol&4 ? 64 : rand()%64+192;
    if (rand() % 1000 == 0 && !disz_vagy_fa) {   /* disz */
        filledCircleRGBA(bmp, x, y, 2, sr, sg, sb, 192);
        aacircleRGBA(bmp, x, y, 3, 0, 0, 0, 48);
    }
}

/*           *
            /.\
           /..'\
           /'.'\
          /.''.'\
          /.'.'.\
   "'""""/'.''.'.\""'"'"
     jgs ^^^[_]^^^      */
void randomfa(SDL_Surface *bmp, double x) {
    AgAdat agak[AGSZAM];
    double dolesmin = -2.0;
    double dolesmax = 2.0;
    int i;
    for (i = 0; i < AGSZAM; ++i) {  /* too randomy */
        agak[i].doles = dolesmin + (dolesmax-dolesmin)/(AGSZAM-1)*i;
        agak[i].helyzet = randdouble(0.8, 1.0);
        agak[i].rovidules = 0.5;
    }
    agak[AGSZAM/2].rovidules = 0.22; /* hogy magasak legyenek, a torzs rovidulese fix. igy a default magassag hatarozza meg */
    agak[AGSZAM/2].helyzet = 1.0;
    int newrand = rand();
    srand(newrand);
    fat_rajzol(bmp, x, bmp->h-32, randdouble(50, 80), 90*3.1415/180, 0.9, agak, 9, 0.6, 4, 1);
    srand(newrand);
    fat_rajzol(bmp, x, bmp->h-32, randdouble(50, 80), 90*3.1415/180, 0.9, agak, 9, 0.6, 4, 0);
    SDL_Flip(bmp);
}

/*
  +----------------+
  |                |
  |                |
  |                |
  |                |
  |                |
  XXXXXXXXXXXXXXXXXX
                    */
void clear(SDL_Surface *bmp) {
    boxColor(bmp, 0, 0, bmp->w-1, bmp->h-1, 0x303090FF);
    boxColor(bmp, 0, bmp->h-32, bmp->w-1, bmp->h-1, 0xF0F0F0FF);
    SDL_Flip(bmp);
}





/* 
 * FŐPROGRAM
 */
/* will initialaiz */
void init(void) {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(SZELES, MAGAS, 32, 0);
    SDL_WM_SetCaption("BOLDOG KARACSONYT!", "BOLDOG KARACSONYT!");
    kep = SDL_CreateRGBSurface(0, screen->w, screen->h, 32, 0, 0, 0, 0);
    clear(kep);
    int i;
    for (i = 0; i < 5; ++i)
        randomfa(kep, kep->w*i/4 + rand()%50 - 25);
}

/* wait litl */
Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


/*         _
          {_}
          / \
         /   \
        /_____\
      {`_______`}
       // . . \\
      (/(__7__)\)
      |'-' = `-'|
      |         |
      /\       /\
     /  '.   .'  \
    /_/   `"`   \_\
   {__}###[_]###{__}
   (_/\_________/\_)
       |___|___|
  jgs   |--|--|
       (__)`(__)        */

int main(int argc, char *argv[]) {
    SDL_TimerID id;
    int kilep;
    Pehely pelyhek[MAXPEHELY];
    int db;
    
    init();
    
    id = SDL_AddTimer(20, idozit, NULL);
    SDL_BlitSurface(kep, NULL, screen, NULL);
    kilep = 0;
    db = 0;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        
        switch (ev.type) {
            case SDL_USEREVENT:
                mozgat(pelyhek, &db);
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    kilep = 1;
                break;
                
            case SDL_QUIT:
                kilep = 1;
                break;
        }
    }
    SDL_RemoveTimer(id);

    SDL_FreeSurface(kep);
    SDL_Quit();
    return 0;
}
