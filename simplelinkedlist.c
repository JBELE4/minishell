#include <stdio.h>
#include <stdlib.h>
#include "simplelinkedlist.h"

void crearVacia(TLinkedList* l){
    *l=NULL;
}

void mostrar(TLinkedList l){
    TLinkedList aux=l;
    while(aux!=NULL){
        print2(aux->info);
        aux=aux->sig;
    }
}

void insertar(TElemento e, TLinkedList* l){
    TLinkedList aux=(TLinkedList) malloc(1*sizeof(Tnodo));
    aux->sig=*l;
    asignar(&(aux->info), e);
    *l=aux;
}

void insertarFinal(TElemento e, TLinkedList* l){
    if(esVacia(*l)){
        insertar(e, l);
    }else{
        TLinkedList aux=*l;
        while(aux->sig!=NULL){
            aux=aux->sig;
        }
        Tnodo* nuevo=(Tnodo*) malloc(1*sizeof(Tnodo));
        nuevo->sig=NULL;
        asignar(&(nuevo->info), e);
        aux->sig=nuevo;
    }
}

int eliminar(int id, TLinkedList* l){
    TLinkedList act=*l;
    TLinkedList ant=NULL;
    int encontrado=0;
    while((act!=NULL) && (!encontrado)){
        encontrado=coincideId(id, act->info);
        if(!encontrado){
            ant=act;
            act=act->sig;
        }
    }
    if(encontrado){
        if(ant==NULL){
            (*l)=(*l)->sig;
        }else{
            ant->sig=act->sig;
        }
        free(act);
    }
    return encontrado;
}

void primero(TLinkedList l, TElemento* e){
    if(!esVacia(l)){
        asignar(e, l->info);
    }
}

void resto(TLinkedList* l){
    if(!esVacia(*l)){
        TLinkedList aux=*l;
        (*l)=(*l)->sig;
        free(aux);
    }
}

void ultimo(TLinkedList l, TElemento* e){
    if(!esVacia(l)){
        TLinkedList aux=l;
        while(aux->sig!=NULL){
            aux=aux->sig;
        }
        asignar(e, aux->info);
    }
}

void concatenar(TLinkedList* l1, TLinkedList* l2){
    TLinkedList aux=*l2;
    while(aux!=NULL){
        insertarFinal(aux->info, l1);
        aux=aux->sig;
    }
}

int contiene(int id, TLinkedList l){
    TLinkedList act=l;
    int encontrado=0;
    while((act!=NULL) && (!encontrado)){
        encontrado=coincideId(id, act->info);
        if(!encontrado){
            act=act->sig;
        }
    }
    return encontrado;
}

TElemento capturar(int id, TLinkedList l){
    TLinkedList act=l;
    int encontrado=0;
    while((act!=NULL) && (!encontrado)){
        encontrado=coincideId(id, act->info);
        if(!encontrado){
            act=act->sig;
        }
    }
    return act->info;
}


int esVacia(TLinkedList l){
    return (l==NULL);
}

int longitud(TLinkedList l){
    TLinkedList aux=l;
    int longi=0;
    while((aux)!=NULL){
        longi++;
        aux=aux->sig;
    }
    return longi;
}

void copiar(TLinkedList original, TLinkedList* copia){
    crearVacia(copia);
    if (!esVacia(original)){
        TLinkedList auxO = original;
        TLinkedList aux = (TLinkedList) malloc(1*sizeof(Tnodo));
        *copia=aux;
        TLinkedList ant =aux; 
        asignar(&(aux->info), auxO->info);
        auxO=auxO->sig;
        while (auxO!=NULL){
            TLinkedList aux = (TLinkedList) malloc(1*sizeof(Tnodo));
            ant->sig=aux;
            asignar(&(aux->info), auxO->info);
            auxO=auxO->sig;
            ant=aux;
        }
        ant->sig=NULL;
    }
}

void destruir(TLinkedList* l){
    TLinkedList ant=NULL;
    TLinkedList act=*l;
    while(act!=NULL){
        ant=act;
        act=act->sig;
        free(ant);
    }
    free(ant);
    *l=NULL;
}

