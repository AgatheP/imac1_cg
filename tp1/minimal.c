#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

/*********************************************************************************  CONSTANTES  */
/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 700;
static unsigned int WINDOW_HEIGHT = 700;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

/* couleurs */
static const unsigned char COLORS[]={
    255,255,255,
    255,0,0,
    0,255,0,
    0,0,255,
    0,0,0,
    255,255,0
};
static const unsigned int NB_COLORS= sizeof(COLORS)/(3*sizeof(unsigned char)); //nombre de couleurs

/*********************************************************************************  STRUCTURES  */

typedef struct Point{
    float x,y;
    unsigned char r,g,b;
    struct Point* next;
} Point, *PointList;

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
}Primitive, *PrimitiveList;

/*********************************************************************************  FONCTIONS  */
float getOpenglX(int x){
    return (2.0*x)/WINDOW_WIDTH-1;
}
float getOpenglY(int y){
    return -(2.0*y/WINDOW_HEIGHT-1);
}

void windowResize(int height, int width){
    WINDOW_WIDTH=width;
    WINDOW_HEIGHT=height;
    printf("Globales: h:%d, w:%d\n",WINDOW_HEIGHT,WINDOW_WIDTH );
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.,1.,-1.,1.);
    SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,BIT_PER_PIXEL,SDL_OPENGL | SDL_RESIZABLE);
}

void drawPalette(){
int i;
    float dx = 2.f / NB_COLORS; // dx = "delta x"
    glBegin(GL_QUADS);
    for(i = 0; i < NB_COLORS; ++i) {
        glColor3ubv(COLORS + i * 3);
        glVertex2f(-1 + i * dx, -1);
        glVertex2f(-1 + (i + 1) * dx, -1);
        glVertex2f(-1 + (i + 1) * dx, 1);
        glVertex2f(-1 + i  * dx, 1);
    }
    glEnd();
}

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b){
    Point* p=(Point*) malloc(sizeof(Point));
    if(!p){
        exit(1);
    }
    p->x=x;
    p->y=y;
    p->r=r;
    p->g=g;
    p->b=b;
    p->next=NULL;
    return p;
}

void addPointToList(Point* point, PointList* list){
    if(list==NULL){
        *list=point;
        return;
    }
    addPointToList(point, &(*list)->next);
}

void drawPoints(PointList list){
    while(list!=NULL){ //poir chaque points
        glColor3ub(list->r,list->g,list->b);
        glVertex2f(list->x, list->y);
        list=list->next;
    }
}
void freePoints(PointList* list){
    while(*list){
        Point* next=(*list)->next;
        free(*list);
        *list=next;
    }
}


Primitive* allocPrimitive(GLenum primitiveType){
    Primitive* p=(Primitive*) malloc(sizeof(Primitive));
    if(!p){
        exit(1);
    }
    p->primitiveType=primitiveType;
    p->points=NULL;
    p->next=NULL;
    return p;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list) {
    if (*list == NULL) {
        *list = primitive;
    } else {
        addPrimitive(primitive, &(*list)->next);
    }
}

void drawPrimitives(PrimitiveList lst){
    while(lst){ //pour chaque élément de la liste
        glBegin(lst->primitiveType);
        drawPoints(lst->points);
        glEnd();

        lst= lst->next;
    }
}

void freePrimitives(PrimitiveList* list){
    while(*list) {
        Primitive* next = (*list)->next;
        freePoints(&(*list)->points);
        free(*list);
        *list = next;
    }
}

int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    glClearColor(0.1,0.1,0.1,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Titre de la fenêtre */
    SDL_WM_SetCaption("Hello l'IMAC!", NULL);

    // création d'un primitive par defaut
    PrimitiveList primitives=NULL;
    Primitive* prim=allocPrimitive(GL_POINTS);
    PrimitiveList lastPrim=prim;
    addPrimitive(prim,&primitives);


    /* Boucle d'affichage */
    int loop = 1;
    int mode =0; //mode d'affichage (0=dessin, 1=palette)
    unsigned int currentColor=0;
    GLenum currentPrimitiveType=GL_POINTS;
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Placer ici le code de dessin */
        glClear(GL_COLOR_BUFFER_BIT);
        if(mode==0){
            drawPrimitives(primitives); //dessiner la liste des primitives
        }else{
            drawPalette();
        }
        
        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT) {
                loop = 0;
                break;
            }

            /* Quelques exemples de traitement d'evenements : */
            switch(e.type) {

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    printf("clic en (%d, %d)\n", e.button.x, e.button.y);
                    if(mode==1){
                        //mode palette
                        currentColor=(e.button.x*NB_COLORS)/WINDOW_WIDTH;
                    }
                    else if(mode==0){
                        //mode dessin
                        /*Transformation des coordonnées en coordonées openGL*/
                        float x=getOpenglX(e.button.x);
                        float y=getOpenglY(e.button.y);
                        //obtenit la couleur
                        unsigned int r=COLORS[currentColor*3];
                        unsigned int g=COLORS[currentColor*3]+1;
                        unsigned int b=COLORS[currentColor*3]+2;
                        //ajouter le nouveau point
                        Point* tmp=allocPoint(x,y,r,g,b);
                        addPointToList(tmp,&lastPrim->points);
                    }
                    break;

                /* Mouvement souris */
                case SDL_MOUSEMOTION:
                    break;

                /* Touche clavier */
                case SDL_KEYDOWN:
                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);
                    
                    int newPrimitivePressed =0;
                    GLenum newPrimitiveType;

                    switch(e.key.keysym.sym){
                        case SDLK_q:
                            printf("On quite le programme.\n");
                            SDL_Quit();
                            return EXIT_SUCCESS;
                            break;

                        case SDLK_SPACE:
                            mode=1;
                            break;

                        case SDLK_p:
                            newPrimitivePressed =1;
                            newPrimitiveType=GL_POINTS;
                            printf("Dessin de points.\n");
                            break;

                        case SDLK_l:
                            newPrimitivePressed =1;
                            newPrimitiveType=GL_LINES;
                            printf("Dessin de lignes.\n");
                            break;

                        case SDLK_t:
                            newPrimitivePressed=1;
                            newPrimitiveType=GL_TRIANGLES;
                            printf("Dessin de triangles.\n");
                            break;
                        case SDLK_c:
                            freePrimitives(&primitives);
                            printf("CLEAR\n");
                            break;
                        default: break;
                    }
                    if(newPrimitivePressed && currentPrimitiveType!=newPrimitiveType){
                        //on change de type de dessin
                        prim =allocPrimitive(newPrimitiveType);
                        lastPrim=prim;
                        addPrimitive(prim, &primitives);
                        currentPrimitiveType=newPrimitiveType;
                    }

                    break;

                // touche relachée
                case SDL_KEYUP:
                    if(e.key.keysym.sym ==SDLK_SPACE){
                        mode =0;
                    }
                    break;

                /* Redimentionnement de la fenetre */
                case SDL_VIDEORESIZE:
                    printf("window resize\n");
                    //printf("Nouvelle taille: h:%d, w:%d\n",e.resize.h,e.resize.w);
                    windowResize(e.resize.h,e.resize.w);
                    break;

                default:
                    break;
            }
        }
        /* Echange du front et du back buffer : mise à jour de la fenêtre */
        SDL_GL_SwapBuffers();

        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    freePrimitives(&primitives);
    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}
