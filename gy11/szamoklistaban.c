/* Kiirja a beolvasott szamok kozul
 * az atlagnal kisebbeket. */
#include <stdio.h>
#include <stdlib.h>


/* lancolt elejere fuzheto egesz szamok */
typedef struct Szam {
   int szam;
   struct Szam *kov;
} Szam;


/* a megadott elejeren levo szamok atlagat adja */
double listaatlag(Szam *lista) {
   Szam *iter;
   double szum=0;
   int db=0;

   for (iter=lista; iter!=NULL; iter=iter->kov) {
      szum+=iter->szam;
      db++;
   }
   return szum/db;
}


/* uj elemet szur be a megadott lista elejere.
 * visszater az uj, megvaltozott lista eleje
 * mutatoval. a hasznalata:
 * l = beszur(l, 5);    
 */
Szam *elejere(Szam *lista, int szam) {
   Szam *uj=(Szam*) malloc(sizeof(Szam)); /* 1 */
   uj->szam=szam;
   uj->kov=lista;                         /* 2 */
   return uj;                             /* 3 */
}


/* felszabaditja a parameterkent kapott listat. */
void felszabadit(Szam *lista) {
   while (lista!=NULL) {
      Szam *kov=lista->kov;
      free(lista);
      lista=kov;
   }
}

int main(void) {
   Szam *lista=NULL, *iter;
   int beolv;
   double atlag;

   scanf("%d", &beolv);
   while (beolv!=0) {
      lista=elejere(lista, beolv);
      scanf("%d", &beolv);
   }

   atlag=listaatlag(lista);

   for (iter=lista; iter!=NULL; iter=iter->kov)
      if (iter->szam<atlag)
         printf("%d ", iter->szam);

   felszabadit(lista);

   return 0;
}
