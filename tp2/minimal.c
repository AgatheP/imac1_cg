#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 600;
static unsigned int CIRCLE_SEG =20;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

typedef struct Point {
    float x, y;
    unsigned char r, g, b;
    struct Point* next;
} Point, *PointList;

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker un point
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Point*.
    */
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
        /* Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur le point donné en paramètre */
        *list = point;
    } else {
        /* Sinon, on réapplique la même fonction mais sur le point suivant */
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

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;

Primitive* allocPrimitive(GLenum primitiveType) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker une primitive
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Primitive*.
    */
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
        // Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur la primitive donnée en paramètre
        *list = primitive;
    } else {
        // Sinon, on réapplique la même fonction mais sur le point suivant
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

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-4.0, 4.0, -3.0, 3.0);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}


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
unsigned int CURRENTCOLOR;

static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));

void drawColorPalette() {
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

Primitive* drawSquare(PrimitiveList* list, int full){
	printf("SQUARE: full=%d\n", full);
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
    return p;
}

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

Primitive* drawCircle(PrimitiveList* list, int full){
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
        Point* p =allocPoint(cos(angle),sin(angle),r,g,b);
        glColor3ub(r,g,b);
        glVertex2f(cos(angle),sin(angle));
        addPointToList(p,&cercle->points);
    }
    addPrimitive(cercle,list);
    glEnd();
    return cercle;
}

float randfrom(float min, float max) {
    float range = (max - min); 
    float div = RAND_MAX / range;
    return min + (rand() / div);
}

int main(int argc, char** argv) {
    srand(time(NULL));
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
    SDL_WM_SetCaption("Paint IMAC", NULL);
    resizeViewport();

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* On créé une première primitive par défaut */
    PrimitiveList primitives = NULL;
    Primitive* prim = allocPrimitive(GL_POINTS);
    PrimitiveList lastPrim = prim;
    addPrimitive(prim, &primitives);

    int loop = 1;
    int mode = 0; // le mode d'affichage. 0 = le dessin, 1 = la palette
    int mouseX=0;
    int mouseY=0;
    CURRENTCOLOR = 0; // l'index de la couleur courante dans le tableau COLORS
    GLenum currentPrimitiveType = GL_POINTS;

    Primitive* Ys=NULL; //yellow square
    Primitive* Bc=NULL; //Blue circle
    float Bcx, Bcy;

    printf("POUR L'EXERCICE 3:\n le glClear et drawPrimitives on été désactivé pour dessiner les primitive une à une avec les changement de matrice. (press 'D')\n");
    /* Boucle d'affichage */
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Code de dessin */
        
        //glClear(GL_COLOR_BUFFER_BIT); // Toujours commencer par clear le buffer

        if (mode == 0) {
            //drawPrimitives(primitives); // On dessine la liste de primitives
        }
        else if (mode == 1) {
            drawColorPalette();
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
                /* Clic souris  (enfonssé)*/
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_RIGHT){
                       mouseX=e.button.x;
                       mouseY=e.button.y; 
                    }
                    break;

                /* Clic souris  (relaché)*/
                case SDL_MOUSEBUTTONUP:
                    if(e.button.button == SDL_BUTTON_RIGHT){
                        printf("Clique droit.\n");
                        float x = -1 + 2. * e.button.x / WINDOW_WIDTH;
                        float y = -(-1 + 2. * e.button.y / WINDOW_HEIGHT);
                        unsigned int r = COLORS[CURRENTCOLOR * 3];
                        unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
                        unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
                        addPointToList(allocPoint(x, y, r, g, b), &lastPrim->points);
                        lastPrim->primitiveType=GL_LINE_LOOP;

                        //réinitialiser
                        prim = allocPrimitive(GL_LINE_STRIP);
                        lastPrim = prim;
                        addPrimitive(prim, &primitives);
                        currentPrimitiveType = GL_LINE_STRIP;

                        //bouger le carré jaune
                        if(Ys!=NULL){
                            printf("Il y a un carré jaune dans la fenetre.\n");
                            glTranslatef(x*4,y*3,0);
                            if(e.button.x!=mouseX || e.button.y!=mouseY){
                                printf("DRAG AND DROP.\n");
                                glRotatef(mouseX-e.button.x,0.0,0.0,1.0);
                            }
                            drawPrimitives(Ys);
                            resizeViewport(); //réinitialise
                            mouseY=0;
                            mouseX=0;
                        }
                    }
                    if (mode == 1) {
                        /* On retrouve automatiquement l'index de la couleur cliquée en connaissant la taille de notre tableau */
                        CURRENTCOLOR = e.button.x * NB_COLORS / WINDOW_WIDTH;
                    }
                    else if (mode == 0) {
                        /* Transformation des coordonnées du clic souris en coordonnées OpenGL */
                        float x = -1 + 2. * e.button.x / WINDOW_WIDTH;
                        float y = -(-1 + 2. * e.button.y / WINDOW_HEIGHT);
                        /* On retrouve la couleur courante grace à son index */
                        unsigned int r = COLORS[CURRENTCOLOR * 3];
                        unsigned int g = COLORS[CURRENTCOLOR * 3 + 1];
                        unsigned int b = COLORS[CURRENTCOLOR * 3 + 2];
                        /* On ajoute un nouveau point à la liste de la primitive courante */
                        addPointToList(allocPoint(x, y, r, g, b), &lastPrim->points);
                    }
                    break;
                /* Touche clavier */
                case SDL_KEYDOWN:

                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);

                    if (e.key.keysym.sym == SDLK_SPACE) {
                        mode = 1;
                    }

                    int newPrimitivePressed = 0;
                    GLenum newPrimitiveType;

                    switch(e.key.keysym.sym) {
                    	case SDLK_d:
                    		printf("DESSINER LES FORMES\n");
                    		printf("Cercle orange\n");
                    		CURRENTCOLOR=8;
                    		drawCircle(&primitives,1);
                    		resizeViewport(); //pour réinitialiser la matrice

                    		printf("Carré rouge\n");
                    		CURRENTCOLOR=2;
                    		glRotatef(45.0,0.0,0.0,1.0);
                    		glTranslatef(2,0,0);
                    		drawSquare(&primitives,1);
                    		resizeViewport();

                    		printf("Carré violet\n (La translation se fait sur le repère donc sur le carré)\n");
                    		CURRENTCOLOR=9;
                    		glTranslatef(2,0,0);
                    		glRotatef(45.0,0.0,0.0,1.0);
                    		drawSquare(&primitives,1);
                    		resizeViewport();

                    		printf("Carré jaune\n");
                    		CURRENTCOLOR=5;
                    		Ys=drawSquare(&primitives,1);

                            printf("Cercle BLeu\n");
                            CURRENTCOLOR=4;
                            Bc=drawCircle(&primitives,0);
                            Bcx=0;
                            Bcy=0;
                    		break;

                        case SDLK_q:
                            loop = 0;
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

                    if (newPrimitivePressed && currentPrimitiveType != newPrimitiveType) {
                        prim = allocPrimitive(newPrimitiveType);
                        lastPrim = prim;
                        addPrimitive(prim, &primitives);
                        currentPrimitiveType = newPrimitiveType;
                    }

                    break;

                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_SPACE) {
                        mode = 0;
                    }
                    break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();

                default:
                    break;
            }
        }

        //si il ya un cercle bleu in le déplace
        if(Bc!=NULL){
            Bcx+=randfrom(-0.5,0.5);
            Bcy+=randfrom(-0.5,0.5);
            glTranslatef(Bcx,Bcy,0);
            drawPrimitives(Bc);
            resizeViewport();
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

    deletePrimitive(&primitives);

    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}