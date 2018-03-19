#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 600;
static unsigned int WINDOW_HEIGHT = 600;
static unsigned int NB_COTES_CERCLE =60;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

static const unsigned char COLORS[] = {
    255, 255, 255, //0 =blanc
    0, 0, 0, //1=noir
    255, 0, 0, //2=rouge
    0, 255, 0, // 3 =vert
    0, 0, 255, //4= bleu
    255, 255, 0, //5=jaune
    0, 255, 255, // 6=cyan
    255, 0, 255, // 7=majenta
    237, 127,16, // 8=orange
    102, 0, 153 //9=violet
};
static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));
unsigned int CURRENTCOLOR;

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

/*******************************************************************DESSIN de primitive */

void drawLandmark() {
  glBegin(GL_LINES);
    glColor3ub(255, 0, 0);
    glVertex2f(0, 0);
    glVertex2f(1, 0);
    glColor3ub(0, 255, 0);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glColor3ub(255, 255, 255);
  glEnd();
}

void drawSquare() {
  glBegin(GL_QUADS);
    glVertex2f( -0.5, -0.5);
    glVertex2f( -0.5, +0.5);
    glVertex2f( +0.5, +0.5);
    glVertex2f( +0.5, -0.5);
  glEnd();
}

void drawCircle() {
  glBegin(GL_LINES);
    float pi = 3.14159;
    for(int i = 0; i < NB_COTES_CERCLE; i++) {
      glVertex2f((cos(2*pi*i/NB_COTES_CERCLE)), (sin(2*pi*i/NB_COTES_CERCLE)));
      glVertex2f((cos(2*pi*(i+1)/NB_COTES_CERCLE)), (sin(2*pi*(i+1)/NB_COTES_CERCLE)));
    }
  glEnd();
}

void drawFilledCircle() {
  glBegin(GL_POLYGON);
    float pi = 3.14159;
    for(int i = 0; i < NB_COTES_CERCLE; i++) {
      glVertex2f((cos(2*pi*i/NB_COTES_CERCLE)), (sin(2*pi*i/NB_COTES_CERCLE)));
      glVertex2f((cos(2*pi*(i+1)/NB_COTES_CERCLE)), (sin(2*pi*(i+1)/NB_COTES_CERCLE)));
    }
  glEnd();
}

/***************************************************************** Autre fonctions */

float randfrom(float min, float max) {
    float range = (max - min); 
    float div = RAND_MAX / range;
    return min + (rand() / div);
}

int main(int argc, char** argv) {
    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    resizeViewport();
    SDL_WM_SetCaption("TP5", NULL);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    int loop =1;

    time_t rawtime;
    struct tm*timeinfo;
    float angle=0;

    while(loop){
        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        glClear(GL_COLOR_BUFFER_BIT);

        /* Gestion du temps */
        time(&rawtime);
        timeinfo=localtime(&rawtime);
        printf("%d : %d : %d \n",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
        /* Code de dessin */
        glColor3ub(255, 255, 255);
        drawFilledCircle();
        glColor3ub(0, 0, 0);
        glPushMatrix();
            glScalef(0.99,0.99,0);
            drawCircle();
        glPopMatrix();

        //afficher les crans //minutes
        int i;
        float increment=360.0/60.0;
        for(i=0;i<60;i++){
        glColor3ub(0, 0, 0);
            glPushMatrix();
                glRotatef(angle+i*increment,0.0,0.0,1.0);
                //drawLandmark();
                glTranslatef(0.85,0,0);
                glScalef(0.1,0.01,0);
                drawSquare();
            glPopMatrix();

            if(i%5==0){
                glPushMatrix();
                    glRotatef(angle+i*increment,0.0,0.0,1.0);
                    //drawLandmark();
                    glTranslatef(0.85,0,0);
                    glScalef(0.15,0.03,0);
                    drawSquare();
                glPopMatrix();
            }
        }
        // afficher les aiguilles

        //heure
        glPushMatrix();
            glRotatef(-(timeinfo->tm_hour*increment*5),0.0,0.0,1.0);
            //drawLandmark();
            glColor3ub(0, 0, 0);
            glTranslatef(0,0.22,0);
            glScalef(0.05,0.5,0);
            drawSquare();
        glPopMatrix();
        //minutes
        glPushMatrix();
            glRotatef(-(timeinfo->tm_min*increment),0.0,0.0,1.0);
            //drawLandmark();
            glColor3ub(0, 0, 0);
            glTranslatef(0,0.33,0);
            glScalef(0.01,0.7,0);
            drawSquare();
        glPopMatrix();

        //minutes
        glPushMatrix();
            glRotatef(-(timeinfo->tm_sec*increment),0.0,0.0,1.0);
            //drawLandmark();
            glColor3ub(255, 0, 0);
            glTranslatef(0,0.43,0);
            glScalef(0.005,0.9,0);
            drawSquare();
        glPopMatrix();



        /* Traitement des events*/
        SDL_Event e;
        while(SDL_PollEvent(&e)){

            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT) {
                loop = 0;
                break;
            }

            switch(e.type){

                /* Touche de clavier*/
                case SDL_KEYDOWN:
                    printf("touche pressée (code=%d)\n",e.key.keysym.sym );
                    switch(e.key.keysym.sym){
                        case SDLK_q:
                            loop=0;
                            break;
                        
                        default:
                            break;

                    }
                    break;
                /* Changer la taille de la fenetre*/
                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();
                    break;

                default:
                    //printf("Evenement non géré\n");
                    break;
            }
        } //fin de la boucle d'event

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }

    }
    SDL_Quit();

    return EXIT_SUCCESS;
}