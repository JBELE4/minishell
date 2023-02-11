#ifndef TELEMENTO_H
#define TELEMENTO_H
#include <stdio.h>
#include <string.h>

typedef struct TBackground{
     int id;                  //Posición que ocupa en background
     int pid;                 //Pid del proceso  que introducimos al background
     char comando[1024];      //Comando  
}TElemento;

//Imprime [id] pid
void print(TElemento t);
//Imprime  [id]+	Running			comando
void print2(TElemento t);
//Imprime  el comando del TElemento t
void printComando(TElemento t);
//Devuelve  el  pid  del  TElemento t
int devolverPid(TElemento t);
//Devuelve  el  id  del  TElemento t
int devolverId(TElemento t);
//Copia TElemento original en TElemento copia
void asignar(TElemento* copia, TElemento original);
//Devuelve si dos TElemento son iguales
int igual(TElemento* uno, TElemento* dos);
//Devuelve si el id que le pasamos coincide con el id del TElemento e
int coincideId(int id, TElemento e);
//Crea un TElemento e
void crear(int id, int pid, char* comando, TElemento* e);

#endif