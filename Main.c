//Proyecto Final: Graficación por Computadora.
//Vladimir Yepez Contreras - S23002520 - zS23002520@estudiantes.uv.mx
//IINF - FIEE - UV

//gcc Main.c -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lfreeglut -lglu32 -lopengl32 -o walking_crab.exe -fsanitize=address -g

//Librerías estándar
#include<stdlib.h>
#include<stdio.h>
#include<time.h> //Para generar números aleatorios
#include<GL/freeglut.h>

//Librerías específicas
#include "Estructuras_Genericas.h"
#include "Gestor_Estados.h"
#include "Gestor_Entes.h"
#include "Gestor_OpenGL.h"
#include "Agente.h"

//CONSTANTES
#define LIMITE_DIBUJOS 10000 //Limite de dibujos en la cola de animación.
#define SEG 24 //Según mis calculos esto es un segundo.
float startXY[] = {-160, 160}; //El estado incial del agente. NOTA: REDUJE EL TAMAÑO DEL ESCENARIO, POR ENDE START Y GOAL CAMBIARON
float targetXY[] = {160, -160}; //El objetivo del agente.

//VARIABLES GLOBALES

//Punteros a estructuras
struct indiceHash *tablaHash = NULL; // <-- Tabla hash con la que obtendremos información de dibujado de cada instante.
struct colaXD *colaDibujado = NULL; // <-- Cola doblemente enlazada que permite gestionar las escenas.
struct nodoLista1D *estaticos = NULL; //Obstaculos que se pueden agregar (son estaticos).
struct nodoLista1D *dinamicos = NULL; //Obstaculos con comportamientos definidos (no se pueden agregar en ejecución).
struct nodoLista1D *agente = NULL; //Nuestro agente que se movera por el escenario.

//Booleanos
//unsigned long int numero_dibujo = 0; //< -- POSIBLE ERROR POR ACCESO A INDICE HASH INDEFINIDO

unsigned long int numero_dibujo = 1;// <-- Indice del nodoDibujo sobre el que nos encontramos.
int detener = 1; // <-- Funcionara como el booleano que nos indica si nos detenemos en un dibujo-escena.
int avanza_retrocede = 0; // <-- Funcionara como el booleano que indica si avanza o retrocede la animación.
int modo_vista = 0; // <-- Booleano que nos indica el modo de vista (0:Isometrico, 1:Ortogonal).
int primer_dibujo = 1; //AGREGADO: Para liberar solamente una vez las listas de entes.

//PROTOTIPOS

//Funciones de configuración y callbacks de OpenGL.
void display(void);
void cerrar(void);
void redibujo(void);
void resize(int width, int height);
void keyboard(unsigned char key, int a, int b);
void specialKeyboard(int key, int x, int y);

//Función para cargar las estructuras (cola de dibujado y entes del escenario).
void loadAll(void);

//Funciones para liberar la memoria al cerrar el programa.
void liberarColaDibujo(struct colaXD *colaDibujo);
void liberarListaColaDibujo(struct nodoLista1D *lista);
void liberarNodoDibujo(struct nodoDibujo *dibujo);
void liberarListaEntesEstaticos(struct nodoLista1D *lista);
void liberarEnteEstatico(struct enteEstatico *ente);
void liberarListaEntesDinamicos(struct nodoLista1D *lista);
void liberarListaAgentes(struct nodoLista1D *lista);
void liberarAgente(struct nodoGrafoD *agente);
void liberarListaGrafo(struct nodoLista1D *lista);
void liberarListaSolucion(struct nodoLista2D *lista);
void liberarListaNodosExistentes(struct nodoLista1D *lista);
void liberarIndiceHash(struct indiceHash *hash);

//FUNCIONES
int main(int argc, char **argv){
    srand(time(NULL)); //Para las funciones que utilizan aleatorios
    loadAll(); //Cargar las estructuras
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(30,30);
    glutInitWindowSize(700,700);
    glutCreateWindow("Walking Crabs");

    iniciogl(); //Configurar la camara y la vista (Es una función de Gestor_OpenGL)
    glutDisplayFunc(display);
    glutCloseFunc(cerrar); 
    glutReshapeFunc(resize); //Redimensiona la ventana
    glutIdleFunc(redibujo); //Función de recalculado
    glutKeyboardFunc(keyboard); //Teclado
    glutSpecialFunc(specialKeyboard); //Teclas especiales del teclado (Flechas)
    glutMainLoop();

    return 0; 
}

void display(void){
    //Aqui obtenemos el dibujo-escena dado el indice y lo mandamos a dibujar.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    struct nodoDibujo *dibujoAct = obtenerDibujoActual(tablaHash, numero_dibujo);
    procesarDibujo(dibujoAct, modo_vista, numero_dibujo); //Se procesa y dibuja esta escena en especifico.
    printf("Dibujando frame: %d\n", numero_dibujo);
    glutSwapBuffers();
}

void cerrar(void){
    //Aqui liberas memoria. Tambien se debe liberar memoria cuando se cierra con keyboard ESC.
    printf("Saliendo del programa...\n");
    printf("Liberando memoria...\n");
    liberarColaDibujo(colaDibujado);
    liberarIndiceHash(tablaHash);
    liberarListaNodosExistentes(nodosExistentes);
    printf("Memoria liberada correctamente!!!\n");
}

void redibujo(void){
    //Esta sera la función que toma la lista de entes y actualiza o calcula sus estados.
    //Tambien itera sobre el indice de dibujos-escenas.
    //Sobre todo para entes dinamicos!!!
    //Sleep(5);
    if(detener){
        if(numero_dibujo < (frames_agente - 1) && numero_dibujo > 1){ //MODIFICACION
            numero_dibujo = (avanza_retrocede) ? (numero_dibujo - 1) : (numero_dibujo + 1); //EL ITERADOR VA AQUI, NO EN void keyboard().       
        }
    }

    //glutSwapBuffers();//Para intercambiar los buffers de color de la ventana, hace que el dibujo sea visible.
   	glutPostRedisplay();//refrescar
}

void resize(int width, int height){
    if(height == 0){
        height = 1; //Evitar división por cero.
    }

    glViewport(0, 0, width, height);
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
        //detener = 1 <--- TAMBIEN ERA UN ERROR
        detener = 0;
    } 

    if(key == 'a' || key == 'A'){ //La animación avanza
        //detener = 0; ERROR
        detener = 1;
        avanza_retrocede = 0;
        numero_dibujo++; //MODIFICACION
    }

    if(key == 'r' || key == 'R'){ //La animación retrocede
        detener = 1;
        avanza_retrocede = 1;
        numero_dibujo--; //MODIFICACION
    }
    
    //TODO ESTE BLOQUE FUE MODIFICADO (SE AÑADIO LA ELIMINACION DEL GRAFO VIEJO).
    if(key == 'o' || key == 'O'){ //Agrega un obstaculo a la lista de entes estaticos, aleatorio
        agregarNuevoEnteEstatico(&estaticos);
        
        //Liberar el grafo y la solución actual.
        liberarListaGrafo(((struct nodoGrafoD*)agente->data)->lista); //ESTO ES NUEVO
        ((struct nodoGrafoD*)agente->data)->lista = NULL;
        struct nodoAgente *agenteAct = (struct nodoAgente*)((struct nodoGrafoD*)agente->data)->data;
        liberarListaSolucion(agenteAct->solucion); //ESTO ES NUEVO
        liberarListaNodosExistentes(nodosExistentes); //ESTO ES NUEVO
        //La trayectoria del agente debe recalcularse.
        agenteAct->solucion = rrt((struct nodoGrafoD*)agente->data, targetXY, estaticos);
        //La cola debe de redimiensionarse de ser necesario.
        redimensionarColaDibujos(colaDibujado, tablaHash, frames_agente);
        printf("Nuevo numero de frames: %ld\n", frames_agente);
    }

    if(key == 'v' || key == 'V'){ //Cambiar la vista: Ortogonal o Perspectiva
        //(0:Perspectiva, 1:Ortogonal)
        modo_vista = modo_vista ? 0 : 1;
        struct nodoAgente *agenteAct = frameEspecificoAgente(agente, numero_dibujo);
        float posAgente[] = {agenteAct->x, agenteAct->y};
        cambiarModoVista(modo_vista, posAgente);
    }
    /*
    if(detener){ //Si no esta en pausa, se redibuja.

        //numero_dibujo = (avanza_retrocede) ? (numero_dibujo - 1) : (numero_dibujo + 1); //ERROR: ESTO VA EN void redibujo(void);
        
        //if(numero_dibujo < frames_agente){ ... }
        if(numero_dibujo < frames_agente || numero_dibujo > 1){ //MODIFICACION
            glutSwapBuffers();
   	        glutPostRedisplay();
        }
    }*/
    //No se actuliza el numero_dibujo si esta en pausa.
    //glutSwapBuffers();
   	glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y){
    //Solo si la animación esta pausada:
    //flecha derecha -> Avanzar un segundo (¿Cuanto es un segundo medido en nodoDibujo?)
    //flecha izquierda -> Retroceder un segundo
    
    //if(detener){...} //MODIFICACION
    if(!detener){
        if(key == GLUT_KEY_RIGHT){ //Avanzar un segundo en la animación
            numero_dibujo = (numero_dibujo < frames_agente) ? (numero_dibujo + SEG) : numero_dibujo;
        }
        
        if(key == GLUT_KEY_LEFT){ //Retroceder un segundo en la animación
            numero_dibujo = (numero_dibujo > 0) ? (numero_dibujo - SEG) : numero_dibujo;
        }

        //glutSwapBuffers();
   	    glutPostRedisplay();
    }
}

void loadAll(void){
    //Crea la lista de entes estaticos.
    estaticos = crearListaEstaticos(15);
    printf("Entes estaticos cargados...\n");
    //Crea los frames-dibujos de la trayectoria del agente.
    agente = agregarAgente(startXY, targetXY, estaticos); //Cada vez que se recalcula la trayectoria con rrt, se actualiza frames_agentes
    printf("Agente cargado...\n");
    //Crea los frames-dibujos de la animación.
    tablaHash = crearIndiceHash();
    colaDibujado = crearColaDibujo(estaticos, dinamicos, agente);
    generarTodosLosDibujos(colaDibujado, tablaHash, frames_agente);
    printf("Cola de dibujo cargada...\n");
    printf("Se cargaron todas las estructuras!!!\n");
    printf("Numero de total de frames: %ld\n", frames_agente);
}

//Funciones para liberar memoria (desde main, ya que aquí conocemos todos los tipos que existen).

void liberarColaDibujo(struct colaXD *colaDibujo){
    if(!colaDibujo){
        return;
    }

    liberarListaColaDibujo((struct nodoLista1D*)colaDibujado->start);
    free(colaDibujo);
}

void liberarListaColaDibujo(struct nodoLista1D *lista){
    if(!lista){
        return;
    }
    liberarListaColaDibujo(lista->next);
    liberarNodoDibujo((struct nodoDibujo*)lista->data);
    free(lista);
}

void liberarNodoDibujo(struct nodoDibujo *dibujo){ //FUNCION MODIFICADA
    if(!dibujo){
        return;
    }

    if(primer_dibujo){
        liberarListaEntesEstaticos(dibujo->entesEstaticos);
        printf("Entes estaticos liberados...\n");
        liberarListaEntesDinamicos(dibujo->entesDinamicos);
        printf("Entes dinamicos liberados...\n");
        liberarListaAgentes(dibujo->agentes);
        printf("Agentes liberados...\n");
        primer_dibujo = 0;
    }
    
    free(dibujo);
}

void liberarListaEntesEstaticos(struct nodoLista1D *lista){
    if(!lista){
        return;
    }

    liberarListaEntesEstaticos(lista->next);
    liberarEnteEstatico((struct enteEstatico*)lista->data);
    free(lista);
}

void liberarEnteEstatico(struct enteEstatico *ente){
    if(!ente){
        return;
    }
    free(ente->data);
    free(ente);
}

void liberarListaEntesDinamicos(struct nodoLista1D *lista){
    if(!lista){
        return;
    }

    liberarListaEntesDinamicos(lista->next);
    free(lista);
}

void liberarListaAgentes(struct nodoLista1D *lista){
    if(!lista){
        return;
    }

    liberarListaAgentes(lista->next);
    liberarAgente((struct nodoGrafoD*)lista->data);
    free(lista);
}

void liberarAgente(struct nodoGrafoD *agente){ //MODIFIQUE ESTA FUNCIÓN
    if(!agente){
        return;
    }

    struct nodoLista1D *aux = agente->lista;
    while(aux){
        liberarAgente((struct nodoGrafoD*)aux->data);
        struct nodoLista1D *elim = aux;
        aux = aux->next;
        free(elim);
    }

    liberarListaSolucion(((struct nodoAgente*)agente->data)->solucion);
    free(agente->data);
    free(agente);
}

void liberarListaGrafo(struct nodoLista1D *lista){
    if(!lista){
        return;
    }

    liberarListaGrafo(lista->next);
    liberarAgente((struct nodoGrafoD*)lista->data);
    free(lista);
}

void liberarListaSolucion(struct nodoLista2D *lista){
    if(!lista){
        return;
    }

    liberarListaSolucion(lista->next);
    free(lista);
}

void liberarListaNodosExistentes(struct nodoLista1D *lista){ //ESTA FUNCION ES NUEVA
    if(!lista){
        return;
    }

    struct nodoLista1D *aux = lista;
    while(aux){
        struct nodoLista1D *elim = aux;
        aux = aux->next;
        free(elim);
    }
}

void liberarIndiceHash(struct indiceHash *hash){
    free(hash->data);
    free(hash);
}