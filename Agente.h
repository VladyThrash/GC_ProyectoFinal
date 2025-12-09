//Hola vlady, aqui implementaremos el algortimo de busqueda del agente, asi como la expansión de la cola de dibujado, ya que la 
//animación no termina hasta que el agente llega al objetivo.
//De momento implemetaremos RRT, posteriormente se embeberan los demás algoritmos de búsqueda.

#ifndef Gestor_Entes
#define Gestor_Entes
#include<stdlib.h>
#include<time.h> //Para generar números aleatorios
#include<math.h>
#include "Estructuras_Genericas.h"
#include "Gestor_Entes.h"

//CONSTANTES Y GLOBALES
#define DELTA_AGENTE 5 //El tamaño
#define PASO 0.05 //El paso delta del algortimo RRT
#define MAX_ITER 10000
#define TAM_ESCENARIO 1000
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

struct agente{ //Informacion de dibujado del agente.
    void *data;
};

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
    
    float xMin2 = (agente->x) - (agente->deltaX); //Intervalos de agente
    float xMax2 = (agente->x) + (agente->deltaX);
    float yMin2 = (agente->y) - (agente->deltaY);
    float yMax2 = (agente->y) + (agente->deltaY);

    while(aux){
        struct enteEstatico *ente = (struct enteEstatico*)aux->data;
        
        float xMin1 = (ente->x) - (ente->deltaX); //Intervalos de ente
        float xMax1 = (ente->x) + (ente->deltaX);
        float yMin1 = (ente->y) - (ente->deltaY);
        float yMax1 = (ente->y) + (ente->deltaY);

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
struct nodoLista1D* agregarAgente(float *startXY, float *targetXY, struct nodoLista1D *listaObst){
    struct nodoAgente *agente = nuevoNodoAgente(startXY, DELTA_AGENTE, DELTA_AGENTE);
    if(!agente){
        return NULL; //Error al generar el nodo agente.
    }

    struct nodoGrafoD* nodoInicial = nuevoNodoGrafo(agente, NULL);
    if(!nodoInicial){
        return NULL; //Error al generar el nodo grafo.
    }

    ((struct nodoAgente*)nodoInicial->data)->solucion = rrt(nodoInicial, targetXY, listaObst);
    return nuevoNodoLista1D(nodoInicial);
}

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
    return NULL; //Se acabaron las iteraciones.
}

//Esta función expande el estado, aqui se genera el nodo mas cercano al punto aleatorio.
struct nodoGrafoD* expandirEstados(float *targetXY, struct nodoLista1D *listaObst){
    //Primero definimos el punto aleatorio sobre el escenario, pero vamos a incluir un sesgo para que el algoritmo converja mas rapido.
    int sesgo = numeroAleatorio(1, 10);
    float x, y;
    if(sesgo <= 2){
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

//Función para obtener la distancia euclidiana entre dos puntos.
float distanciaEuclidiana(struct nodoAgente *agente, float x, float y){
    return sqrt(pow(x - agente->x, 2) + pow(y - agente->y, 2));
}

//Función para obtener las coordenadas del nuevo nodo.
void nuevasCoordenadas(float *nuevas, float *actual, float *random){
    float vect[] = {random[0] - actual[0], random[1] - actual[1]}; //Vector entre el nodo actual y el punto aleatorio.
    float magnitud = sqrt(pow(vect[0], 2) + pow(vect[1], 2)); //Magnitud del vector
    float unitario[] = {vect[0]/magnitud, vect[1]/magnitud}; //Vector unitario
    nuevas[0] = actual[0] + unitario[0] * PASO; //Coordenadas del nuevo punto
    nuevas[1] = actual[1] + unitario[1] * PASO;
}

//Función para determinar si el objetivo esta siendo tocado por el agente.
int enRango(struct nodoAgente *agente, float *targetXY){
    float xMin = (agente->x) - (agente->deltaX);
    float xMax = (agente->x) + (agente->deltaX);
    float yMin = (agente->y) - (agente->deltaY);
    float yMax = (agente->y) + (agente->deltaY);

    int compX = (xMin <= targetXY[0] && targetXY[0] <= xMax);
    int compY = (yMin <= targetXY[1] && targetXY[1] <= yMax);
    
    if(compX && compY){
        return 1;
    }
    return 0;
}

//Esta función crea un número aleatorio entre los limites del escenario.
int ejeAleatorio(){
    int n = numeroAleatorio(0, TAM_ESCENARIO * 2); //Entre 0 y 2000
    return n - TAM_ESCENARIO; //[0, 2000] - 1000 = [-1000, 1000]
} 

#endif