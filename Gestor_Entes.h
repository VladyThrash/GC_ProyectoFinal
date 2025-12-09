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
    int conTecho;
};

struct edificio{
    float deltaX;
    float deltaY;
    float altura;
    int cilindrico;
};

//FUNCIONES

//Esta función crea un número aleatorio entre los limites del escenario.
int numeroDentroElEscenario(){
    int n = numeroAleatorio(0, TAM_ESCENARIO * 2); //Entre 0 y 2000
    return n - TAM_ESCENARIO; //[0, 2000] - 1000 = [-1000, 1000]
} 

//Esta función recalcula las coordenadas aleatorias de un ente (dentro de los limites del escenario).
void nuevasCoordsEnte(struct enteEstatico *ente){
    ente->x = numeroDentroElEscenario();
    ente->y = numeroDentroElEscenario();
}

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
    c->conTecho = numeroAleatorio(0,1);
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
    e->cilindrico = numeroAleatorio(0,1);
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

    ne->x = numeroDentroElEscenario(); //Aleatorio dentro de los limites del escenario!!!!
    ne->y = numeroDentroElEscenario();
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
    int cicloActivo = 1; //Nos permite recalcular posiciones hasta hallar la posicion correcta.
    struct enteEstatico *ente = obtenerEnteAleatorio();
    if(!lista){ //La lista esta vacia, no importa si el ente esta en colision.
        return insertarNodoLista1D(&lista, ente);
    }

    //Aqui debemos de checar si no entra en colision con los demas entes de la lista.
    //Vamos a utilizar fuerza bruta (generar posiciones aleatorias hasta que una encajé).
    struct nodoLista1D *aux = lista;
    while(cicloActivo){
        while(aux){
            if(enColision((struct enteEstatico*)aux->data, ente)){ //Los entes entran en colision.
                nuevasCoordsEnte(ente);
                aux = lista;
                break;
            }
            aux = aux->next;
        }
        if(!aux){
            cicloActivo = 0;
        }
    }

    return insertarNodoLista1D(&lista, ente);
}

//Esta función detecta si dos entes entraron colisión.
int enColision(struct enteEstatico *ente1, struct enteEstatico *ente2){
    //Estraron en colisión si los intervalos se atraviesan.
    float xMin1 = (ente1->x) - (ente1->deltaX); //Intervalos de ente1
    float xMax1 = (ente1->x) + (ente1->deltaX);
    float yMin1 = (ente1->y) - (ente1->deltaY);
    float yMax1 = (ente1->y) + (ente1->deltaY);

    float xMin2 = (ente2->x) - (ente2->deltaX); //Intervalos de ente2
    float xMax2 = (ente2->x) + (ente2->deltaX);
    float yMin2 = (ente2->y) - (ente2->deltaY);
    float yMax2 = (ente2->y) + (ente2->deltaY);

    int colX = (xMin1 <= xMax2 && xMin2 <= xMax1); //Dos intervalos A y B colisionan si:
    int colY = (yMin1 <= yMax2 && yMin2 <= yMax1); //Amin <= Bmax y Bmin <= Amax
    if(colX && colY){
        return 1;
    }
    return 0;
}

//Esta función crea una lista con 'n' entes estaticos repartidos de manera aleatoria.
struct nodoLista1D* crearListaEstaticos(int n){
    if(n >= 0){
        return NULL; //No se pueden crear entes de manera negativa.
    }

    struct nodoLista1D *lista = NULL;
    while(n){ //Cargamos 'n' entes en la lista.
        if(agregarNuevoEnteEstatico(lista)){
            n--;
        }
    }

    return lista;
}

#endif