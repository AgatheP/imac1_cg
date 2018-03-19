#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 900;
static unsigned int WINDOW_HEIGHT = 900;
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
    102, 0, 153, //9=violet
    150, 150, 150 //10=gris
};
static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-180,180,-180,180);
    //gluOrtho2D(-300,300,-300,300);
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

void drawTexture(GLuint texture){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    float pi=3.14159;
    glBegin(GL_POLYGON);
        for (int i = 0; i <= NB_COTES_CERCLE; ++i){
            glTexCoord2f((cos(2*pi*i/NB_COTES_CERCLE))/2, (sin(2*pi*i/NB_COTES_CERCLE))/4); 
            glVertex2f((cos(2*pi*i/NB_COTES_CERCLE)), (sin(2*pi*i/NB_COTES_CERCLE)));
            glTexCoord2f((cos(2*pi*(i+1)/NB_COTES_CERCLE))/2, (sin(2*pi*(i+1)/NB_COTES_CERCLE))/4);
            glVertex2f((cos(2*pi*(i+1)/NB_COTES_CERCLE)), (sin(2*pi*(i+1)/NB_COTES_CERCLE)));
        }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}

void drawPlanete(float rayon, float distance, unsigned int couleur){
    glPushMatrix();
        glColor3ub(COLORS[couleur*3],COLORS[couleur*3+1],COLORS[couleur*3+2]);
        glTranslatef(distance,0,0);
        glScalef(rayon,rayon,0);
        drawFilledCircle();
    glPopMatrix();
    glColor3ub(255,255,255);
}
void drawTexturedPlanete(float rayon, float distance, GLuint texture){
    glPushMatrix();
        glTranslatef(distance,0,0);
        glScalef(rayon,rayon,0);
        drawTexture(texture);
    glPopMatrix();
}

void drawBackground(GLuint texture){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glPushMatrix();
        glScalef(WINDOW_WIDTH,WINDOW_HEIGHT,0);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(-0.5,0.5);
            glTexCoord2f(1,0);
            glVertex2f(0.5,0.5);
            glTexCoord2f(1,1);
            glVertex2f(0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex2f(-0.5,-0.5);
        glEnd();
    glPopMatrix();
    drawLandmark();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);

}

void drawBigLandmark(int zoom){
    glPushMatrix();
        glScalef(zoom,zoom,0);
        drawLandmark();
    glPopMatrix();
}


GLuint loadTexture(const char* path){
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
        GL_RGB,
        surface->w,surface->h,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels
        );

    //détacher la texture du point de bind (debinder la texture)
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
    SDL_FreeSurface(surface);
    return textureID;
}

float* createRandomAsteroid(float distMin, float distMax){
    //asteroid[]=[angle,distance,taille,r,v,b]
    float asteroid[6]=NULL;
    return asteroid;
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

    //à quel angle se trouve les planetes
    float angles[8]={0,0,0,0,0,0,0,0};
    float distMinAsteroid, distMaxAsteroid;

    //chargement des textures
    GLuint textures[11];
    textures[0]=loadTexture("./sun.jpg");//soleil
    textures[1]=loadTexture("./mercury");//mercure
    textures[2]=loadTexture("./venus_atmosphere.jpg"); //venus
    textures[3]=loadTexture("./earth_daymap.jpg"); //terre
    textures[4]=loadTexture("./moon.jpg");//lune
    textures[5]=loadTexture("./mars.jpg"); //mars
    textures[6]=loadTexture("./jupiter.jpg");//jupiter
    textures[7]=loadTexture("./saturn.jpg"); //saturn
    textures[8]=loadTexture("./uranus.jpg");
    textures[9]=loadTexture("./neptune.jpg");
    textures[10]=loadTexture("./stars_milky_way.jpg");

    //initialiser les valeurs des astéroids
    int nbAsteroid=10;
    /*
    asteroid[]=[angle,distance,taille,r,v,b]
    asteroidBelt[asteroid1, asteroid2...]
    */

    while(loop){
        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        /*On exprimera la vitesse de planettes en fonction de la vitesse de Neptune 
        pour une représentation correcte.
        Note: Neptune se déplace à 18 000 km/h*/
        float vN=0.3;

        /* Code de dessin */
        //dessiner le fond
        drawBackground(textures[10]);

        float dist=0;

        //solei
        float rsoleil=19;
        //drawPlanete(rsoleil,dist,8);
        drawTexturedPlanete(rsoleil,dist,textures[0]);
        glPopMatrix();

        //mercure
        float rmerc=1;
        dist=rsoleil+5;
        angles[0]=(angles[0]+9.6*vN);
        glPushMatrix();
            glRotatef(angles[0],0.0,0.0,1.0);
            //drawPlanete(rmerc,dist,0);
            drawTexturedPlanete(rmerc,dist,textures[1]);
        glPopMatrix();
        //venus
        float rven=2;
        dist=dist+rmerc+5;
        angles[1]=(angles[1]+7*vN);
        glPushMatrix();
            glRotatef(angles[1],0.0,0.0,1.0);
            //drawPlanete(rven,dist,5);
            drawTexturedPlanete(rven,dist,textures[2]);
        glPopMatrix();

        //terre
        float rterre=rven;
        dist=dist+rven+5;
        angles[2]=(angles[2]+5.7*vN);
        glPushMatrix();
            glRotatef(angles[2],0.0,0.0,1.0);
            //drawPlanete(rterre,dist,6);
            drawTexturedPlanete(rterre,dist,textures[3]);
            //lune
            glPushMatrix();
                //se placer sur la planette
                glTranslatef(dist,0.0,0.0);
                glRotatef(angles[0],0.0,0.0,1.0);
                //drawPlanete(1,5,0);
                drawTexturedPlanete(1,5,textures[4]);
            glPopMatrix();
            
        glPopMatrix();

        //mars
        float rmars=1;
        dist=dist+rmars+5;
        angles[3]=(angles[3]+4.69*vN);
        glPushMatrix();
            glRotatef(angles[3],0.0,0.0,1.0);
            //drawPlanete(rmars,dist,2);
            drawTexturedPlanete(rmars,dist,textures[5]);
        glPopMatrix();
        distMinAsteroid = dist+rmars;

        //jupiter
        float rjup=14;
        dist=dist+rjup+15;
        angles[4]=(angles[4]+2.6*vN);
        glPushMatrix();
            glRotatef(angles[4],0.0,0.0,1.0);
            //drawPlanete(rjup,dist,5);
            drawTexturedPlanete(rjup,dist,textures[6]);

            //LUNES
            glPushMatrix();
                glTranslatef(dist,0.0,0.0);
                //drawBigLandmark(100);
                //Io
                glPushMatrix();
                    glRotatef(angles[0],0.0,0.0,1.0);
                    drawPlanete(1,2+rjup,0);
                glPopMatrix();
                //Europa
                glPushMatrix();
                    glRotatef(angles[1],0.0,0.0,1.0);
                    drawPlanete(0.9,4+rjup,0);
                glPopMatrix();
                //Ganymede
                glPushMatrix();
                    glRotatef(-angles[2],0.0,0.0,1.0);
                    drawPlanete(1.5,6+rjup,0);
                glPopMatrix();
                //callisto
                glPushMatrix();
                    glRotatef(angles[3],0.0,0.0,1.0);
                    drawPlanete(1.4, 8+rjup,0);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        distMaxAsteroid=dist-rjup;

        //saturn
        float rsaturn=12;
        dist=dist+rsaturn+15;
        angles[5]=(angles[5]+2.25*vN);
        glPushMatrix();
            glRotatef(angles[5],0.0,0.0,1.0);
            //drawPlanete(rsaturn,dist,4);
            drawTexturedPlanete(rsaturn,dist,textures[7]);
        glPopMatrix();

        //Uranus
        float ruran=5;
        dist=dist+ruran+30;
        angles[6]=(angles[6]+1.4*vN);
        glPushMatrix();
            glRotatef(angles[6],0.0,0.0,1.0);
            //drawPlanete(ruran,dist,7);
            drawTexturedPlanete(ruran,dist,textures[8]);
        glPopMatrix();

        //Neptune
        float rnept=5;
        dist=dist+rnept+30;
        angles[7]=angles[7]+vN;
        glPushMatrix();
            glRotatef(angles[7],0.0,0.0,1.0);
            //drawPlanete(rnept,dist,7);
            drawTexturedPlanete(rnept,dist,textures[9]);
        glPopMatrix();


        //ceinture d'astéroid entre Mars et Jupiter
        

        //réduire les angles pour ne pas avoir de valeurs trop grandes
        int i;
        for (i = 0; i < 8; ++i)
        {
            angles[i]=(int)angles[i]%360+(angles[i]-(int) angles[i]);
        }

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
    for (int i = 0; i < 11; ++i)
    {
        glDeleteTextures(1,&textures[i]);
    }
    
    return EXIT_SUCCESS;
}