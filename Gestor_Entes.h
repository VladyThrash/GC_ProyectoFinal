//Hola vlady, aqui definiremos las structs y funciones especificas para el manejo de los entes.
//Esta libreria solo se encarga de los contenedores y como se utilizan. En el .c principal debemos de definir
//estos contenedores, ya que desde una libreria no se puden hacer asignaciones (No se como explicar mejor esto).
//Si, pero desde allá invocan lo de acá (No se explicar).
//Los .h son manuales de cocina, dicen como condimentar, cuanto tiempo freir, que verduras seleccionar, etc.
//Los .c son los cheffs que siguen estos manuales, tambien pueden tener recetas propias (El que no entiende soy yo).

#ifndef Gestor_Entes
#define Gestor_Entes
#include<stdlib.h>
#include<time.h> //Para generar numeros aleatorios
#include "Estructuras_Genericas.h"

//CONSTANTES
#define NUM_ESTATICOS 2
#define TAM_ESCENARIO 1000 //Aqui podrias cambiarlo
#define ALT_MAX_EDI 80 //Altura maxima y minima de los edificios
#define ALT_MIN_EDI 50
#define DELTA_MAX_EDI 35 //Ancho maximo y minimo de los edificios
#define DELTA_MIN_EDI 20
#define ALT_MAX_CAS 35 //Altura maxima y minima de las casas
#define ALT_MIN_CAS 15
#define DELTA_MAX_CAS 30 //Ancho maximo y minimo de las casas
#define DELTA_MIN_CAS 15

//STRUCTS
struct enteEstatico{
    float x; //Posición en el plano XY, para una traza con Z constante.
    float y;

    float deltaX; //Que tanto crece en ese eje con respecto al centro.
    float deltaY;
    float altura; //Que tan alto es.

    int tipo; //Tipo de ente que es.
    void *data; //Puntero a la información de dibujado.
};

struct casa{
    float deltaX;
    float deltaY;
    float altura;
};

struct edificio{
    float deltaX;
    float deltaY;
    float altura;
};

//FUNCIONES

//Esta función crear y definir los atributos del ente casa.
struct casa* nuevaCasa(){
    struct casa * c = NULL;
    c = (struct casa*)malloc(sizeof(struct casa));
    if(!c){
        return NULL;
    }

    c->altura = numeroAleatorio(ALT_MIN_CAS, ALT_MAX_CAS);
    c->deltaX = numeroAleatorio(DELTA_MIN_CAS, DELTA_MAX_CAS);
    c->deltaY = numeroAleatorio(DELTA_MIN_CAS, DELTA_MAX_CAS);
    return c;
}

//Esta función crear y definir los atributos del ente edificio.
struct edificio* nuevoEdificio(){
    struct edificio * e = NULL;
    e = (struct edificio*)malloc(sizeof(struct edificio));
    if(!e){
        return NULL;
    }

    e->altura = numeroAleatorio(ALT_MIN_EDI, ALT_MAX_EDI);
    e->deltaX = numeroAleatorio(DELTA_MIN_EDI, DELTA_MAX_EDI);
    e->deltaY = numeroAleatorio(DELTA_MIN_EDI, DELTA_MAX_EDI);
    return e;
}

//Esta función crea un contenedor de ente estatico (enteEstatico).
struct enteEstatico* nuevoEnteEstatico(void *data, int tipo){
    if(!data){
        return NULL; //No se incializo el puntero void *data.
    }

    struct enteEstatico *ne = NULL;
    ne = (struct enteEstatico*)malloc(sizeof(enteEstatico));
    if(!ne){
        free(data); //Porque el struct del ente especifico debe de estar ya inicializado.
        return NULL;
    }

    ne->x = 0; //Aleatorio dentre del escenario!!!! <-- AQUI
    ne->y = 0;
    ne->tipo = tipo;
    ne->data = data;
    return ne;
}

//Esta función retorna un puntero generico a alguno de los tipos de entes estaticos que existen.
struct enteEstatico* obtenerEnteAleatorio(){
    int n = numeroAleatorio(1, NUM_ESTATICOS);
    struct enteEstatico *ne = NULL;
    
    switch(n){
        case 1:
            //struct casa
            struct casa *c = nuevaCasa();
            ne = nuevoEnteEstatico(c, 1);
            ne->deltaX = c->deltaX;
            ne->deltaY = c->deltaY;
            ne->altura = c->altura;
            break;
        
        case 2:
            //struct edificio
            struct edificio *e = nuevoEdificio();
            ne = nuevoEnteEstatico(e, 2);
            ne->deltaX = e->deltaX;
            ne->deltaY = e->deltaY;
            ne->altura = e->altura;
            break;
        
        default:
            return NULL; //Aunque esto no deberia pasar.
    }

    return ne;
}

//Esta función añade un ente estatico a una lista, en una posicion aleatoria.
int agregarNuevoEnteEstatico(struct nodoLista1D *lista){
    struct enteEstatico *ente = obtenerEnteAleatorio();
    if(!lista){ //La lista esta vacia, no importa si el ente esta en colision.
        return insertarNodoLista1D(&lista, ente);
    }

    //Aqui debemos de checar si no entra en colision con los demas entes de la lista

}

//Esta función crea una lista con 'n' entes estaticos repartidos de manera aleatoria.
struct nodoLista1D* crearListaEstaticos(int n){

}

#endif