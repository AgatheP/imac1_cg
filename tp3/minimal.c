#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 600;
static unsigned int CIRCLE_SEG =20;

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
    int zoom=17;
    gluOrtho2D(-8*zoom, 8*zoom, -6*zoom, 6*zoom);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

/*******************************************************************Structure et fonctions sur POINT */
typedef struct Point {
    float x, y;
    unsigned char r, g, b;
    struct Point* next;
} Point, *PointList;

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    Point* point = (Point*) malloc(sizeof(Point));
    if (!point) {
        return NULL;
    }
    point->x = x;
    point->y = y;
    point->r = r;
    point->g = g;
    point->b = b;
    point->next = NULL;
    return point;
}

void addPointToList(Point* point, PointList* list) {
    assert(point);
    assert(list);
    if (*list == NULL) {
        *list = point;
    } else {
        addPointToList(point, &(*list)->next);
    }
}

void drawPoints(PointList list) {
    while(list) {
        glColor3ub(list->r, list->g, list->b);
        glVertex2f(list->x, list->y);
        list = list->next;
    }
}

void deletePoints(PointList* list) {
    assert(list);
    while (*list) {
        Point* next = (*list)->next;
        free(*list);
        *list = next;
    }
}
/*******************************************************************Structure et fonctions sur PRIMITIVE */
typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;

Primitive* allocPrimitive(GLenum primitiveType) {
    Primitive* primitive = (Primitive*) malloc(sizeof(Primitive));
    if (!primitive) {
        return NULL;
    }
    primitive->primitiveType = primitiveType;
    primitive->points = NULL;
    primitive->next = NULL;
    return primitive;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list) {
    assert(primitive);
    assert(list);
    if (*list == NULL) {
        *list = primitive;
    } else {
        addPrimitive(primitive, &(*list)->next);
    }
}

void drawPrimitives(PrimitiveList list) {
    while(list) {
        glBegin(list->primitiveType);
        drawPoints(list->points);
        glEnd();
        list = list->next;
    }
}

void deletePrimitive(PrimitiveList* list) {
    assert(list);
    while(*list) {
        Primitive* next = (*list)->next;
        deletePoints(&(*list)->points);
        free(*list);
        *list = next;
    }
}

/*******************************************************************DESSIN de primitive */
void drawLandmark(PrimitiveList* list){
    //axe des X
    Primitive* ax=allocPrimitive(GL_LINE_STRIP);
    addPointToList(allocPoint(0,0,255,0,0),&ax->points);
    addPointToList(allocPoint(1,0,255,0,0),&ax->points);
    addPrimitive(ax, list);
    //axe des Y
    Primitive* ay=allocPrimitive(GL_LINE_STRIP);
    addPointToList(allocPoint(0,0,0,255,0),&ay->points);
    addPointToList(allocPoint(0,1,0,255,0),&ay->points);
    addPrimitive(ay,list);
}

void drawSquare(PrimitiveList* list, int full){
    //printf("SQUARE: full=%d\n", full);
    unsigned int r = COLORS[CURRENTCOLOR * 3];
    unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
    unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
    Primitive* p;
    if(full){
        p=allocPrimitive(GL_QUADS);
        glBegin(GL_QUADS);
    }else{
        p=allocPrimitive(GL_LINE_LOOP);
        glBegin(GL_LINE_LOOP);
    }
    glColor3ub(r,g,b);
    glVertex2f(-0.5,0.5);
    addPointToList(allocPoint(-0.5,0.5, r, g, b), &p->points);
    glVertex2f(0.5,0.5);
    addPointToList(allocPoint(0.5,0.5, r, g, b), &p->points);
    glVertex2f(0.5,-0.5);
    addPointToList(allocPoint(0.5,-0.5, r, g, b), &p->points);
    glVertex2f(-0.5,-0.5);
    addPointToList(allocPoint(-0.5,-0.5, r, g, b), &p->points);
    addPrimitive(p,list);
    glEnd();
    return;
}

void drawCircle(PrimitiveList* list, int full){
    unsigned int r = COLORS[CURRENTCOLOR * 3];
    unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
    unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
    float angle;
    float pi=3.1415;
    int i;
    Primitive* cercle;
    if(full){
        cercle=allocPrimitive(GL_POLYGON);
        glBegin(GL_POLYGON);
    }else{
        cercle=allocPrimitive(GL_LINE_LOOP);
        glBegin(GL_LINE_LOOP);
    }
    for(i = 1; i <= CIRCLE_SEG; ++i){
        angle=((2*pi)/CIRCLE_SEG)*i;
        //printf("point en X:%f Y:%f\n",cos(angle),sin(angle));
        glColor3ub(r,g,b);
        glVertex2f(cos(angle),sin(angle));
        addPointToList(allocPoint(cos(angle),sin(angle),r,g,b),&cercle->points);
    }
    addPrimitive(cercle,list);
    glEnd();
    return;
}

void drawRoundedSquare(){
    PrimitiveList plist2=NULL;
    //matrice des cercles
    //CURRENTCOLOR = 7;
    glPushMatrix();
        glScalef(0.125,0.125,0);
        glPushMatrix();
            glTranslatef(3.0,3.0,0);
            drawCircle(&plist2,1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-3.0,3.0,0);
            drawCircle(&plist2,1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(3.0,-3.0,0);
            drawCircle(&plist2,1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(-3.0,-3.0,0);
            drawCircle(&plist2,1);
        glPopMatrix();
    glPopMatrix();

    //rectangle 1:
    //CURRENTCOLOR=8;
    glPushMatrix();
        glScalef(1.0,0.75,0);
        drawSquare(&plist2,1);
    glPopMatrix();

    //rectangle 2:
    //CURRENTCOLOR=3;
    glPushMatrix();
        glScalef(0.75,1,0);
        drawSquare(&plist2,1);
    glPopMatrix();
}

void drawFirstArm(){
    PrimitiveList plist=NULL;
    //PrimitiveList plist2=NULL;
    //drawLandmark(&plist2);
    //carré central
    unsigned int r = COLORS[CURRENTCOLOR * 3];
    unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
    unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
    glBegin(GL_POLYGON);
    glColor3ub(r,g,b);
    glVertex2f(0,20);
    glVertex2f(60,10);
    glVertex2f(60,-10);
    glVertex2f(0,-20);
    glEnd();
    //CURRENTCOLOR=3;
    //grand CERCLE
    glPushMatrix();
        glScalef(20.0,20.0,0);
        drawCircle(&plist,0);
    glPopMatrix();
    //petit cercle CERCLE
    glPushMatrix();
        glTranslatef(60,0,0);
        glScalef(10.0,10.0,0);
        drawCircle(&plist,0);
    glPopMatrix();

    //drawPrimitives(plist2);
}

void drawSecondArm(){
    Primitive* list=NULL;
    //premier carré
    glPushMatrix();
        glScalef(10.0,10.0,0);
        drawRoundedSquare();
    glPopMatrix();
    //carré 2
    glPushMatrix();
        glTranslatef(44.5,0,0);
        glScalef(10.0,10.0,1);
        drawRoundedSquare();
    glPopMatrix();
    //rectangle
    glPushMatrix();
        glTranslatef(21.5,0,0);
        glScalef(46,6,0);
        //drawLandmark(&list);
        drawSquare(&list,1);
        //drawPrimitives(list);
    glPopMatrix();
}

void drawThirdArm(){
    Primitive* list=NULL;
    //carré
    glPushMatrix();
        glScalef(6.0,6.0,1);
        drawRoundedSquare();
    glPopMatrix();
    //rectangle
    glPushMatrix();
        glScalef(40,4,1);
        glTranslatef(0.46,0,0);
        drawSquare(&list,1);
    glPopMatrix();
    //cercle
    glPushMatrix();
        glTranslatef(37,0,0);
        glScalef(4,4,0);
        drawCircle(&list,1);
    glPopMatrix();
}

GLuint createFirstArmIDList(){
    GLuint id=glGenLists(1);
    glNewList(id,GL_COMPILE);
    PrimitiveList plist=NULL;
    //grand CERCLE
    glPushMatrix();
        glScalef(20.0,20.0,0);
        drawCircle(&plist,0);
    glPopMatrix();
    //petit cercle CERCLE
    glPushMatrix();
        glTranslatef(60,0,0);
        glScalef(10.0,10.0,0);
        drawCircle(&plist,0);
    glPopMatrix();
    unsigned int r = COLORS[CURRENTCOLOR * 3];
    unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
    unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
    glBegin(GL_POLYGON);
    glColor3ub(r,g,b);
    glVertex2f(0,20);
    glVertex2f(60,10);
    glVertex2f(60,-10);
    glVertex2f(0,-20);
    glEnd();
    glEndList();
    return id;
}

GLuint creatSecondArmIDList(){
    GLuint id=glGenLists(1);
    glNewList(id,GL_COMPILE);
    Primitive* list=NULL;
    //premier carré
    glPushMatrix();
        glScalef(10.0,10.0,0);
        drawRoundedSquare();
    glPopMatrix();
    //carré 2
    glPushMatrix();
        glTranslatef(44.5,0,0);
        glScalef(10.0,10.0,1);
        drawRoundedSquare();
    glPopMatrix();
    //rectangle
    glPushMatrix();
        glTranslatef(21.5,0,0);
        glScalef(46,6,0);
        //drawLandmark(&list);
        drawSquare(&list,1);
        //drawPrimitives(list);
    glPopMatrix();
    glEndList();
    return id;
}

GLuint createThirdArmIDList(){
    GLuint id=glGenLists(1);
    glNewList(id,GL_COMPILE);
    Primitive* list=NULL;
    //carré
    glPushMatrix();
        glScalef(6.0,6.0,1);
        drawRoundedSquare();
    glPopMatrix();
    //rectangle
    glPushMatrix();
        glScalef(40,4,1);
        glTranslatef(0.46,0,0);
        drawSquare(&list,1);
    glPopMatrix();
    //cercle
    glPushMatrix();
        glTranslatef(37,0,0);
        glScalef(4,4,0);
        drawCircle(&list,1);
    glPopMatrix();
    glEndList();
    return id;
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
    SDL_WM_SetCaption("TP3", NULL);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* On créé une première primitive par défaut */
    PrimitiveList primitives = NULL;
    Primitive* prim = allocPrimitive(GL_POINTS);
    PrimitiveList lastPrim = prim;
    addPrimitive(prim, &primitives);
    int loop =1;
    CURRENTCOLOR = 0; // l'index de la couleur courante dans le tableau COLORS
    GLenum currentPrimitiveType = GL_POINTS;

    float alpha=40;
    float beta=-10;
    float gamma=35;

    while(loop){
        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Code de dessin */
        glClear(GL_COLOR_BUFFER_BIT);
        drawPrimitives(primitives);
        //dessin des bras
        alpha=(int)(alpha+1)%360;
        beta=(int)(beta+1)%360;
        gamma=(int)(gamma+1)%360;
        glPushMatrix();
            CURRENTCOLOR = 3;
            glRotatef(alpha,0.0,0.0,1.0);
            glCallList(createFirstArmIDList());
            CURRENTCOLOR = 4;
            glTranslatef(60,0,0);
            glRotatef(beta,0.0,0.0,1.0);
            glCallList(creatSecondArmIDList());

            glTranslatef(45,0,0);
            glPushMatrix();
                CURRENTCOLOR = 5;
                glRotatef(gamma,0.0,0.0,1.0);
                glCallList(createThirdArmIDList());
            glPopMatrix();
            glPushMatrix();
                CURRENTCOLOR = 6;
                glRotatef((int)(gamma+alpha)%360,0.0,0.0,1.0);
                glCallList(createThirdArmIDList());
            glPopMatrix();

            glScalef(10,10,1);
            drawLandmark(&primitives);
            drawPrimitives(primitives);
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

                // clique souris
                case SDL_MOUSEBUTTONUP:

                    if(e.button.button == SDL_BUTTON_RIGHT){
                        printf("Clique droit\n");
                        if(lastPrim->primitiveType==GL_LINE_STRIP){ //si la dernière primitive est un trait
                            /* Transformation des coordonnées du clic souris en coordonnées OpenGL */
                            float x =( -1 + 2. * e.button.x / WINDOW_WIDTH)*4;
                            float y = -(-1 + 2. * e.button.y / WINDOW_HEIGHT)*3;
                            //fermer la ligne tracée
                            unsigned int r = COLORS[CURRENTCOLOR * 3];
                            unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
                            unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
                            addPointToList(allocPoint(x, y, r, g, b), &lastPrim->points);
                            lastPrim->primitiveType=GL_LINE_LOOP;
                            // débuter une nouvelle ligne
                            prim = allocPrimitive(GL_LINE_STRIP);
                            lastPrim = prim;
                            addPrimitive(prim, &primitives);
                            currentPrimitiveType = GL_LINE_STRIP;
                        }

                    }else{
                        printf("Clique gauche\n");
                        /* Transformation des coordonnées du clic souris en coordonnées OpenGL */
                        float x =( -1 + 2. * e.button.x / WINDOW_WIDTH)*4;
                        float y = -(-1 + 2. * e.button.y / WINDOW_HEIGHT)*3;
                        /* On retrouve la couleur courante grace à son index */
                        unsigned int r = COLORS[CURRENTCOLOR * 3];
                        unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
                        unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
                        /* On ajoute un nouveau point à la liste de la primitive courante */
                        addPointToList(allocPoint(x, y, r, g, b), &lastPrim->points);
                    }
                    break;

                /* Touche de clavier*/
                case SDL_KEYDOWN:
                    printf("touche pressée (code=%d)\n",e.key.keysym.sym );

                    int newPrimitivePressed=0;
                    GLenum newPrimitiveType;
                    switch(e.key.keysym.sym){
                        case SDLK_q:
                            loop=0;
                            break;
                        case SDLK_l:
                            printf("Draw line.\n");
                            newPrimitivePressed =1;
                            newPrimitiveType =GL_LINE_STRIP;
                            break;

                        case SDLK_s:
                            printf("Draw square!\n");
                            drawSquare(&primitives, 0);
                            break;
                        case SDLK_m:
                            printf("Draw landmark\n");
                            drawLandmark(&primitives);
                            break;
                        case SDLK_e:
                            printf("drawCircle\n");
                            drawCircle(&primitives, 0);
                            break;
                        case SDLK_c:
                            /* Touche pour effacer le dessin */
                            deletePrimitive(&primitives); // on supprime les primitives actuelles
                            addPrimitive(allocPrimitive(currentPrimitiveType), &primitives); // on réinitialise à la primitive courante
                            break;
                        
                        default:
                            break;

                    }
                    // gérér le nouveau type
                    if (newPrimitivePressed && currentPrimitiveType != newPrimitiveType) {
                        prim = allocPrimitive(newPrimitiveType);
                        lastPrim = prim;
                        addPrimitive(prim, &primitives);
                        currentPrimitiveType = newPrimitiveType;
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

    deletePrimitive(&primitives);
    SDL_Quit();

    return EXIT_SUCCESS;
}