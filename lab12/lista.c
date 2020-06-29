#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"

/* Lista létrehozása */
void lista_letrehoz(Lista *lista) {
    char *konyvtomb[][3]={
        {"Rick", "Riordan", "The Son of Neptunen"},
        {"Jeff", "Kinney", "Cabin Fever"},
        {"Rick", "Riordan", "The Throne of Fire"},
        {"Rick", "Riordan", "The Lost Hero"},
        {"Shel", "Silverstein", "Every Thing On It"},
        {"J. K.", "Rowling", "Harry Potter Paperback Boxed Set"},
        {"John", "Flanagan", "The Outcasts"},
        {"Dorling", "Kindersley", "The LEGO Ideas Book"},
        {"J. K.", "Rowling", "Harry Potter Hardcover Boxed Set"},
        {"James", "Patterson", "Middle School, the Worst Years of My Life"},
        {"Rick", "Silverstein", "The Outcasts"},
        {"Brian", "Selznick", "Wonderstruck"},
        {"Jeff", "Kinney", "The Ugly Truth"},
        {NULL}
    };
    ListaElem *akt;
    int i;

    /* Strázsák inicializálása */
    lista->eleje = (ListaElem*) malloc(sizeof(ListaElem));
    lista->eleje->elozo = NULL;
    lista->vege = (ListaElem*) malloc(sizeof(ListaElem));
    lista->vege->kovetkezo = NULL;

    /* Feltöltés */
    akt=lista->eleje;
    for (i=0; konyvtomb[i][0]!=NULL; i++) {
        ListaElem *uj = (ListaElem*)malloc(sizeof(ListaElem));
        strcpy(uj->szerzo_kereszt, konyvtomb[i][0]);
        strcpy(uj->szerzo_vezetek, konyvtomb[i][1]);
        strcpy(uj->cim, konyvtomb[i][2]);
        uj->db = 1;

        akt->kovetkezo=uj;
        uj->elozo = akt;
        akt=uj;
    }

    /* Záró strázsa pointerei */
    akt->kovetkezo = lista->vege;
    lista->vege->elozo = akt;
}

/* Lista felszabadítása */
void lista_free(Lista *const lista) {
    ListaElem *p=lista->eleje; /* Végigmegyek a listán */
    while (p!=NULL) {
        ListaElem *tmp = p->kovetkezo; /* Ideiglenes pointer */
        free(p); /* Törlés */
        p=tmp; /* Folytatás az ideiglenes pointertől */
    }
}

/* Lista kiírása előre */
void lista_kiir(Lista * const lista) {
    ListaElem *iter;
    /* Strázsákat kihagyva megyek végig */
    for (iter=lista->eleje->kovetkezo; iter!=lista->vege; iter=iter->kovetkezo)
        printf("%s %s: %s (%d db)\n", iter->szerzo_kereszt, iter->szerzo_vezetek, iter->cim, iter->db);
}

/* Lista kiírása hátrafele */
void lista_kiir_vissza(Lista * const lista) {
    ListaElem *iter;
    /* Strázsákat kihagyva megyek végig */
    for (iter=lista->vege->elozo; iter!=lista->eleje; iter=iter->elozo)
        printf("%s %s: %s (%d db)\n", iter->szerzo_kereszt, iter->szerzo_vezetek, iter->cim, iter->db);
}

/* Keresés a listában */
ListaElem * lista_keres(Lista *const lista,char *kereszt,char *vezetek,char *cim) {
    ListaElem *p;
    /* Végigmegyek a listán */
    for (p=lista->eleje->kovetkezo; p!=lista->vege;p=p->kovetkezo)
        /* Ha mindhárom paraméter egyezik, akkor visszaadom a címet */
        if ((strcmp(p->szerzo_vezetek,vezetek)==0) && (strcmp(p->szerzo_kereszt,kereszt)==0) && (strcmp(p->cim,cim)==0)) return p;

    /* Ha nincs találat */
    return NULL;
}

/* Hozzáfűzés */
void  lista_hozzafuz(Lista *const lista,char *kereszt,char *vezetek,char* cim) {
    /* Megkeresem, hogy benne van-e amit hozzá akarok adni */
    ListaElem * ker = lista_keres(lista,kereszt,vezetek,cim);
    /* Ha benne van, egyszerűen megnövelem a darabszámot */
    if (ker!=NULL) {
        ker->db++;
    }
    else{
        /* Különben foglalok új memóriaterületet */
        ListaElem * uj=(ListaElem*)malloc(sizeof(ListaElem));
        if (uj==NULL) return; /* Ha nem sikerül
        /* Inicializálom az adatokkal */
        strcpy(uj->cim,cim);
        strcpy(uj->szerzo_kereszt,kereszt);
        strcpy(uj->szerzo_vezetek,vezetek);
        uj->db=1;
        /* Befűzöm a lista végére (rajzon könnyebb elképzelni) */
        uj->kovetkezo=lista->vege;
        uj->elozo=lista->vege->elozo;
        lista->vege->elozo->kovetkezo=uj;
        lista->vege->elozo=uj;
    }
}

/* Törlés a listából */
void  lista_torol(Lista *const lista,char *kereszt,char *vezetek,char* cim) {
    ListaElem *p = lista_keres(lista,kereszt,vezetek,cim); /* Megkeressük a listában a törlendő elemet */
    if (p==NULL) return; /* Ha nem találjuk meg akkor nincs semmi dolgunk */
    /* Ha megvan */
    if (p->db>1) p->db--; /* Ha több van belőle egyszerűen a darabszámot csökkentjük */
    else{ /* Különben ténylegesen törlünk */
        /* Kifűzzük a listából */
        p->elozo->kovetkezo = p->kovetkezo;
        p->kovetkezo->elozo = p->elozo;
        free(p); /* Törlés */
    }
}

/* Beszúrás rendezve (segédfüggvény) */
void lista_rendezve_beszur(Lista *lista, ListaElem *uj) {
    ListaElem *mozgo;

    /* Egyszerű keresés, nincs lemaradó pointer */
    for (mozgo=lista->eleje->kovetkezo;
       mozgo!=lista->vege && strcmp(mozgo->cim, uj->cim)<0;
       mozgo=mozgo->kovetkezo);

    uj->elozo=mozgo->elozo;    /* Kisebb elemeket követi */
    mozgo->elozo->kovetkezo=uj;
    uj->kovetkezo=mozgo;       /* Nagyobb elemeket megelőzi */
    mozgo->elozo=uj;
}
