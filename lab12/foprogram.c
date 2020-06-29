#include <stdio.h>
#include "lista.h"



int main(void) {
    Lista konyvek;
    ListaElem *ker;
    lista_letrehoz(&konyvek); /* Létrehozzuk a listát */

    /* Kiírjuk */
    printf("A lista:\n");
    lista_kiir(&konyvek);
    printf("\n\n\n");

    /* Hozzáfűzök egy már létező elemet 3szor (darabszám fog nőni) */
    lista_hozzafuz(&konyvek,"John", "Flanagan", "The Outcasts");
    lista_hozzafuz(&konyvek,"John", "Flanagan", "The Outcasts");
    lista_hozzafuz(&konyvek,"John", "Flanagan", "The Outcasts");
    /* Hozzáfűzök egy teljesen új elemet */
    lista_hozzafuz(&konyvek,"Stephen", "Kochan", "Programfejlesztes C nyelven");

    /* Kitörlök egy elemet amiből több van */
    lista_torol(&konyvek,"John", "Flanagan", "The Outcasts");
    /* Kitörlök egyet amiből csak egy van (ez el fog tűnni) */
    lista_torol(&konyvek, "Brian", "Selznick", "Wonderstruck");

    /* Megkeresünk egy elemet */
    ker = lista_keres(&konyvek,"John", "Flanagan", "The Outcasts");
    if (ker==NULL) printf("Nem talalhato...\n");
    else printf("Talalat: %s %s - %s (%d db)\n",ker->szerzo_vezetek,ker->szerzo_kereszt,ker->cim,ker->db);

    /* Felszabadítás */
    lista_free(&konyvek);
    return 0;
}
