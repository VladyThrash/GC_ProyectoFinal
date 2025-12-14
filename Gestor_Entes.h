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
#define TAM_ESCENARIO 170 //Aqui podrias cambiarlo. NOTA: HE ESTATO JUGANDO CON ESTE TAMAÑO
#define ALT_MAX_EDI 80 //Altura maxima y minima de los edificios
#define ALT_MIN_EDI 50
#define DELTA_MAX_EDI 35 //Ancho maximo y minimo de los edificios
#define DELTA_MIN_EDI 20
#define ALT_MAX_CAS 35 //Altura maxima y minima de las casas
#define ALT_MIN_CAS 15
#define DELTA_MAX_CAS 30 //Ancho maximo y minimo de las casas
#define DELTA_MIN_CAS 15
#define MAX_ITER_BRUTO 10000 //Maximo número de iteraciones para poder generar un ente en el mapa

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

//PROTOTIPOS
int numeroDentroElEscenario();
void nuevasCoordsEnte(struct enteEstatico *ente);
struct casa* nuevaCasa();
struct edificio* nuevoEdificio();
struct enteEstatico* nuevoEnteEstatico(void *data, int tipo);
struct enteEstatico* obtenerEnteAleatorio();
int agregarNuevoEnteEstatico(struct nodoLista1D **lista, float *start, float *target);
int enColision(struct enteEstatico *ente1, struct enteEstatico *ente2);
int enColisionFlags(struct enteEstatico *ente, float *flag);
struct nodoLista1D* crearListaEstaticos(int n, float *start, float *target);

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
    ne = (struct enteEstatico*)malloc(sizeof(struct enteEstatico));
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

//CORRECIÓN: AQUI LISTA DEBE PASARSE COMO PUNTERO DOBLE!!!
//Esta función añade un ente estatico a una lista, en una posicion aleatoria.
int agregarNuevoEnteEstatico(struct nodoLista1D **lista, float *start, float *target){
    int cicloActivo = 1; //Nos permite recalcular posiciones hasta hallar la posicion correcta.
    struct enteEstatico *ente = obtenerEnteAleatorio();
    if(!(*lista)){ //La lista esta vacia, no importa si el ente esta en colision.
        return insertarNodoLista1D(lista, ente);
    }

    //Aqui debemos de checar si no entra en colision con los demas entes de la lista.
    //Vamos a utilizar fuerza bruta (generar posiciones aleatorias hasta que una encajé).
    int i = 0; //Contar el número de iteraciones para maximas (si no se cicla infinitamente).
    struct nodoLista1D *aux = *lista;
    while(cicloActivo){
        while(aux){
            if(enColision((struct enteEstatico*)aux->data, ente) || enColisionFlags((struct enteEstatico*)aux->data, start) || enColisionFlags((struct enteEstatico*)aux->data, target)){ //Los entes entran en colision.
                nuevasCoordsEnte(ente);
                aux = *lista;
                break;
            }
            aux = aux->next;
        }
        if(!aux){
            cicloActivo = 0;
        }
        if(i >= MAX_ITER_BRUTO){ //Para no estar buscar infinitamente (el peor de los casos).
            return 0;
        }
        i++;
    }

    return insertarNodoLista1D(lista, ente);
}

//Esta función detecta si dos entes entraron colisión.
int enColision(struct enteEstatico *ente1, struct enteEstatico *ente2){
    //Estraron en colisión si los intervalos se atraviesan.
    float xMin1 = (ente1->x) - (ente1->deltaX/2); //Intervalos de ente1
    float xMax1 = (ente1->x) + (ente1->deltaX/2);
    float yMin1 = (ente1->y) - (ente1->deltaY/2);
    float yMax1 = (ente1->y) + (ente1->deltaY/2);

    float xMin2 = (ente2->x) - (ente2->deltaX/2); //Intervalos de ente2
    float xMax2 = (ente2->x) + (ente2->deltaX/2);
    float yMin2 = (ente2->y) - (ente2->deltaY/2);
    float yMax2 = (ente2->y) + (ente2->deltaY/2);

    int colX = (xMin1 <= xMax2 && xMin2 <= xMax1); //Dos intervalos A y B colisionan si:
    int colY = (yMin1 <= yMax2 && yMin2 <= yMax1); //Amin <= Bmax y Bmin <= Amax
    if(colX && colY){
        return 1;
    }
    return 0;
}

//Esta función detecta si un ente entra en colisión con coordenadas especificas importantes, no deben generase entes estaticos
//sobre los puntos de generación y objetivo del agente.
int enColisionFlags(struct enteEstatico *ente1, float *flag){
    //Entra en colisión si los intervalos se atraviesan.
    float xMin1 = (ente1->x) - (ente1->deltaX/2); //Intervalos de ente1
    float xMax1 = (ente1->x) + (ente1->deltaX/2);
    float yMin1 = (ente1->y) - (ente1->deltaY/2);
    float yMax1 = (ente1->y) + (ente1->deltaY/2);

    float xMin2 = flag[0] - 4; //Intervalos de las banderas (hardcodeamos y colocamos directamente el delta del agente).
    float xMax2 = flag[0] + 4;
    float yMin2 = flag[1] - 4;
    float yMax2 = flag[1] + 4;

    int colX = (xMin1 <= xMax2 && xMin2 <= xMax1); //Dos intervalos A y B colisionan si:
    int colY = (yMin1 <= yMax2 && yMin2 <= yMax1); //Amin <= Bmax y Bmin <= Amax
    if(colX && colY){
        return 1;
    }
    return 0;
} 

//Esta función crea una lista con 'n' entes estaticos repartidos de manera aleatoria.
struct nodoLista1D* crearListaEstaticos(int n, float *start, float *target){
    //if(n >= 0){ <-- POR ESTA ALIMAÑA NO PODIA GENERAR OBSTACULOS
    if(n <= 0){
        return NULL; //No se pueden crear entes de manera negativa.
    }

    int state, count = 0; //Obtener el estado en lista y el número existente en la misma.
    int expected = n; //Número de entes esperados.
    struct nodoLista1D *lista = NULL;
    while(n){ //Cargamos 'n' entes en la lista.
        state = agregarNuevoEnteEstatico(&lista, start, target);  //<--- Aqui debemos recibir start y target.
        if(state){
            count++;
        }
        n--;
    }

    printf("Agentes generados: %d/%d \n", count, expected);
    return lista;
}

#endif