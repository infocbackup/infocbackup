#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <math.h>


static void kocka(void) {
    glBegin(GL_QUADS);
    glNormal3d(0, 0, -1);    /* hatlap */
    glVertex3d(0, 1, 0); glVertex3d(1, 1, 0); glVertex3d(1, 0, 0); glVertex3d(0, 0, 0);
    glNormal3d(0, 0, +1);   /* elolap */
    glVertex3d(0, 0, 1); glVertex3d(1, 0, 1); glVertex3d(1, 1, 1); glVertex3d(0, 1, 1);
    glNormal3d(-1, 0, 0);   /* bal */
    glVertex3d(0, 0, 0); glVertex3d(0, 0, 1); glVertex3d(0, 1, 1); glVertex3d(0, 1, 0);
    glNormal3d(1, 0, 0);   /* jobb */
    glVertex3d(1, 1, 0); glVertex3d(1, 1, 1); glVertex3d(1, 0, 1); glVertex3d(1, 0, 0);
    glNormal3d(0, 1, 0);   /* felso */
    glVertex3d(0, 1, 1); glVertex3d(1, 1, 1); glVertex3d(1, 1, 0); glVertex3d(0, 1, 0);
    glNormal3d(0, -1, 0);   /* also */
    glVertex3d(0, 0, 0); glVertex3d(1, 0, 0); glVertex3d(1, 0, 1); glVertex3d(0, 0, 1);
    glEnd();

    glColor3d(0, 0, 0);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0); glVertex3d(2, 0, 0);
    glVertex3d(2, 0, 0); glVertex3d(1.8, 0.1, 0);
    glVertex3d(2, 0, 0); glVertex3d(1.8, -0.1, 0);
    
    glVertex3d(0, 0, 0); glVertex3d(0, 2, 0);
    glVertex3d(0, 2, 0); glVertex3d(0.1, 1.8, 0);
    glVertex3d(0, 2, 0); glVertex3d(-0.1, 1.8, 0);
    
    glVertex3d(0, 0, 0); glVertex3d(0, 0, 2);
    glVertex3d(0, 0, 2); glVertex3d(0, 0.1, 1.8);
    glVertex3d(0, 0, 2); glVertex3d(0, -0.1, 1.8);
    glEnd();
}


Uint32 timer(Uint32 ms, void *param) {
    SDL_Event ev = {SDL_USEREVENT};
    SDL_PushEvent(&ev);
    return ms;
}


void rajzol(double forgat) {
    /* rajzolas kezdete */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-10, 6, 20, 0, 0, 0, 0, 1, 0);        /* innen, erre, ezvanfelfele */
    glRotatef(forgat, 0, 1, 0);                     /* meg pluszba elforgatjuk */

    /* fold - az elmaradhatatlan sakktabla */
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
    double x, z;
    for (x = -80; x<80; x+=10)
        for (z = -80; z<80; z+=10) {
            glColor3d(0, ((int)(x/10)^(int)(z/10)) & 1 ? 0.5 : 0.45, 0);
            glVertex3d(x+10, -10, z);
            glVertex3d(x, -10, z);
            glVertex3d(x, -10, z+10);
            glVertex3d(x+10, -10, z+10);
        }
    glEnd();

    /* piros */
    glPushMatrix();
    glColor3d(1, 0, 0); kocka();
    glPopMatrix();

    /* kek */
    glPushMatrix();
    glTranslatef(2, 3, 0);
    glRotatef(15, 0.3, 1, 0.9);
    glColor3d(0, 0, 1); kocka();
    glPopMatrix();

    /* zold */
    glPushMatrix();
    glTranslatef(-2, +4, -1);
    glRotatef(85, 0.7, -1, 0.3);
    glScalef(1.7, 0.5, 0.7);
    glColor3d(0, 1, 0); kocka();
    glPopMatrix();
    
    /* sarga */
    glPushMatrix();
    glTranslatef(-1.3, 0, 3);
    glRotatef(220, 3, -1, 6);
    glScalef(1.2, 1.2, 1.2);
    glColor3d(1, 1, 0); kocka();
    glPopMatrix();
    
    /* rajzolas vege, mehet a kepernyore */
    SDL_GL_SwapBuffers();
}


void grafika_init() {
    int w = 800, h = 600;
    GLfloat lightcol[4] = { 0.5, 0.5, 0.5 };        /* szin */
    GLfloat lightdir[4] = { 0, 1, 0, 1 };           /* irany */

    SDL_SetVideoMode(w, h, 0, SDL_OPENGL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35.0, w / (double) h, 1, 200);
    glEnable(GL_DEPTH_TEST);            /* z-tarazas bekapcsolasa */
    glEnable(GL_LIGHTING);              /* fenyek szamitasa */
    glEnable(GL_COLOR_MATERIAL);        /* az alakzatok anyaganak szinet a sokszogek szine adja meg */
    glEnable(GL_LIGHT0);                /* default ertekekkel: szemtol elore, feher */
    glEnable(GL_LIGHT1);                /* sajat, ujabb feny beallitasa */
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcol);
    glLightfv(GL_LIGHT1, GL_POSITION, lightdir);
    glClearColor(0.5, 0.5, 1, 1);       /* hatterszin: az eg szine */
}


int main(int argc, char *argv[]) {
    double forgat;
    SDL_Event ev;
    int kilep;
    
    SDL_Init(SDL_INIT_EVERYTHING);

    grafika_init();
    
    kilep = 0;
    forgat = 0;
    while (!kilep) {
        rajzol(forgat);
        forgat = fmod(forgat+1, 360);
        
        SDL_Delay(40);
        
        if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
            kilep = 1;
    }
    
    SDL_Quit();
    
    return 0;
}
