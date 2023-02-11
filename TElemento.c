#include <stdio.h>
#include <string.h>
#include "TElemento.h"

void print(TElemento t){
    printf("[%d] %d\n", t.id, t.pid);
}

void print2(TElemento t){
    printf("[%d]+	Running				%s\n", t.id, t.comando);
}

void printComando(TElemento t){
    char comandoAux[1024]="";
    strncpy(comandoAux, t.comando, strlen(t.comando)-2);
    printf("%s\n", comandoAux);
}

int devolverPid(TElemento t){
    return t.pid;
}

int devolverId(TElemento t){
    return t.id;
}

void asignar(TElemento* copia, TElemento original){
    copia->id = original.id;
    copia->pid = original.pid;
    strcpy(copia->comando, original.comando);
}

int igual(TElemento* uno, TElemento* dos){
    return (uno->id)==(dos->id);
}

int coincideId(int id, TElemento e){
    return (e.id)==id;
}

void crear(int id, int pid, char* comando, TElemento* e){
    e->id = id;
    e->pid = pid;
    strcpy(e->comando, comando);
}
