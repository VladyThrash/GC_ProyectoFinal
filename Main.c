//Proyecto Final: Graficación por Computadora.
//Vladimir Yepez Contreras - S23002520 - zS23002520@estudiantes.uv.mx
//IINF - FIEE - UV

//Librerías estándar
#include<stdlib.h>
#include<GL/freeglut.h>

//Librerías específicas
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"
#include "Gestor_OpenGL.h"


//CONSTANTES

//STRUCTS


//VARIABLES GLOBALES
unsigned long int numero_dibujo = 1; // <-- Indice del nodoDibujo sobre el que nos encontramos.
struct indiceHash *tablaHash = NULL; // <-- Tabla hash con la que obtendremos información de dibujado de cada instante.
struct colaXD *colaDibujado = NULL; // <-- Cola doblemente enlazada que permite gestionar las escenas.


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

}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void cerrar(void){
    //Aqui liberas memoria. Tambien se debe liberar memoria cuando se cierra con keyboard ESC.
}

void redibujo(void){
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
    
}

void specialKeyboard(int key, int x, int y){
    
}

void loadAll(void){

}