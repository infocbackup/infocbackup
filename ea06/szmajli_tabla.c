/*
 * Az allapotgepeket nem csak egymasba agyazott switch()
 * szerkezetekkel lehet lekodolni. Sot nagyobb tablazatokat
 * kifejezetten nem erdemes ugy.
 * 
 * Ehelyett kihasznalhatjuk azt, hogy az allapotatmeneti tabla
 * es a tevekenysegtabla mind ketdimenziosak - az egyes
 * allapotatmeneteket es tevekenysegeket igy eltarolhatjuk egy
 * ketdimenzios tombben.
 * 
 * A tomb minden cellaja egy uj allapotot es egy tevekenyseget
 * tarol (igy a tomb strukturakbol kell felepuljon). A
 * tevekenyseget a szmajlis programnal eltarolhatjuk olyan
 * formaban, hogy beleirjuk a tombbe, milyen szoveget kell epp
 * kiirni, es hogy az aktualisan kiolvasott karakternek is
 * meg kell-e jelennie a kimeneten. Ez egy sztring es egy
 * logikai ertek. Az ly szamlalo eseten a tevekenyseg egy
 * egyszeru egesz szam lehetne: 0-val, 1-gyel vagy 2-vel kell
 * novelni a szamlalot. (Bonyolultabb esetekben un.
 * fuggvenypointereket hasznalhatnank, de errol egy kesobbi
 * eloadasban lesz szo.)
 * 
 * A tablazatbol torteno kiolvasasnak a feltetele az, hogy mind
 * az allapotokhoz, mind az esemenyekhez (karakterekhez) egy
 * egesz szamot rendeljunk, amely a tablazatnak a sor- es
 * oszlopszama lesz. Az allapotoknal a hozzarendeles
 * automatikusan megtortenik (hiszen a felsorolt tipusu ertekek
 * egesz szamma alakulnak), a karaktereknel nekunk kell ezt
 * elvegezni.
 * 
 * A program futtatasaval kapcsolatos tudnivalokat lasd a
 * switch()-es verzio kommentjeben.
 */

#include <stdio.h>

/* az allapotok, es lekepezesuk 0..2 egesz szamra. */
typedef enum SzmajliAllapot {
    Alap = 0,
    Kettospont = 1,
    Kisebb = 2
} SzmajliAllapot;

/* a beolvasott karakter lekepezese 0..4 egesz szamra. */
int karakterosztaly(char c) {
   switch (c) {
      default:  return 0;
      case ':': return 1;
      case ')': return 2;
      case '<': return 3;
      case '3': return 4;
   }
}

/* allapotatmenet es tevekenyseg a tablazat egy cellajaban. */
typedef struct TablaCella {
   SzmajliAllapot uj_allapot;
   char szoveg[5];
   int akt_char_kiir;
} TablaCella;

/* a 3x5-os allapotatmeneti es tevekenysegtabla. egy az
 * egyben ugyanaz, mint papiron! */
TablaCella allapotgep[3][5] = {
   { {Alap, "", 1},  {Kettospont, "", 0},  {Alap, "", 1},             {Kisebb, "", 0},  {Alap, "", 1} },
   { {Alap, ":", 1}, {Kettospont, ":", 0}, {Alap, "\342\230\273", 0}, {Kisebb, ":", 0}, {Alap, ":", 0} },
   { {Alap, "<", 1}, {Kettospont, "<", 0}, {Alap, "<", 1},            {Kisebb, "<", 0}, {Alap, "\342\231\245", 0} },
};


int main(void) {
   SzmajliAllapot all;
   int c;
 
   /* az allapotgep kodja pedig ennyi. mindent a tablazatbol olvas! */
   all = Alap;
   while ((c = getchar()) != EOF) {
      int kar = karakterosztaly(c);

      /* tevekenyseg */
      printf("%s", allapotgep[all][kar].szoveg);
      if (allapotgep[all][kar].akt_char_kiir)
         putchar(c);
      /* allapotatmenet */
      all = allapotgep[all][kar].uj_allapot;
   }
   
   return 0;
}
