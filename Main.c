//Proyecto Final: Graficación por Computadora.
//Vladimir Yepez Contreras - S23002520 - zS23002520@estudiantes.uv.mx
//IINF - FIEE - UV

//Librerías estándar
#include<stdlib.h>
#include<stdio.h>
#include<time.h> //Para generar numeros aleatorios
#include<GL/freeglut.h>

//Librerías específicas
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"
#include "Gestor_OpenGL.h"


//CONSTANTES
#define LIMITE_DIBUJOS 10000 //Limite de dibujos en la cola de animación.
#define SEG 24 //Según mis calculos esto es un segundo.

//STRUCTS


//VARIABLES GLOBALES

//Punteros a estructuras
struct indiceHash *tablaHash = NULL; // <-- Tabla hash con la que obtendremos información de dibujado de cada instante.
struct colaXD *colaDibujado = NULL; // <-- Cola doblemente enlazada que permite gestionar las escenas.
struct lista1D *estaticos = NULL; //Obstaculos que se pueden agregar (son estaticos).
struct lista1D *dinamicos = NULL; //Obstaculos con comportamientos definidos (no se pueden agregar).
struct lista1D *agente = NULL; //Nuestro agente que se movera por el escenario.

//Booleanos
unsigned long int numero_dibujo = 1; // <-- Indice del nodoDibujo sobre el que nos encontramos.
int detener = 0; // <-- Funcionara como el booleano que nos indica si nos detenemos en un dibujo-escena.
int avanza_retrocede = 0; // <-- Funcionara como el booleano que indica si avanza o retrocede la animación.
int modo_vista = 0; // <-- Booleano que nos indica el modo de vista (0:Isometrico, 1:Ortogonal).

//PROTOTIPOS

//Funciones de configuración y callbacks de OpenGL.
void iniciogl(void);
void display(void);
void cerrar(void);
void redibujo(void);
void resize(int width, int height);
void keyboard(unsigned char key, int a, int b);
void specialKeyboard(int key, int x, int y);

//Función para cargar las estructuras (cola de dibujado y entes del escenario).
void loadAll(void);

//FUNCIONES
int main(int argc, char **argv){
    srand(time(NULL)); //Para las funciones que utilizan aleatorios
    loadAll(); //Cargar las estructuras
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(30,30);
    glutInitWindowSize(700,700);
    glutCreateWindow("Walking Crabs");

    iniciogl(); //Configurar la camara y la vista
    glutDisplayFunc(display);
    glutCloseFunc(callbackCerrar); 
    glutReshapeFunc(resize); //Redimensiona la ventana
    glutIdleFunc(redibujo); //Función de recalculado
    glutKeyboardFunc(keyboard); //Teclado
    glutSpecialFunc(); //Teclas especiales del teclado (Flechas)
    glutMainLoop();

    return 0; 
}

void iniciogl(void){
    //Configura la la proyeccion y la vista, tambien la luz.
}

void display(void){
    //Aqui obtenemos el dibujo-escena dado el indice y lo mandamos a dibujar.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    struct nodoDibujo *dibujoAct = obtenerDibujoActual(tablaHash, numero_dibujo);
    procesarDibujo(dibujoAct); //Se procesa y dibuja esta escena en especifico.

    glutSwapBuffers();
}

void cerrar(void){
    //Aqui liberas memoria. Tambien se debe liberar memoria cuando se cierra con keyboard ESC.
}

void redibujo(void){
    //Esta sera la función que toma la lista de entes y actualiza o calcula sus estados.
    //Tambien itera sobre el indice de dibujos-escenas.
    glutSwapBuffers();//Para intercambiar los buffers de color de la ventana, hace que el dibujo sea visible.
   	glutPostRedisplay();//refrescar
}

void resize(int width, int height){
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 1.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int a, int b){
    //Puede avanzar, retroceder o detener en la animación.
    //d -> Detener
    //a -> Avanzar
    //r -> Retroceder
    //o -> Nuevo obtaculo aleatorio
    //v -> Cambiar la vista (ortogonal y isometrico).

    if(key == 27){ //Liberar todo cuando se cierra con ESC
        cerrar();
		exit(0);
    }

    if(key =='d' || key =='D'){ //Se detiene la animación
        detener = 1;
    } 

    if(key == 'a' || key == 'A'){ //La animación avanza
        detener = 0;
        avanza_retrocede = 0;
    }

    if(key == 'r' || key == 'R'){ //La animación retrocede
        detener = 0;
        avanza_retrocede = 1;
    }
    
    if(key == 'o' || key == 'O'){ //Agrega un obstaculo a la lista de entes estaticos, aleatorio
        //Una función de Gestor_Entes
        //La trayectoria del agente debe recalcularse.
        agregarNuevoEnteEstatico(estaticos);
    }

    if(key == 'v' || key == 'V'){ //Cambiar la vista: Ortogonal o Isometrica
        //(0:Isometrica, 1:ortogonal)
        modo_vista = modo_vista ? 0 : 1;
        cambiarModoVista(modo_vista);
    }

    if(!detener){ //Si no esta en pausa, se redibuja.
        numero_dibujo = (avanza_retrocede) ? (numero_dibujo - 1) : (numero_dibujo + 1);
        glutSwapBuffers();
   	    glutPostRedisplay();
    }
    //No se actuliza el numero_dibujo si esta en pausa.
}

void specialKeyboard(int key, int x, int y){
    //Solo si la animación esta pausada:
    //flecha derecha -> Avanzar un segundo (¿Cuanto es un segundo medido en nodoDibujo?)
    //flecha izquierda -> Retroceder un segundo
    if(detener){
        if(key == GLUT_KEY_RIGHT){ //Avanzar un segundo en la animación
            numero_dibujo = (numero_dibujo < LIMITE_DIBUJOS) ? (numero_dibujo + SEG) : numero_dibujo;
        }
        
        if(key == GLUT_KEY_LEFT){ //Retroceder un segundo en la animación
            numero_dibujo = (numero_dibujo > 0) ? (numero_dibujo - SEG) : numero_dibujo;
        }

        glutSwapBuffers();
   	    glutPostRedisplay();
    }
}

void loadAll(void){
    estaticos = crearListaEstaticos(15);
    
}