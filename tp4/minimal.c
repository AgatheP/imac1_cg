#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 800;
static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

GLuint loadText(const char* path){
    //charger dans le CPU
    SDL_Surface* surface;
    surface = IMG_Load(path); 
    if(surface == NULL){
        printf("Erreur de IMG_Load\n");
        return EXIT_FAILURE;
    }

    //transférer dans le GPU
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->w,surface->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->pixels
        );

    //détacher la texture du point de bind (debinder la texture)
    glBindTexture(GL_TEXTURE_2D,0);
    return textureID;
}

const char* filename = "logo_imac_400x400.jpg";

int main(int argc, char** argv) {

    // Initialisation de la SDL
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    // Ouverture d'une fenêtre et création d'un contexte OpenGL
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("td04", NULL);
    resizeViewport();

    // TODO: Chargement et traitement de la texture

    //charger dans le CPU
    SDL_Surface* surface;
    surface = IMG_Load("./logo_imac_400x400.jpg"); 
    if(surface == NULL){
        printf("Erreur de IMG_Load\n");
        return EXIT_FAILURE;
    }

    //transférer dans le GPU
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        surface->w,surface->h,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels
        );

    //détacher la texture du point de bind (debinder la texture)
    glBindTexture(GL_TEXTURE_2D,0);

    // TODO: Libération des données CPU
    SDL_FreeSurface(surface);

    // charger les textures des chiffres
    GLuint texture0=loadText("./numbers/0.png");
    /* Boucle de dessin (à décommenter pour l'exercice 3)*/
    int loop = 1;
    glClearColor(0.1, 0.1, 0.1 ,1.0);
    while(loop) {

        Uint32 startTime = SDL_GetTicks();

        // TODO: Code de dessin
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D,texture0);

        //heures 1
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(-2.0,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();
        //heures 2
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(-1.5,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();

        //separation h/min
        /*glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(-1.0,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();*/

        //minutes 1
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(-0.5,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();
        //minutes 2
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(0,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();

        //separation min/sec
        /*glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(0.5,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();*/

        //secondes 1
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(1,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();
        //secondes 2
        glPushMatrix();
            glScalef(0.25,0.25,0);
            glTranslatef(1.5,0,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-0.25,0.25);
                glTexCoord2f(1,0);
                glVertex2f(0.25,0.25);
                glTexCoord2f(1,1);
                glVertex2f(0.25,-0.5);
                glTexCoord2f(0,1);
                glVertex2f(-0.25,-0.5);
            glEnd();
        glPopMatrix();


        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);

        // Fin du code de dessin

        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            switch(e.type) {

                case SDL_QUIT:
                    loop = 0;
                    break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();

                default:
                    break;
            }
        }

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }
    

    // TODO: Libération des données GPU
    glDeleteTextures(1,&textureID);
    glDeleteTextures(1,&texture0);

    // Liberation des ressources associées à la SDL
    //SDL_Delay(3000);
    SDL_Quit();

    return EXIT_SUCCESS;
}
