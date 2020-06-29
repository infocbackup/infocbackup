#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(void) {
   int szamok[5];
   int i, j;
   int mar_van;
   
   srand(time(0));
   
   /* 5 db szamot talalunk ki */
   for (i = 0; i < 5; i += 1) {
      do {
         szamok[i] = rand()%90+1;
 
         mar_van = 0;       /* nezzuk meg, van-e ilyen */
         for (j = 0; j < i; j += 1)
            if (szamok[j] == szamok[i])
               mar_van = 1;
      } while (mar_van);
   }
   
   for (i = 0; i < 5; i += 1)
      printf("%d ", szamok[i]);
 
   return 0;
}
