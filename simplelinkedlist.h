#include <stdio.h>
#include <stdlib.h>
#include "TElemento.h"

struct Nodo{
    TElemento info;
    struct Nodo* sig;
};

typedef struct Nodo Tnodo;
typedef Tnodo* TLinkedList;

//Crea vacía la lista
void crearVacia(TLinkedList* l);
//Muestra la lista
void mostrar(TLinkedList l);
//Inserta por el principio TElemento e
void insertar(TElemento e, TLinkedList* l);
//Inserta por el final TElemento e
void insertarFinal(TElemento e, TLinkedList* l);
//Busca el id del TElemento en la lista y si lo encuentra lo elimina
int eliminar(int id, TLinkedList* l);
//Copia en  el  Telemento e el primer TElemento de la lista
void primero(TLinkedList l, TElemento* e);
//Elimina todos los TElemento de la lista menos el primero
void resto(TLinkedList* l);
//Copia en  el  Telemento e el último TElemento de la lista
void ultimo(TLinkedList l, TElemento* e);
//Concatena l1 y l2 
void concatenar(TLinkedList* l1, TLinkedList* l2);
//Devuelve si contiene o no un TElemento con ese id en la lista
int contiene(int id, TLinkedList l);
//Devuelve el TElemento que contenga ese id en la lista 
TElemento capturar(int id, TLinkedList l);
//Devuelve si la lista es vacía
int esVacia(TLinkedList l);
//Devuelve la longitud de la lista
int longitud(TLinkedList l);
//Copia la lista orginal en lista copia
void copiar(TLinkedList original, TLinkedList* copia);
//Destruye la lista l
void destruir(TLinkedList* l);