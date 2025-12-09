//Basicamente maneja las funciones especificas de dibujado (segun ente o estado).

#ifndef Gestor_OpenGL
#define Gestor_OpenGL
#include<stdlib.h>
#include<GL/freeglut.h>
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"
#include "Agente.h"

//VARIABLES GLOBALES
float light_pos[] = {0.0f, 0.0f, 1.0f, 0.0f}; //Posición incial de la luz


//STRUCTS

//FUNCIONES

//Función para configurar el modo de proyección, el modo de vista y los atributos de la iluminación.
void iniciogl(){
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1, 0.4, 0.1, 1.0); //Verde Obscuro
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable(GL_LIGHTING);

    //Parametros de luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, sol.luz_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sol.luz_difusa_especular);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sol.luz_difusa_especular);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, sol.angulo_foco);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, sol.exponente_foco);
    glEnable(GL_LIGHT0);

    //Proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 200.0);

    //Vista
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Light_pos justo después del LoadIdentity para que la luz "salga" de la cámara
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glTranslatef(0.0, 0.0, -50.0); //Alejar la escena
    glRotatef(30.0, 1.0, 0.0, 0.0); //Elevar la vista
    //glTranslatef(0.0, -10.0, 0.0); 
}

//Función para cambiar el modo de vista entre Superior y Perspectiva, utiliza LookAt para seguir al agente.
//(0:Perspectiva, 1:Ortogonal)
void cambiarModoVista(int modo, float *posAgente){
    //Proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(modo){ //Ortogonal
        float zoom = 30.0f;
        glOrtho(-zoom, zoom, -zoom, zoom, 1.0, 150.0);
    } 
    else{ //Perspectiva
        gluPerspective(60.0, 1.0, 1.0, 200.0);
    }

    //Vista
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Aqui vlady no pudo configurar el lookAt y le pido ayuda a un duende mágico (Gemini).
    if(modo){ //Ortogonal superior
        // La cámara se pone justo encima del agente (ay + 50)
        // Mira hacia el agente (ax, ay, az)
        gluLookAt(posAgente[0], posAgente[1] + 50.0f, posAgente[2],  // OJO
                  posAgente[0], posAgente[1],         posAgente[2],  // CENTRO
                  0.0,                    0.0,                 -1.0);// ARRIBA
    } 
    else { //Perspectiva
        // La cámara se pone atrás (+40 en Z) y arriba (+30 en Y) del agente
        gluLookAt(posAgente[0], posAgente[1] + 30.0f, posAgente[2] + 40.0f, // OJO
                  posAgente[0], posAgente[1],         posAgente[2],         // CENTRO
                  0.0,                  1.0,                    0.0);       // ARRIBA
    }

    //Light_pos justo después del LoadIdentity para que la luz "salga" de la cámara
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}


//Función que recibe un nodoDibujo (Esta función sera la raiz de la matriz de transformación del MODEL_VIEW)
void procesarDibujo(struct nodoDibujo *dibujo, int modoVista, int frameAct){
    if(!dibujo){
        return;
    }
    
    //Obtenemos las listas de cada tipo de ente.
    struct nodoLista1D *estaticos = dibujo->entesEstaticos;
    struct nodoLista1D *dinamicos = dibujo->entesDinamicos;
    struct nodoLista1D *agentes = dibujo->agentes;

    //Antes de dibujar todo, le debo decir al lookAt donde va a estar el agente para que lo siga
    float posAgente[] = {0.0, 0.0, 0.0};
    if(agentes){
        struct nodoAgente *frameAgente = frameEspecificoAgente(agentes, frameAct); //Aunque solo seguiremos un agente (tal vez con varias ventanas podramos con todos).
        if(frameAgente){
            posAgente[0] = frameAgente->x;
            posAgente[1] = frameAgente->y;
        }
    }
    cambiarModoVista(modoVista, posAgente);

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