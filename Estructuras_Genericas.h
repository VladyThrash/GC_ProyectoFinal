//Hola Vlady del futuro!!! 
//En esta libreria vamos a tener las piezas (structs y funciones) para manejar estructuras de datos genericas.
//Utilizamos punteros void para poder generalizar el uso en diferentes secciones del proyecto.

#ifndef Estructuras_Genericas
#define Estructuras_Genericas
#include<stdlib.h>

//STRUCTS

//Struct Lista en una dirección
struct nodoLista1D{ 
    void *data;
    struct nodoLista1D *next;
};

//Struct Lista en dos direcciones
struct nodoLista2D{
    void *data;
    struct nodoLista2D *next;
    struct nodoLista2D *prev;
};

//Struct Cola, utilizamos de momento nodoLista1D para manejar los nodos. La cola solo se encarga apuntar al start y al end.
struct colaXD{ //XD -> x direccional. La cola maneja dos tipos de nodoLista: 1D y 2D.
    void *start;
    void *end;
    int oDir; //Orden de conexion (1D o 2D).
};

//Un hash simple.
struct indiceHash{
    void **data;
    unsigned int tam;
    unsigned int cap;
};

//Grafo dirigido
struct nodoGrafoD{
    void *data; //Su implementación especifica.
    struct nodoLista1D *lista; //Lista con todos los nodos hijos.
    struct nodoGrafoD *padre; //Puntero al nodo padre (podria o no ser utilizado).
};

//Aqui vamos a ir definiendo futuras estructuras, el Arbol por ejemplo...


//FUNCIONES

//Esta función retorna un número natural aleatorio entre un intervalo min-max.
int numeroAleatorio(int min, int max){
    if(min > max){ //Invertimos el intervalo.
        aux = min;
        min = max;
        max = aux;
    }
    
    if(max < 0){
        return -99999; //No se puede generar un número natural.
    }

    if(min < 0){
        min = 0; //Movemos el intervalo, no se van a generar negativos.
    }

    if(min == max){
        return min; //Solo existe un número en el intervalo.
    }

    if(min == 0){
        return rand() % max + 1; //El limite inferior es cero.
    }
    
    return (rand() % (max - min + 1)) + min; //[min, max]
}

//Crear un nodoLista1D. 
struct nodoLista1D* nuevoNodoLista1D(void *data){
    struct nodoLista1D *newNodo = NULL;
    newNodo = (struct nodoLista1D*)malloc(sizeof(struct nodoLista1D));
    if(!newNodo){
        return NULL;
    }
    newNodo->data = data;
    newNodo->next = NULL;
    return newNodo;
}

//Crear un nodoLista2D.
struct nodoLista2D* nuevoNodoLista2D(void *data){
    struct nodoLista2D *newNodo = NULL;
    newNodo = (struct nodoLista2D*)malloc(sizeof(struct nodoLista2D));
    if(!newNodo){
        return NULL;
    }
    newNodo->data = data;
    newNodo->next = NULL;
    newNodo->prev = NULL;
    return newNodo;
}

//Insertar un nuevo nodoLista1D en la lista, orden LIFO.
int insertarNodoLista1D(struct nodoLista1D **start, void *data){
    if(!(*start)){ //El inicio es NULL.
        *start = nuevoNodoLista1D(data);
        if(!(*start)){
            return 0; //No se pudo generar el primer nodo.
        }
        return 1; //Se genero el primer nodo.
    }

    struct nodoLista1D *newNodo= NULL;
    newNodo = nuevoNodoLista1D(data);
    if(!newNodo){
        return 0; //No se pudo insertar el nuevo nodo.
    }
    newNodo->next = *start;
    *start = newNodo;
    return 1;
}

//Insertar un nuevo nodoLista2D en la lista, orden LIFO.
int insertarNodoLista2D(struct nodoLista2D **start, void *data){
    if(!(*start)){ //El inicio es NULL.
        *start = nuevoNodoLista2D(data);
        if(!(*start)){
            return 0; //No se pudo generar el primer nodo.
        }
        return 1; //Se genero el primer nodo.
    }

    struct nodoLista2D *newNodo= NULL;
    newNodo = nuevoNodoLista2D(data);
    if(!newNodo){
        return 0; //No se pudo insertar el nuevo nodo.
    }
    (*start)->prev = newNodo;
    newNodo->next = *start;
    *start = newNodo;
    return 1;
}

//Crear una colaXD. Parametro: orden de dirección de los nodos (1 -> direción del siguiente nodo), 
//(2 -> dirección del siguiente y el anterior nodo), (otro -> no se podran insertar nodos en la cola).
struct colaXD* nuevaColaXD(int oDir){
    struct colaXD *newCont = NULL;
    newCont = (struct colaXD*)malloc(sizeof(struct colaXD));
    if(!newCont){
        return NULL;
    }
    newCont->start = NULL;
    newCont->end = NULL;
    newCont->oDir = oDir;
    return newCont;
}

//Insertar un nuevo nodo en la cola, orden FIFO. Para poder utilizar esta función primero se debe de crear la cola
//con la función (nuevaColaXD) ya que se debe definir el orden de dirección (1D o 2D).
int insertarEnColaXD(struct colaXD **cont, void *data){
    if(!(*cont)){ //La cola aun no ha sido inicializada.
        return 0;
    }

    //Debemos de verificar el orden de dirección.
    //Aqui obtenemos el tipo del nuevo nodo.
    void *newNodo = NULL;
    switch((*cont)->oDir){
        case 1:
            void *newNodo = nuevoNodoLista1D(data);
            break;
        
        case 2:
            void *newNodo = nuevoNodoLista2D(data);
            break;      
       }

    if(!newNodo){
        return 0; //No se pudo incializar el nuevo nodo.
    }

    if(!(*cont)->start){ //Es el primer nodo a insertar (cuando start es NULL, end tambien lo es).
        (*cont)->start = newNodo;
        (*cont)->end = newNodo;
        return 1;
    }

    //Ahora la forma en la que se coloca el nodo y se asignan los punteros depende del orden de direccón.
    switch ((*cont)->oDir){
        case 1:
            return colocarEnFinal1D(&(*cont)->end, newNodo);

        case 2:
            return colocarEnFinal2D(&(*cont)->end, newNodo);

        default:
            return 0; //Aunque este caso ya lo filtramos.

    }
}

//Coloca un nodoLista1D en el end de la cola y reacomoda los punteros.
int colocarEnFinal1D(void **e, void *ne){
    struct nodoLista1D *end = (struct nodoLista1D*)(*e);
    struct nodoLista1D *newEnd = (struct nodoLista1D*)ne;

    end->next = newEnd;
    end = newEnd;
    return 1;
}

//Coloca un nodoLista2D en el end de la cola y reacomoda los punteros.
int colocarEnFinal2D(void **e, void *ne){
    struct nodoLista2D *end = (struct nodoLista2D*)(*e);
    struct nodoLista2D *newEnd = (struct nodoLista2D*)ne;

    end->next = newEnd;
    newEnd->prev = end;
    end = newEnd;
    return 1;
}

//Funcion para crear el indice, lo incializa con un tamaño de 1000 slots.
struct indiceHash* crearIndiceHash(){
    struct indiceHash *hash = NULL;
    hash = (struct indiceHash*)malloc(sizeof(indiceHash));
    if(!hash){
        return NULL; //Error al generar el contenedor.
    }
    hash->data = (void**)malloc(sizeof(void*)*1000); //Con mil indice iniciales
    if(!(hash->data)){
        free(hash);
        return NULL; //Error al generar el arreglo dinamico.
    }
    hash->tam = 0;
    hash->cap = 1000;
    return hash;
}

//Función para insertar en indice hash, secuencial sin funcion de generado de indice.
int insertarIndiceHash(struct indiceHash *hash, void *data){
    if(!hash){
        return 0; //No se a incializado el indice
    }

    if(hash->tam == hash->cap){ //Reajustar el tamaño del indice
        hash->cap = hash->cap * 2; //Duplicamos la capacidad del indice.
        hash->data = (void**)realloc(hash->data, sizeof(void*)*hash->cap);
    }

    hash->data[hash->tam] = data; //Es llenado secuencial, no tenemos ninguna funcion que genere en indice. De momento no lo necesitamos.
    hash->tam++;
    return 1;
}

//Función para obtener un dato mediante el indice hash.
void* obtenerDatoHash(struct indiceHash *hash, unsigned long int indice){
    if(!hash){
        return NULL; // No se inicializo el indice
    }
    if(indice >= hash->cap){
        return NULL; //Indice fuera de rango
    }
    return hash->data[indice];
}

//Función para generar un nuevo nodoGrafoD.
struct nodoGrafoD* nuevoNodoGrafo(void *data, struct nodoGrafoD *padre){
    struct nodoGrafoD *nodo = NULL;
    nodo = (struct nodoGrafiD*)malloc(sizeof(struct nodoGrafoD));
    if(!nodo){
        return NULL; //No se pudo asignar espacio en memoria
    }

    nodo->data = data;
    nodo->lista = NULL;
    nodo->padre = padre;
    return nodo;
}

//Aqui vamos a ir definiendo las funciones de manejo de estructuras futuras, el Arbol por ejemplo...

//Tambien tienes que liberar memoria vlady <--- Al final es especifico de cada cliente, ya que desde aqui no 
//concemos que puede ser void* (Un perro, un gato o un alien).

#endif