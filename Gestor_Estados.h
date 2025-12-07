//Aqui vamos a desarrollar la cola de estados (momentos de la animacion), y obtener el orden que alimentamos
//OpenGL. 

//1.- Cola de dibujado: Es la que le dice a OpenGl que va a dibujar en cada momento. Cada nodo es un momento de dibujado,
//                      es decir, le damos a dibujar a OpenGl todo lo que el nodo tiene.

//2.- Cada nodo de la cola de dibujado se puede ver como una raiz, a partir de esa raiz tenemos jerarquias,
//    donde cada nivel representa el orden en que le vamos a indicar a OpenGL que dibuje. Entes estaticos, 
//    entes con calculos simples y agentes complejos.

//3.- Los niveles jerarquicos que lo requieran se recalculan en tiempo, es decir, entre nodos de la cola de dibujado.

//4.- El recalculado es con respecto al estado actual, debemos de ser capaces de propagar hacia el pasado y hacia el
//    futuro el estado de los entes. El fin de esto es poder tomar un nodo 'x' de la cola de dibujado, modificar los entes
//    de la jerarquia, y que estos cambios se vean reflejados en todos los entes de todos los nodos de la cola.


#ifndef Gestor_Estados
#define Gestor_Estados
#include<stdlib.h>
#include "Estructuras_Genericas.h"

//Hola vlady, aqui te quedaste!!!

#endif