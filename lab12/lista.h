#ifndef LISTA_H
#define LISTA_H

typedef struct ListaElem {
    char szerzo_kereszt[50];
    char szerzo_vezetek[50];
    char cim[100];
    int db;
    struct ListaElem *kovetkezo, *elozo;
} ListaElem;

typedef struct Lista {
    ListaElem *eleje, *vege;
} Lista;

/* Lista létrehozása */
void lista_letrehoz(Lista *lista);
/* Lista felszabadítása */
void lista_free(Lista *const lista);
/* Lista kiírása előre */
void lista_kiir(Lista * const lista);
/* Lista kiírása hátrafele */
void lista_kiir_vissza(Lista * const lista);
/* Keresés a listában */
ListaElem * lista_keres(Lista *const lista,char *vezetek,char *kereszt,char *cim);
/* Hozzáfűzés */
void  lista_hozzafuz(Lista *const lista,char *kereszt,char *vezetek,char* cim);
/* Törlés a listából */
void  lista_torol(Lista *const lista,char *kereszt,char *vezetek,char* cim);
/* Beszúrás rendezve (segédfüggvény) */
void lista_rendezve_beszur(Lista *lista, ListaElem *uj);

#endif
