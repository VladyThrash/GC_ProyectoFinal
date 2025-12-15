//Hola vlady, aqui implementaremos el algortimo de busqueda del agente, asi como la expansión de la cola de dibujado, ya que la 
//animación no termina hasta que el agente llega al objetivo.
//De momento implemetaremos RRT, posteriormente se embeberan los demás algoritmos de búsqueda.

#ifndef Agente
#define Agente
#include<stdlib.h>
#include<time.h> //Para generar números aleatorios
#include<math.h>
#include "Estructuras_Genericas.h"
#include "Gestor_Entes.h"

//CONSTANTES Y GLOBALES
#define DELTA_AGENTE 5 //El tamaño
#define PASO 0.3 //El paso delta del algortimo RRT
#define MAX_ITER 50000
#define SEGMENTOS_GRID 1000
#define SESGO 6 
//#define TAM_ESCENARIO 100
struct nodoLista1D *nodosExistentes = NULL; //Nos ayuda a agilizar la busqueda (ya no dependemos del recorrido recursivo del grafo).
unsigned long int frames_agente;

//STRUCTS

struct nodoAgente{ //Coordenadas y colisión del agente.
    float x;
    float y;
    float deltaX;
    float deltaY;
    struct nodoLista2D *solucion;
};

struct Greedy{ //Guardar el valor de la heuristica para el algoritmo Greedy
    float h;
};

struct AStar{ //Guarda el valor de la heuristica, el coste y la suma de ambos para el algoritmo A*
    float h;
    float f;
    float sum;
};

//PROTOTIPOS
struct nodoAgente* nuevoNodoAgente(float *startXY, float deltaX, float deltaY);
int enColisionAgente(struct nodoLista1D *listaObst, struct nodoAgente *agente);
struct nodoLista1D* agregarAgente(float *startXY, float *targetXY, struct nodoLista1D *listaObst, int typeSearch);
struct nodoLista2D* rrt(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst);
struct nodoGrafoD* expandirEstados(float *targetXY, struct nodoLista1D *listaObst);
struct nodoGrafoD* obtenerMasCercano(float x, float y);
struct nodoLista2D* bpp(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst);
struct nodoLista2D* greedy(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst);
struct nodoLista2D* aEstrella(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst);
void* tipoHeuristica(int type);
int expandirGrid(struct nodoGrafoD *nodo, struct matrizHash *hash, struct nodoLista1D **listaEstados, struct nodoLista1D *listaObst, float *targetXY, int h, int f);
void insertarListaEstados(struct nodoLista1D **listaEstados, struct nodoGrafoD *nodo, int lambda);
int ordenarGreedy(struct nodoLista1D **lista, struct nodoGrafoD *nodo);
int ordenarAStar(struct nodoLista1D **lista, struct nodoGrafoD *nodo);
float distanciaEuclidiana(struct nodoAgente *agente, float x, float y);
void nuevasCoordenadas(float *nuevas, float *actual, float *random);
int enRango(struct nodoAgente *agente, float *targetXY);
int enEscenario(struct nodoAgente *agente);
struct nodoAgente* frameEspecificoAgente(struct nodoLista1D *agente, int frameAct);
int agregarVisitado(struct matrizHash *hash, struct nodoAgente *agente);
int ejeAleatorio();

//FUNCIONES

//Función para crear un nuevo nodoAgente.
struct nodoAgente* nuevoNodoAgente(float *startXY, float deltaX, float deltaY){
    struct nodoAgente *nodo = NULL;
    nodo = (struct nodoAgente*)malloc(sizeof(struct nodoAgente));
    if(!nodo){
        return NULL;
    } 
    nodo->x = startXY[0];
    nodo->y = startXY[1];
    nodo->deltaX = deltaX;
    nodo->deltaY = deltaY;
    nodo->solucion = NULL;
    return nodo;
}

//Función para verificar si un nodoAgente no se encuentra en colision con un obstaculo.
int enColisionAgente(struct nodoLista1D *listaObst, struct nodoAgente *agente){
    struct nodoLista1D *aux = listaObst;
    
    float xMin2 = (agente->x) - (agente->deltaX/2); //Intervalos de agente
    float xMax2 = (agente->x) + (agente->deltaX/2);
    float yMin2 = (agente->y) - (agente->deltaY/2);
    float yMax2 = (agente->y) + (agente->deltaY/2);

    while(aux){
        struct enteEstatico *ente = (struct enteEstatico*)aux->data;
        
        float xMin1 = (ente->x) - (ente->deltaX/2); //Intervalos de ente
        float xMax1 = (ente->x) + (ente->deltaX/2);
        float yMin1 = (ente->y) - (ente->deltaY/2);
        float yMax1 = (ente->y) + (ente->deltaY/2);

        int colX = (xMin1 <= xMax2 && xMin2 <= xMax1); //Dos intervalos A y B colisionan si:
        int colY = (yMin1 <= yMax2 && yMin2 <= yMax1); //Amin <= Bmax y Bmin <= Amax
        if(colX && colY){
            return 1;
        }
        aux = aux->next;
    }
    return 0;
}

//Esta función recibe el inicio y el objetivo de un agente, regresa el nodo con la solución o trayectoria.
//Nuevo: El parametro typeSearch determina el algortimo de busqueda - planificación a utilizar para hallar la trayectoria o solución del agente para trasladarse de start a target.
//       typeSearch -> 1 : BPP
//       typeSearch -> 2 : Greedy
//       typeSearch -> 3 : A*
//       typeSearch -> 4 : RRT
//       typeSearch -> 5 : Otro de planificación (toca investigarlo)
//       Si se ingresa un typeSearch no definido, por defecto se realizara la busqueda con RRT.
struct nodoLista1D* agregarAgente(float *startXY, float *targetXY, struct nodoLista1D *listaObst, int typeSearch){
    struct nodoAgente *agente = nuevoNodoAgente(startXY, DELTA_AGENTE, DELTA_AGENTE);
    if(!agente){
        printf("nuevoNodoAgente regreso NULL!!!\n");
        return NULL; //Error al generar el nodo agente.
    }

    struct nodoGrafoD* nodoInicial = nuevoNodoGrafo(agente, NULL);
    if(!nodoInicial){
        printf("nuevoNodoGrafo regreso NULL!!!\n");
        return NULL; //Error al generar el nodo grafo.
    }
    
    switch(typeSearch){ //Se obtiene la solución dado el tipo de busqueda requerido.
        case 1:
            //BPP
            printf("Busqueda BPP!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = bpp(nodoInicial, targetXY, listaObst);
            break;
        case 2:
            //GREEDY
            printf("Busqueda Greedy!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = greedy(nodoInicial, targetXY, listaObst);
            break;
        case 3:
            //A*
            printf("Busqueda A*!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = aEstrella(nodoInicial, targetXY, listaObst);
            break;
        case 4:
            //RRT
            printf("Busqueda RRT!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = rrt(nodoInicial, targetXY, listaObst);
            break;
        case 5:
            //No se xD
            printf("Busqueda con otro (de momento RRT)!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = rrt(nodoInicial, targetXY, listaObst); //<-- RRT por ahora
            break;
        default:
            //A*
            printf("Busqueda A*!!!\n");
            ((struct nodoAgente*)nodoInicial->data)->solucion = aEstrella(nodoInicial, targetXY, listaObst);
            break;
    }

    if(!((struct nodoAgente*)nodoInicial->data)->solucion){
        return NULL;
    }
    
    struct nodoLista1D *nuevo = nuevoNodoLista1D(nodoInicial);
    if(!nuevo){
        printf("Error al generar el nodoLista1D que contiene al agente!!!\n");
        return NULL;
    }
    nuevo->data = nodoInicial;
    return nuevo;
}

//ALGORITMOS DE PLANIFICACIÓN DE MOVIMIENTOS

//Algoritmo de búsqueda RRT (Rapidly-exploring Random Tree).
//Se generan puntos aleatorios en el espacio, el arbol o grafo crece en dirección hacia el punto generado, hasta que 
//uno de los nodos se encuentra lo suficientemente cerca del objetivo.
struct nodoLista2D* rrt(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst){
    frames_agente = 0;
    struct nodoLista2D *solucion = NULL;
    if( enRango( (struct nodoAgente*)nodoInicial->data, targetXY ) ){ //El agente esta tocando el objetivo.
        if(insertarNodoLista2D(&solucion, nodoInicial)){
            frames_agente++;
            return solucion;
        }
        return NULL; //No se pudo insertar en la lista de solución.
    }
    insertarNodoLista1D(&nodosExistentes, nodoInicial); //Insertamos a la lista de nodos existentes.

    int iter = 0;
    while(iter < MAX_ITER){ //Número maximo de iteraciones de la búsqueda.
        struct nodoGrafoD *nN = expandirEstados(targetXY, listaObst);
        if(!nN){
            continue; //No se pudo generar el nuevo nodo.
        }
        if( enRango( (struct nodoAgente*)nN->data, targetXY ) ){ //El nuevo nodo esta tocando al objetivo
            //Hacemos el backtraking
            insertarNodoLista2D(&solucion, nN);
            frames_agente++;
            struct nodoGrafoD *padre = nN->padre;
            while(padre){
                insertarNodoLista2D(&solucion, padre);
                frames_agente++;
                padre = padre->padre;
            }
            return solucion;
        }

        iter++;
    }
    printf("Se acabaron las iteraciones en RRT!!!\n"); //<--- SE ACABAN LAS ITERACIONES, DEBO AUMENTARLAS O AUMENTAR EL PASO DEL AGENTE
    return NULL; //Se acabaron las iteraciones.
}

//Esta función expande el estado, aqui se genera el nodo mas cercano al punto aleatorio.
struct nodoGrafoD* expandirEstados(float *targetXY, struct nodoLista1D *listaObst){
    //Primero definimos el punto aleatorio sobre el escenario, pero vamos a incluir un sesgo para que el algoritmo converja mas rapido.
    int sesgo = numeroAleatorio(1, 10);
    float x, y;
    if(sesgo <= SESGO){ //AQUI AUMENTE EL SESGO
        x = targetXY[0];
        y = targetXY[1];
    }
    else{
        x = ejeAleatorio();
        y = ejeAleatorio();
    }

    //Obtenemos el nodo mas cercano
    struct nodoGrafoD *masCercano = obtenerMasCercano(x, y);
    if(!masCercano){
        return NULL;
    }

    //Calculamos las coordenadas del nuevo nodo y lo generamos.
    float nuevoPunto[2];
    float actuales[] = {((struct nodoAgente*)masCercano->data)->x, ((struct nodoAgente*)masCercano->data)->y};
    float aleatorios[] = {x, y}; 
    nuevasCoordenadas(nuevoPunto, actuales, aleatorios);
    struct nodoAgente *nA = nuevoNodoAgente(nuevoPunto, DELTA_AGENTE, DELTA_AGENTE);
    if(!nA){
        return NULL;
    }

    //Verificamos si no esta en colision
    if(enColisionAgente(listaObst, nA)){
        free(nA);
        return NULL;
    }

    //Insertamos el nuevo nodo en la lista de nodosExistentes y en la lista de hijos de su nodo padre
    struct nodoGrafoD *nN = nuevoNodoGrafo(nA, masCercano);
    if(!nN){
        free(nA);
        return NULL;
    }

    if(!insertarNodoLista1D(&masCercano->lista, nN)){ 
        free(nA);
        free(nN);
        return NULL; //No se pudo insertar en nuevo nodoGrafoD es la lista del padre.
    }

    if(!insertarNodoLista1D(&nodosExistentes, nN)){
        free(nA);
        free(nN);
        //Tambien deberia eliminar el nodoLista con el puntero al nodoGrafo que ahora estaria desrreferenciado.
        return NULL; //No se pudo insertar el nodoGrafoD en la lista de nodos existentes.
    }

    return nN;
}

//Función para obtener el nodo mas cercano al punto aleatorio, utiliza distancia euclidiana.
struct nodoGrafoD* obtenerMasCercano(float x, float y){
    if(!nodosExistentes){
        printf("la lista nodosExistentes es NULL\n");
        return NULL;
    }
    struct nodoLista1D *aux = nodosExistentes;
    struct nodoGrafoD *cercano = (struct nodoGrafoD*)nodosExistentes->data;
    float dist_min = distanciaEuclidiana((struct nodoAgente*)cercano->data, x, y);
    
    while(aux){
        struct nodoGrafoD *nodo = (struct nodoGrafoD*)aux->data;
        float dist = distanciaEuclidiana((struct nodoAgente*)nodo->data, x, y);
        
        if(dist < dist_min){
            dist_min = dist;
            cercano = nodo;
        }
        aux = aux->next;
    }
    return cercano;
}

//Función para obtener las coordenadas del nuevo nodo.
void nuevasCoordenadas(float *nuevas, float *actual, float *random){
    float vect[] = {random[0] - actual[0], random[1] - actual[1]}; //Vector entre el nodo actual y el punto aleatorio.
    float magnitud = sqrt(pow(vect[0], 2) + pow(vect[1], 2)); //Magnitud del vector
    float unitario[] = {vect[0]/magnitud, vect[1]/magnitud}; //Vector unitario
    nuevas[0] = actual[0] + unitario[0] * PASO; //Coordenadas del nuevo punto
    nuevas[1] = actual[1] + unitario[1] * PASO;
}

//ALGORITMOS GRID - BÚSQUEDA EN ENTORNO DISCRETO

//Búsqueda primero en profundidad, se va por las ramas hasta llegar a NULL. 
//Si recorrieramos el arbol, seria preorden.
struct nodoLista2D* bpp(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst){
    //Primero debemos crear la discretización del espacio.
    struct matrizHash *mapa = crearMatrizCoords(SEGMENTOS_GRID, TAM_ESCENARIO);
    struct nodoLista2D *trayectoria = NULL; //Trayectoria del agente
    struct nodoLista1D *pilaEstados = NULL; //Para ir accediendo a los estados generados expandidos (LIFO)
    frames_agente = 0;

    if( enRango( (struct nodoAgente*)nodoInicial->data, targetXY ) ){ //Valida si el nodo inicial es solucion
        if(insertarNodoLista2D(&trayectoria, nodoInicial)){
            frames_agente++;
            liberarMatrizCoords(mapa);
            return trayectoria;
        }
        return NULL;
    }
    
    expandirGrid(nodoInicial, mapa, &pilaEstados, listaObst, targetXY, 0,  0);
    while(pilaEstados){
        struct nodoLista1D *act= popNodoLista1D(&pilaEstados); //Pop a la pila de estados
        if(!act){
            continue; //Ya no hay nodos en la pila de estados
        }
        struct nodoGrafoD *nodo = (struct nodoGrafoD*)act->data;

        if( enRango( (struct nodoAgente*)nodo->data, targetXY ) ){ //Si estamos lo suficientemente cerca, iniciamos backtraking
            //Hacemos el backtraking
            insertarNodoLista2D(&trayectoria, nodo);
            frames_agente++;
            struct nodoGrafoD *padre = nodo->padre;
            while(padre){
                insertarNodoLista2D(&trayectoria, padre);
                frames_agente++;
                padre = padre->padre;
            }
            liberarMatrizCoords(mapa);
            return trayectoria;
        }

        expandirGrid(nodo, mapa, &pilaEstados, listaObst, targetXY, 0, 0); //Expandimos (generamos mas estados)
    }

    printf("BPP no pudo generar mas estados!!!\n");
    liberarMatrizCoords(mapa);
    return NULL;
}

//Genera los estados en una cola de atención, se atienden primero a los estados o nodos con mejor metrica (heuristica)
struct nodoLista2D* greedy(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst){
    //Primero debemos crear la discretización del espacio.
    struct matrizHash *mapa = crearMatrizCoords(SEGMENTOS_GRID, TAM_ESCENARIO);
    struct nodoLista2D *trayectoria = NULL; //Trayectoria del agente
    struct nodoLista1D *colaEstados = NULL; //Para ir accediendo a los estados generados expandidos (LIFO)
    //Obtenemos la heuristica del primer nodo
    struct Greedy *g = (struct Greedy*)tipoHeuristica(0);
    g->h = distanciaEuclidiana((struct nodoAgente*)nodoInicial->data, targetXY[0], targetXY[1]);
    nodoInicial->cost = g;
    frames_agente = 0;

    if( enRango( (struct nodoAgente*)nodoInicial->data, targetXY ) ){ //Valida si el nodo inicial es solucion
        if(insertarNodoLista2D(&trayectoria, nodoInicial)){
            frames_agente++;
            liberarMatrizCoords(mapa);
            return trayectoria;
        }
        return NULL;
    }
    
    expandirGrid(nodoInicial, mapa, &colaEstados, listaObst, targetXY, 1 , 0);
    while(colaEstados){
        struct nodoLista1D *act= popNodoLista1D(&colaEstados); //Pop a la pila de estados
        if(!act){
            continue; //Ya no hay nodos en la pila de estados
        }
        struct nodoGrafoD *nodo = (struct nodoGrafoD*)act->data;

        if( enRango( (struct nodoAgente*)nodo->data, targetXY ) ){ //Si estamos lo suficientemente cerca, iniciamos backtraking
            //Hacemos el backtraking
            insertarNodoLista2D(&trayectoria, nodo);
            frames_agente++;
            struct nodoGrafoD *padre = nodo->padre;
            while(padre){
                insertarNodoLista2D(&trayectoria, padre);
                frames_agente++;
                padre = padre->padre;
            }
            liberarMatrizCoords(mapa);
            return trayectoria;
        }

        expandirGrid(nodo, mapa, &colaEstados, listaObst, targetXY, 1, 0); //Expandimos (generamos mas estados)
    }

    printf("Greedy no pudo generar mas estados!!!\n");
    liberarMatrizCoords(mapa);
    return NULL;
}

//Genera los estados en una cola de atención, atiende según la metria del estado o nodo (costo + heuristica)
struct nodoLista2D* aEstrella(struct nodoGrafoD *nodoInicial, float *targetXY, struct nodoLista1D *listaObst){
    //Primero debemos crear la discretización del espacio.
    struct matrizHash *mapa = crearMatrizCoords(SEGMENTOS_GRID, TAM_ESCENARIO);
    struct nodoLista2D *trayectoria = NULL; //Trayectoria del agente
    struct nodoLista1D *colaEstados = NULL; //Para ir accediendo a los estados generados expandidos (LIFO)
    //Obtenemos la heuristica y costo del primer nodo
    struct AStar *ae = (struct AStar*)tipoHeuristica(1);
    ae->h = distanciaEuclidiana((struct nodoAgente*)nodoInicial->data, targetXY[0], targetXY[1]);
    ae->f = 0;
    nodoInicial->cost = ae;
    frames_agente = 0;

    if( enRango( (struct nodoAgente*)nodoInicial->data, targetXY ) ){ //Valida si el nodo inicial es solucion
        if(insertarNodoLista2D(&trayectoria, nodoInicial)){
            frames_agente++;
            liberarMatrizCoords(mapa);
            return trayectoria;
        }
        return NULL;
    }
    
    expandirGrid(nodoInicial, mapa, &colaEstados, listaObst, targetXY, 1, 1);
    while(colaEstados){
        struct nodoLista1D *act= popNodoLista1D(&colaEstados); //Pop a la pila de estados
        if(!act){
            continue; //Ya no hay nodos en la pila de estados
        }
        struct nodoGrafoD *nodo = (struct nodoGrafoD*)act->data;

        if( enRango( (struct nodoAgente*)nodo->data, targetXY ) ){ //Si estamos lo suficientemente cerca, iniciamos backtraking
            //Hacemos el backtraking
            insertarNodoLista2D(&trayectoria, nodo);
            frames_agente++;
            struct nodoGrafoD *padre = nodo->padre;
            while(padre){
                insertarNodoLista2D(&trayectoria, padre);
                frames_agente++;
                padre = padre->padre;
            }
            liberarMatrizCoords(mapa);
            return trayectoria;
        }

        expandirGrid(nodo, mapa, &colaEstados, listaObst, targetXY, 1, 1); //Expandimos (generamos mas estados)
    }

    printf("A* no pudo generar mas estados!!!\n");
    liberarMatrizCoords(mapa);
    return NULL;
}

//Nos ayuda a obtener un type = 0 : (struct Greedy*) o type = 1 : (struct AStar*). 
//En caso de no ingresar un parametro valido, se retorna (NULL).
void* tipoHeuristica(int type){
    void* th = NULL; 
    
    switch(type){
        case 0:
            th = malloc(sizeof(struct Greedy));
            if(th){
                ((struct Greedy*)th)->h = 0;
            }

            break;
        case 1:
            th = malloc(sizeof(struct AStar));
            if(th){
                ((struct AStar*)th)->h = 0;
                ((struct AStar*)th)->f = 0;
            }

            break;
    }
    return th;
}

//Expande a nuevos estados dado el estado actual, determina el algoritmo para generar una pila (bpp) o una cola de atencio (greedy - A*).
//Esta implementación resuelve la redundancia al no dejar que nodos existentes en la pila - cola de atención vuelvan a ser insertados.
int expandirGrid(struct nodoGrafoD *nodo, struct matrizHash *hash, struct nodoLista1D **listaEstados, struct nodoLista1D *listaObst, float *targetXY, int h, int f){
    float x = ((struct nodoAgente*)nodo->data)->x; //Coordenadas del nodo actual
    float y = ((struct nodoAgente*)nodo->data)->y;
    int cont = 0; //Para contar los nodos expandidos.

    for(int i = -1; i<=2; i+=2){
        float deltaX[] = {x + hash->delta*i, y};
        float deltaY[] = {x, y + hash->delta*i};
        struct nodoAgente *dX = nuevoNodoAgente(deltaX, DELTA_AGENTE, DELTA_AGENTE);
        struct nodoAgente *dY = nuevoNodoAgente(deltaY, DELTA_AGENTE, DELTA_AGENTE);

        if(enEscenario(dX) && (!enColisionAgente(listaObst, dX)) && (!validarCoordenada(hash, deltaX[0], deltaX[1]))){
            struct nodoGrafoD *nodoX = nuevoNodoGrafo(dX, nodo);
            //Agregamos void* cost a nodoGrafoD
            if(h){
                if(f){
                    struct AStar *ae = (struct AStar*)tipoHeuristica(1);
                    ae->h = distanciaEuclidiana(dX, targetXY[0], targetXY[1]);
                    ae->f = ((struct AStar*)nodo->cost)->f + 1;
                    ae->sum = ae->h + ae->f;
                    nodoX->cost = ae;
                }
                else{
                    struct Greedy *g = (struct Greedy*)tipoHeuristica(0);
                    g->h = distanciaEuclidiana(dX, targetXY[0], targetXY[1]);
                    nodoX->cost = g;
                }
                insertarNodoLista1D(&nodo->lista, nodoX); //Añadimos el nuevo nodo como hijo del actual
                agregarVisitado(hash, dX); //Lo marcamos como visitado
                if(f){
                    insertarListaEstados(listaEstados, nodoX, 1); //Se inserta de manera ordenada.
                }
                else{
                    insertarListaEstados(listaEstados, nodoX, 0); //Se inserta de manera ordenada.
                }
            }
            else{
                insertarNodoLista1D(&nodo->lista, nodoX); //Añadimos el nuevo nodo como hijo del actual
                insertarNodoLista1D(listaEstados, nodoX); //Lo agregamos al la lista de estados
                agregarVisitado(hash, dX); //Lo marcamos como visitado
            }
            cont++;
        }
        else{
            free(dX);
        }

        if(enEscenario(dY) && (!enColisionAgente(listaObst, dY)) && (!validarCoordenada(hash, deltaY[0], deltaY[1]))){
            struct nodoGrafoD *nodoY = nuevoNodoGrafo(dY, nodo);
            //Agregamos void* cost a nodoGrafoD
            if(h){
                if(f){
                    struct AStar *ae = (struct AStar*)tipoHeuristica(1);
                    ae->h = distanciaEuclidiana(dY, targetXY[0], targetXY[1]);
                    ae->f = ((struct AStar*)nodo->cost)->f + 1;
                    ae->sum = ae->h + ae->f;
                    nodoY->cost = ae;
                }
                else{
                    struct Greedy *g = (struct Greedy*)tipoHeuristica(0);
                    g->h = distanciaEuclidiana(dY, targetXY[0], targetXY[1]);
                    nodoY->cost = g;
                }
                insertarNodoLista1D(&nodo->lista, nodoY); //Añadimos el nuevo nodo como hijo del actual
                agregarVisitado(hash, dY); //Lo marcamos como visitado
                if(f){
                    insertarListaEstados(listaEstados, nodoY, 1); //Se inserta de manera ordenada
                }
                else{
                    insertarListaEstados(listaEstados, nodoY, 0); //Se inserta de manera ordenada
                }
            }
            else{
                insertarNodoLista1D(&nodo->lista, nodoY); //Añadimos el nuevo nodo como hijo del actual
                insertarNodoLista1D(listaEstados, nodoY); //Lo agregamos a la lista de estados
                agregarVisitado(hash, dY); //Lo marcamos como visitado
            }
            cont++;
        }
        else{
            free(dY);
        }
    }
    return cont;
}

//Función para insertar el nodoGrafoD en la lista de estados de manera ordenada (de menor a mayor).
void insertarListaEstados(struct nodoLista1D **listaEstados, struct nodoGrafoD *nodo, int lambda){
    if(!nodo){
        return;
    }

    switch(lambda){ //Voy a Hardcodear (no se utilizar callbacks en C).
        case 0:
            ordenarGreedy(listaEstados, nodo);
            break;
        case 1:
            ordenarAStar(listaEstados, nodo);
            break;
        default:
            printf("Lambda invalido!!!\n");
    }
}

//Función especifica para ordenar la lista de estados, para la implementación de Greedy.
int ordenarGreedy(struct nodoLista1D **lista, struct nodoGrafoD *nodo){
    if(!(*lista)){ //El primero en ser insertado
        insertarNodoLista1D(lista, nodo);
        return 1;
    }

    struct nodoLista1D *act = *lista;
    if( ((struct Greedy*)nodo->cost)->h < ((struct Greedy*)((struct nodoGrafoD*)act->data)->cost)->h ){
        //Ahora el nodo es el nuevo mejor
        insertarNodoLista1D(lista, nodo);
        return 1;
    }

    struct nodoLista1D *nuevo = nuevoNodoLista1D(nodo);

    while(act->next){ //Es un nodo intermedio
        if( ((struct Greedy*)nodo->cost)->h < ((struct Greedy*)((struct nodoGrafoD*)(act->next)->data)->cost)->h ){
            nuevo->next = act->next;
            act->next = nuevo;
            return 1;
        }
        act = act->next;
    }

    act->next = nuevo; //El nodo es el ultimo
    return 1;
}

//Función especifica para ordenar la lista de estados, para la implementación de A*
int ordenarAStar(struct nodoLista1D **lista, struct nodoGrafoD *nodo){
    if(!(*lista)){ //El primero en ser insertado
        insertarNodoLista1D(lista, nodo);
        return 1;
    }

    struct nodoLista1D *act = *lista;
    if( ((struct AStar*)nodo->cost)->sum < ((struct AStar*)((struct nodoGrafoD*)act->data)->cost)->sum ){
        //Ahora el nodo es el nuevo mejor
        insertarNodoLista1D(lista, nodo);
        return 1;
    }

    struct nodoLista1D *nuevo = nuevoNodoLista1D(nodo);

    while(act->next){ //Es un nodo intermedio
        if( ((struct AStar*)nodo->cost)->sum < ((struct AStar*)((struct nodoGrafoD*)(act->next)->data)->cost)->sum ){
            nuevo->next = act->next;
            act->next = nuevo;
            return 1;
        }
        act = act->next;
    }

    act->next = nuevo; //El nodo es el ultimo
    return 1;
}

//Función para determinar si el objetivo esta siendo tocado por el agente.
int enRango(struct nodoAgente *agente, float *targetXY){
    float xMin = (agente->x) - (agente->deltaX/2);
    float xMax = (agente->x) + (agente->deltaX/2);
    float yMin = (agente->y) - (agente->deltaY/2);
    float yMax = (agente->y) + (agente->deltaY/2);

    int compX = (xMin <= targetXY[0] && targetXY[0] <= xMax);
    int compY = (yMin <= targetXY[1] && targetXY[1] <= yMax);
    
    if(compX && compY){
        return 1;
    }
    return 0;
}

//Función para determinar si un agente se encuentra dentro del escenario.
int enEscenario(struct nodoAgente *agente){
    //TAM_ESCENARIO
    float xMin = (agente->x) - (agente->deltaX/2);
    float xMax = (agente->x) + (agente->deltaX/2);
    float yMin = (agente->y) - (agente->deltaY/2);
    float yMax = (agente->y) + (agente->deltaY/2);

    int compX = (xMax <= TAM_ESCENARIO && xMin >= -TAM_ESCENARIO);
    int compY = (yMax <= TAM_ESCENARIO && yMin >= -TAM_ESCENARIO);

    if(compX && compY){
        return 1;
    }
    return 0;
}

//Función para agregar una coordenada o indice a la matriz de visitados, retorna 1 si se agrego con exito, 0 si ya existe en la matriz o si no se pudo agregar.
int agregarVisitado(struct matrizHash *hash, struct nodoAgente *agente){
    if(validarCoordenada(hash, agente->x, agente->y)){
        return 0; //Ya existe en la matriz
    }

    return setearCoordenada(hash, agente->x, agente->y, 1);
}

//Función para obtener la distancia euclidiana entre dos puntos.
float distanciaEuclidiana(struct nodoAgente *agente, float x, float y){
    return sqrt(pow(x - agente->x, 2) + pow(y - agente->y, 2));
}

//Esta función regresa el estado del agente dado el frame, es decir, en el frame 'x' el agente se encontrara en 'x' y en 'y'.
//No pude realizar un hash debido a que en el backtraking los indice se invierten.
struct nodoAgente* frameEspecificoAgente(struct nodoLista1D *agente, int frameAct){
    if(!agente){
        printf("struct nodoLista1D *agente es NULL!!!\n"); //Debuggeando :)
    }
    
    struct nodoAgente *raiz = (struct nodoAgente*)((struct nodoGrafoD*)agente->data)->data;
    struct nodoLista2D *aux = raiz->solucion;

    int i = 0;
    while(i < frameAct){
        if(!aux){
            return NULL;
        }
        aux = aux->next;
        i++; //AQUI ESTA EL ERROR: FALTABA ITERAR 'i', LA CONDICION DEL BUCLE SIMPRE SE CUMPLIA Y LA LISTA SE ACABABA.
    }
    
    return (struct nodoAgente*)((struct nodoGrafoD*)aux->data)->data;
}

//Esta función crea un número aleatorio entre los limites del escenario.
int ejeAleatorio(){
    int n = numeroAleatorio(0, TAM_ESCENARIO * 2); //Entre 0 y 2000
    return n - TAM_ESCENARIO; //[0, 2000] - 1000 = [-1000, 1000]
} 

#endif