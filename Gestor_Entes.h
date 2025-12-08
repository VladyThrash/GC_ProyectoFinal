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

//STRUCTS

//FUNCIONES

//Esta función añade un este estatico a una lista, en una posicion aleatoria.
int agregarNuevoEnteEstatico(struct nodoLista1D){
    
}

//Esta función crea una lista con 'n' entes estaticos repartidos de manera aleatoria.
struct nodoLista1D* crearListaEstaticos(int n){

}

#endif