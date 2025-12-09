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
float color_suelo[] = {0.1f, 0.6f, 0.1f, 1.0f}; //Color del suelo del escenario
float color_casa[] = {0.9, 0.9, 0.9, 1.0}; //Color de las casas (blanco)
float color_techo[] = {0.8, 0.2, 0.2, 1.0}; //Color techo de las casas (rojito)
float color_edificio[] = {0.5, 0.5, 0.5, 1.0}; //Color de todos los edificios (gris)
float azul_cangrejo[] = {0.2, 0.4, 0.9, 1.0}; //Caparazón azul del cangrejo
float color_ojo[] = {1.0, 1.0, 1.0, 1.0}; //Ojo blanco del cangrejo
float color_pupila[] = {0.0, 0.0, 0.0, 1.0}; //Pupila negra del ojo del cangrejo

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
                dibujarCasa((struct casa*)ent->data, ent->x, ent->x);
                break;
            
            case 2:
                dibujarEdificio((struct edificio*)ent->data, ent->x, ent->y);
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
void dibujarCasa(struct casa *casa, float x, float y) {
    if(!casa){
        return; //No se asigno en memoria ninguna casa.
    }

    //Los planos estan invertidos XY -> XZ
    glPushMatrix();
        glTranslatef(x, 0.0, y); //Posicion base
        glPushMatrix();
            //Subimos para no dibujar entre el suelo
            glTranslatef(0.0, casa->altura/2, 0.0);
            //Damos las dimensiones delta y altura
            glScalef(casa->deltaX, casa->altura, casa->deltaY);
            //Pintamos toda la casa
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_casa);
            glutSolidCube(1.0); 
        glPopMatrix();
        
        if(casa->conTecho){ //Para generar aleatoriamente casas con techo o sin techo
            glTranslatef(0.0, casa->altura, 0.0);
            //Color techo
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_techo);
            //El duende tambien nos ayudo con los techos.
            glPushMatrix();
                // Rotamos -90 en X porque glutSolidCone apunta hacia Z por defecto, 
                // queremos que apunte hacia arriba (Y).
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                // El cono base tiene radio 1 y altura 1.
                float radioTecho = (casa->deltaX > casa->deltaY ? casa->deltaX : casa->deltaY) * 0.7f;
                glutSolidCone(radioTecho, casa->altura * 0.5f, 4, 4); // 4 lados = pirámide
            glPopMatrix();
        }
    glPopMatrix();
}

//Esta función dibuja el ent estatico edificio.
void dibujarEdificio(struct edificio *edificio, float x, float y){
    if (!edificio) {
        return; //No se asigno en memoria el edificio.
    }

    glPushMatrix()
        glTranslatef(x, 0.0, y);
        //Pintamos el edificio.
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_edificio);
        if(edificio->cilindrico){ //El edificio es un cilindro
            glPushMatrix();
                //Rotar el cilindro para que apunte a cielo
                glRotatef(-90.0, 1.0, 0.0, 0.0);
                //Aqui como rotamos los ejes, los planos si coinciden
                glScalef(edificio->deltaX, edificio->deltaY, edificio->altura);
                glutSolidCylinder(0.5, 1.0, 12, 2); 
            glPopMatrix();
        } 
        else{ //El edificio es un cubo
            glPushMatrix();
                //Subimos la mitad de su altura para que no quede entre el suelo
                glTranslatef(0.0f, edificio->altura / 2.0f, 0.0f);
                //Los planos estan invertidos XY -> XZ
                glScalef(edificio->deltaX, edificio->altura, edificio->deltaY);
                glutSolidCube(1.0); 
            glPopMatrix();
        }
    glPopMatrix();
}

//Función para dibujar el estado actual del agente.
void dibujarAgente(struct nodoAgente *frameAgente){
    if(!frameAgente){
        return; //No se inicializo el estado del agente.
    }

    //El duende mágico me ayudo un poco aquí (poquito).
    glPushMatrix();
        glTranslatef(frameAgente->x, 0.0, frameAgente->y); //Posicion del cangrejo
        //Ajusta el tamaño del cangrejo (es el mismo delta para todos los ejes)
        glScalef(frameAgente->deltaX, frameAgente->deltaX, frameAgente->deltaY);
        glTranslatef(0.0, 0.3, 0.0); //Movemos el cangrejo hacia arriba, para que no atravise el piso
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, azul_cangrejo); //Pintamo el cangrejo
        glPushMatrix();
            //Caparazón: esfera chata
            glScalef(1.0, 0.5, 0.7); 
            glutSolidSphere(0.5, 16, 16); 
        glPopMatrix();

        //Ojo Izquierdo
        glPushMatrix();
            glTranslatef(-0.2, 0.3, -0.2);
            
            //Tallo del ojo
            glPushMatrix();
                glRotatef(-90, 1, 0, 0); //Cilindro
                glutSolidCylinder(0.05, 0.3, 8, 2);
            glPopMatrix();
            
            //Globo ocular
            glTranslatef(0.0, 0.3, 0.0);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_ojo);
            glutSolidSphere(0.12, 8, 8);
            
            //Pupila
            glTranslatef(0.0, 0.0, -0.1);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_pupila);
            glutSolidSphere(0.05, 6, 6);
        glPopMatrix();

        //Ojo Derecho
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, azul_cangrejo);
        glPushMatrix();
            glTranslatef(0.2, 0.3, -0.2);
            
            //Tallo
            glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glutSolidCylinder(0.05, 0.3, 8, 2);
            glPopMatrix();

            //Globo ocular
            glTranslatef(0.0, 0.3, 0.0);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_ojo);
            glutSolidSphere(0.12, 8, 8);
            
            //Pupila
            glTranslatef(0.0, 0.0, -0.1);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_pupila);
            glutSolidSphere(0.05, 6, 6);
        glPopMatrix();

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, azul_cangrejo);
        //Pinza izquierda
        glPushMatrix();
            glTranslatef(-0.5, 0.0, -0.4);
            glRotatef(-30.0, 0.0, 1.0, 0.0); //Rotada hacia afuera
            glScalef(0.25, 0.25, 0.4); //Alargada
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();
        //Pinza derecha
        glPushMatrix();
            glTranslatef(0.5, 0.0, -0.4);
            glRotatef(30.0, 0.0, 1.0, 0.0); //Rotada hacia afuera
            glScalef(0.25, 0.25, 0.4); 
            glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        //Patitas
        for(int i=0; i<3; i++) {
            float offsetZ = (float)i * 0.2f; //Calcula la separación entre las patitas (Gemini).
            
            //Patas izquierdas
            glPushMatrix();
                glTranslatef(-0.5, -0.1, 0.0 + offsetZ);
                glScalef(0.4, 0.1, 0.1);
                glutSolidSphere(0.5, 6, 6);
            glPopMatrix();
            //Patas derechas
            glPushMatrix();
                glTranslatef(0.5, -0.1, 0.0 + offsetZ);
                glScalef(0.4, 0.1, 0.1);
                glutSolidSphere(0.5, 6, 6);
            glPopMatrix();
        }

    glPopMatrix();
}

//Esta función dibuja partes del escenario.
void dibujarEscenario(){
    glPushMatrix();
        //Color y material
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color_suelo);
        // glColor3f(0.1, 0.6, 0.1);
        glBegin(GL_POLYGON);
            glNormal3f(0.0, 1.0, 0.0);
            //Mi sistema de coordenadas es 2D, pero aqui estamos trabajando en 3D. 
            //El eje 'y' y el 'z' se invierten, es decir, lo que estaba originalmente pensado como un plano XY, 
            //ahora debe de dibujarse en el plano XZ.
            glVertex3f(-TAM_ESCENARIO, 0.0, -TAM_ESCENARIO); 
            glVertex3f(TAM_ESCENARIO, 0.0, -TAM_ESCENARIO);
            glVertex3f(TAM_ESCENARIO, 0.0, TAM_ESCENARIO);
            glVertex3f(-TAM_ESCENARIO, 0.0, TAM_ESCENARIO);
        glEnd();
    glPopMatrix();
}

#endif