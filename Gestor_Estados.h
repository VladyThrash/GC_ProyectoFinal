//Aqui vamos a desarrollar la cola de estados (momentos de la animacion), y obtener el orden que alimentamos
//OpenGL. 

//1.- Cola de dibujado: Es la que le dice a OpenGl que va a dibujar en cada momento. Cada nodo es un momento de dibujado,
//                      es decir, le damos a dibujar a OpenGl todo lo que el nodo tiene.

//2.- Cada nodo de la cola de dibujado se puede ver como una raiz, a partir de esa raiz tenemos jerarquias,
//    donde cada nivel representa el orden en que le vamos a indicar a OpenGL que dibuje. Entes estaticos, 
//    entes con calculos simples y agentes complejos.

//3.- Los niveles jerarquicos que lo requieran se recalculan en tiempo, es decir, entre nodos de la cola de dibujado.

//4.- El recalculado es con respecto al estado actual, debemos de ser capaces de propagar hacia el pasado y hacia el
//    futuro el estado de los entes. El fin de esto es poder tomar un nodo 'x' de la cola de dibujado, modificar los entes
//    de la jerarquia, y que estos cambios se vean reflejados en todos los entes de todos los nodos de la cola.


#ifndef Gestor_Estados
#define Gestor_Estados
#include<stdlib.h>
#include "Estructuras_Genericas.h"

//STRUCTS

//Struct para la cola de dibujado.
struct nodoDibujo{
    int numDibujo; //El numero de dibujo o estado, debe de ser autoincrementable con un limite (la animacion no es infinita).
    struct nodoLista1D *entesEstaticos;
    struct nodoLista1D *entesDinamicos;
    struct nodoLista1D *agentes;
    //Aqui podria ir el callback para la funcion de actulizacion (entes dinamicos y agentes).
};

//FUNCIONES PARA EL MANEJO DE LA COLA DE DIBUJO

//Crear en memoria nodoDibujo, que contiene la informacion la listas de agentes y numero de dibujo de la cola.
struct nodoDibujo* nuevoNodoDibujo(int numD){
    struct nodoDibujo *newNodo = NULL;
    newNodo = (struct nodoDibujo*)malloc(sizeof(struct nodoDibujo));
    if(!newNodo){
        return NULL;
    }
    newNodo->numDibujo = numD;
    newNodo->entesEstaticos = NULL;
    newNodo->entesDinamicos = NULL;
    newNodo->agentes = NULL;
    return newNodo;
}

//Crear el primer nodo de la cola de dibujo.
//Para ello se debe de tener las listas de entes creadas (estaticos, dinamicos y agentes).
struct colaXD* crearColaDibujo(struct nodoLista1D *e, struct nodoLista1D *d, struct nodoLista1D *a){
    struct colaXD *nodoInicial = NULL;
    nodoInicial = nuevaColaXD(2); //Cola doblemente enlazada (podemos ir la pasado y al futuro).
    if(!nodoInicial){
        return NULL; //No se pudo crear la cola (Contenedor).
    }

    //Creamos el nodo actual
    struct nodoDibujo *dAct = nuevoNodoDibujo(1); //Es el primer dibujo.
    if(!dAct){
        return NULL; //No se pudo crear el primer nodo dibujo (void *data).
    }
    dAct->entesEstaticos = e;
    dAct->entesDinamicos = d;
    dAct->agentes = a;
    
    //Insertamos en la cola nuestra struct con informacion del nodo.
    if(insertarEnColaXD(&nodoInicial, dAct)){
        return nodoInicial;
    }

    return NULL; //No se pudo insertar el primer nodo de la cola.
}

//Función de creacion de los nodos de la cola de animacion.
//Se debe de incializar la cola manualmente con la funcion especifica (Debe de existir el primer nodo).
int addColaDibujo(struct colaXD *cont){
    if(!cont){
        return 0; //No se creo la cola o hubo un error al crearla.
    }

    //Primero (void *data: nodoDibujo)
    struct nodoDibujo *dAnterior = (struct nodoDibujo*)(((struct nodoLista2D*)cont->end)->data); //Qué feo
    struct nodoDibujo *nDibujo = pasarEntes(dAnterior); //Pasamos la lista de entes de un nodo a otro.
    if(!nDibujo){
        return 0; //No se pudo crear el nodoDibujo (void *data del nodo).
    }
    nDibujo->numDibujo = dAnterior->numDibujo + 1; //Actualizamos el numero de dibujo.
    if(insertarEnColaXD(&cont, nDibujo)){
        return 1;
    }
    return 0;
}

//Función para pasar la lista de entes a otro nodo nuevo. 
//Crea un nuevo nodoDibujo, toma la lista de uno ya existente y añade los punteros de las listas al nuevo nodo.
struct nodoDibujo* pasarEntes(struct nodoDibujo *nE){
    struct nodoDibujo *nN = nuevoNodoDibujo(0); //Definiremos el número despues.
    if(!nN){
        return NULL;
    }
    nN->entesEstaticos = nE->entesEstaticos; //Lista de entes estaticos
    nN->entesDinamicos = nE->entesDinamicos; //Lista entes dinamicos
    nN->agentes = nE->agentes; //Lista agentes
    return nN;
}

#endif