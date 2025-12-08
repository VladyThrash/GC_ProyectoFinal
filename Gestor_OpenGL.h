//Basicamente maneja las funciones especificas de dibujado (segun ente o estado).

#ifndef Gestor_OpenGL
#define Gestor_OpenGL
#include<stdlib.h>
#include<GL/freeglut.h>
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"

//STRUCTS

//FUNCIONES

//Función que recibe un nodoDibujo (Esta función sera la raiz de la matriz de transformación del MODEL_VIEW)
void procesarDibujo(struct nodoDibujo *dibujo){
    if(!dibujo){
        return;
    }
    //AQUI VAMOS POR CADA LISTA DE ENTES...
}

//Función para cambiar el modo de vista entre Isometrica y Ortogonal
void cambiarModoVista(int modo){
    //(0:Isometrica, 1:Ortogonal)
    if(modo){ //Orto

    }
    else{ //Iso

    }
}


#endif