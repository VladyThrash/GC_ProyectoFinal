//Hola vlady, aqui implementaremos el algortimo de busqueda del agente, asi como la expansión de la cola de dibujado, ya que la 
//animación no termina hasta que el agente llega al objetivo.
//De momento implemetaremos RRT, posteriormente se embeberan los demás algoritmos de búsqueda.

#ifndef Gestor_Entes
#define Gestor_Entes
#include<stdlib.h>
#include<time.h> //Para generar numeros aleatorios
#include "Estructuras_Genericas.h"
#include "Gestor_Entes.h"

//CONSTANTES

//STRUCTS

struct nodoAgente{ //Coordenadas y colisión del agente.
    float x;
    float y;
    float deltaX;
    float deltaY;
};

struct agente{ //Informacion de dibujado del agente.
    void *data;
};

//FUNCIONES

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

//numeroDentroElEscenario();
//Una lista simple con todos los nodos del grafo
//Se busca el mas cercano al punto aleatorio
//Se añade el nuevo nodo con un avance delta hacia el punto aleatorio (si no esta en colision)
//Si el nuevo esta en meta se regresa la ruta con backtraking
//Si el nuevo no es en meta se repite el proceso

#endif