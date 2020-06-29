/* Negy alapmuveletbol allo matematikai kifejezeseket kiertekelo
 * program. A megvalositas egy rekurziv alaszallo ertelmezo.
 * Reszletesebb magyarazat a 14. eloadas anyaganak elso feleben!
 */

#include <stdio.h>


int karakter(char **szoveg, char const *ertekek, char *karakter);
int kifejezes(char **szoveg, int *ertek);
int osszeg(char **szoveg, int *ertek);
int pluszminusz_szorzat(char **szoveg, char *muvelet, int *ertek);
int szam(char **szoveg, int *ertek);
int szamjegy(char **szoveg, char *szamjegy);
int szorzat(char **szoveg, int *ertek);
int szorper_tenyezo(char **szoveg, char *muvelet, int *ertek);
int tenyezo(char **szoveg, int *ertek);
int whitespace(char **szoveg);
int zarojeles(char **szoveg, int *ertek);


int main(void) {
    char txt[500], *szoveg, *res;
    int szam;

    while (NULL != (res = fgets(txt, 500, stdin)) && txt[1] != 0) {
        szoveg = txt;

        if (kifejezes(&szoveg, &szam)) {
            printf("Értéke: %d\n", szam);
        }
        else printf("Nem sikerült értelmezni a szöveget.\n");
    }

    return 0;
}


/* A legfelsobb szabaly. Megnezi, a sztringben kifejezes
 * van-e; ha igen, akkor az erteket beteszi a valtozoba. */
int kifejezes(char **szoveg, int *ertek) {
    return osszeg(szoveg, ertek);
}


/* Egy darab szamjegyet probal illeszteni. Ha sikerul, beteszi
 * a szamjegy nevu parameterbe. */
int szamjegy(char **szoveg, char *szamjegy) {
    char *ptxt = *szoveg;

    if (*ptxt >= '0' &&  *ptxt <= '9') {
        *szamjegy = *ptxt;
        *szoveg += 1;
        return 1;
    }

    return 0;
}


/* Egy egesz tizes szamrendszerbeli szamot illeszt. Ha sikerul,
 * az erteket integerre alakitva az ertek nevu parameterbe teszi. */
int szam(char **szoveg, int *ertek) {
    char *ptxt = *szoveg, c;
    int val = 0;

    if (*ptxt == '0') {
        *ertek = 0;
        *szoveg += 1;
        return 1;
    }

    if (szamjegy(&ptxt, &c)) {
        val = c - '0';

        while (szamjegy(&ptxt, &c)) {
            val = val * 10 + (c - '0');
        }

        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }

    return 0;
}


/* Egy karaktert probal meg illeszteni. A lehetseges karakterek
 * az ertekek nevu sztringben vannak. Ha sikerul az illesztes,
 * a megtalalt karakter a karakter nevu valtozoba kerul. */
int karakter(char **szoveg, char const *ertekek, char *karakter) {
    for (; *ertekek != 0; ++ertekek) {
        if (**szoveg == *ertekek) {
            *karakter = **szoveg;
            *szoveg += 1;
            return 1;
        }
    }

    return 0;
}


/* Osszeget probal illeszteni. Ha sikerul, az ertek a parameterkent
 * kapott valtozoba kerul. */
int osszeg(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;
    int val;

    whitespace(&ptxt);

    if (szorzat(&ptxt, &val)) {
        int tmp;
        char c;

        while (pluszminusz_szorzat(&ptxt, &c, &tmp)) {
            if (c == '+') val += tmp;
            else val -= tmp;
        }

        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }

    return 0;
}


/* egy plusz vagy minusz jelet, utana pedig egy szorzatot illeszt.
 * ha sikeres, akkor a muveletbe teszi a muveleti jelet, az ertekbe
 * pedig a szorzas eredmenyet; egyeb esetben a ket parameter hivas
 * utani erteke definialatlan. */
int pluszminusz_szorzat(char **szoveg, char *muvelet, int *ertek) {
    char *ptxt = *szoveg;

    if (whitespace(&ptxt) && karakter(&ptxt, "+-", muvelet) && whitespace(&ptxt) && szorzat(&ptxt, ertek)) {
        /* eddigre a karakter() a muveletbe, a szorzat() az ertekbe tette az illesztesek eredmenyeit */
        *szoveg = ptxt;
        return 1;
    }
    return 0;
}


/* Szorzatot probal illeszteni. Ha sikerul, az erteket beteszi a
 * parameterkent kapott valtozoba. */
int szorzat(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;
    int val;

    whitespace(&ptxt);

    if (tenyezo(&ptxt, &val)) {
        int tmp;
        char c;

        while (szorper_tenyezo(&ptxt, &c, &tmp)) {
            if (c == '*') val *= tmp;
            else val /= tmp;
        }

        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }

    return 0;
}


/* egy szorzas vagy osztas jelet, utana pedig egy tenyezot illeszt.
 * ha sikeres, akkor a muveletbe teszi a muveleti jelet, az ertekbe
 * pedig a szorzas eredmenyet; egyeb esetben a ket parameter hivas
 * utani erteke definialatlan. */
int szorper_tenyezo(char **szoveg, char *muvelet, int *ertek) {
    char *ptxt = *szoveg;

    if (whitespace(&ptxt) && karakter(&ptxt, "*/", muvelet) && whitespace(&ptxt) && tenyezo(&ptxt, ertek)) {
        /* eddigre a karakter() a muveletbe, a tenyezo() az ertekbe tette az illesztesek eredmenyeit */
        *szoveg = ptxt;
        return 1;
    }
    return 0;
}


/* Tenyezot probal illeszteni. A tenyezo lehet egy szam,
 * vagy egy zarojeles kifejezes. Ha sikerult, erteket a
 * parameterkent kapott valtozoba teszi. */
int tenyezo(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;
    int val;

    whitespace(&ptxt);

    if (szam(&ptxt, &val)) {
        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }

    if (zarojeles(&ptxt, &val)) {
        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }


    return 0;
}


/* Zarojeles kifejezest probal illeszteni. */
int zarojeles(char **szoveg, int *ertek) {
    char *ptxt = *szoveg, c;
    int val;

    whitespace(&ptxt);

    if (karakter(&ptxt, "(", &c) && kifejezes(&ptxt, &val) && whitespace(&ptxt) && karakter(&ptxt, ")", &c)) {
        *ertek = val;
        *szoveg = ptxt;
        return 1;
    }

    return 0;
}


/* Szokozt vagy szokoz jellegu karaktert illeszt, es lepteti a pointert.
 * Mindig sikeres - ha nem talal szokozt, akkor is. Csak azert van, hogy
 * a kifejezesben tetszolegesen sok szokoz szerepelhessen. */
int whitespace(char **szoveg) {
    char c;

    while (karakter(szoveg, " \t\n", &c));

    return 1;
}

