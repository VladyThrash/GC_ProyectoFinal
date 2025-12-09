//Basicamente maneja las funciones especificas de dibujado (segun ente o estado).

#ifndef Gestor_OpenGL
#define Gestor_OpenGL
#include<stdlib.h>
#include<GL/freeglut.h>
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"
#include "Agente.h"

//STRUCTS

//FUNCIONES

//Función para configurar el modo de proyección, el modo de vista y los atributos de la iluminación.
void iniciogl(){
    //Propiedades de la luz
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0,0.0,0.0,0.0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0,GL_AMBIENT,sol.luz_ambiente);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,sol.luz_difusa_especular);
    glLightfv(GL_LIGHT0,GL_SPECULAR,sol.luz_difusa_especular);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,sol.angulo_foco);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,sol.exponente_foco);
    glEnable(GL_LIGHT0);
    //glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mat_ambient_diffuse);
    //glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    //glMaterialf(GL_FRONT,GL_SHININESS,mat_shininess);

    //Configuración del modo de proyección y el modo vista
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0,100.0);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0,0.0,-27.0);
    glRotatef(13.0,1.0,0.0,0.0);
}

//Función para cambiar el modo de vista entre Isometrica y Ortogonal
void cambiarModoVista(int modo){
    //(0:Isometrica, 1:Ortogonal)
    if(modo){ //Orto

    }
    else{ //Iso

    }
}

//Función que recibe un nodoDibujo (Esta función sera la raiz de la matriz de transformación del MODEL_VIEW)
void procesarDibujo(struct nodoDibujo *dibujo, int frameAct){
    if(!dibujo){
        return;
    }
    
    //Obtenemos las listas de cada tipo de ente.
    struct nodoLista1D *estaticos = dibujo->entesEstaticos;
    struct nodoLista1D *dinamicos = dibujo->entesDinamicos;
    struct nodoLista1D *agentes = dibujo->agentes;

    //El dibujado es jerarquico, primero se dibujan partes del escenario como el suelo, depues los entes estaticos,
    //despues los entes dinamicos y por ultimo el agente (que es el mas errático).
    dibujarEscenario();
    
    if(estaticos){
        dibujarEntesEstaticos(estaticos); //Dibujamos los estaticos (no cambian, no necesitan el momento).
    }

    if(dinamicos){
        dibujarMomentoDinamicos(dinamicos, frameAct); //Necesitan saber el momento o instante.
    }

    if(agentes){
        dibujarMomentoAgentes(agentes, frameAct); //Necesitan saber el momento o instante.
    }

}

//Función para dibujar las lista de entes estaticos de la animación.
void dibujarEntesEstaticos(struct nodoLista1D *estaticos){
    struct nodoLista1D *aux = estaticos;
    while(aux){
        struct enteEstatico *ent = (struct enteEstatico*)aux->data;
        switch(ent->tipo){ //Si agregamos mas entes, debemos modificar todo este bloque.
            case 1:
                dibujarCasa((struct casa*)ent->data);
                break;
            
            case 2:
                dibujarEdificio((struct edificio*)ent->data);
                break;
        }
        aux = aux->next;
    }
}

//Función para dibujar la lista de entes dinamicos de la animación.
void dibujarMomentoDinamicos(struct nodoLista1D *estaticos, int frameAct){
    struct nodoLista1D *aux = estaticos;
    while(aux){
        //AQUI LLAMARIAMOS LA FUNCIÓN O CALLBACK PARA OBTENER EL FRAME ESPECIFICO DE LA ANIMACIÓN DEL DINAMICO
        aux = aux->next;
    }
}

//Función para dibujar la lista de agentes de la animación.
void dibujarMomentoAgentes(struct nodoLista1D *agentes, int frameAct){
    struct nodoLista1D *aux = agentes;
    while(aux){
        struct nodoAgente *frameAgente = frameEspecificoAgente(aux, frameAct);
        dibujarAgente(frameAgente);
        aux = aux->next;
    }
}

//Esta función dibuja el ente estatico casa.
void dibujarCasa(struct casa *casa){

}

//Esta función dibuja el ent estatico edificio.
void dibujarEdificio(struct edificio *edificio){
    
}

//Función para dibujar el estado actual del agente.
void dibujarAgente(struct nodoAgente *frameAgente){

}

//Esta función dibuja partes de escenario.
void dibujarEscenario(){

}

#endif