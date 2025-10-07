#ifndef STRUCT_NODO_H
#define STRUCT_NODO_H

constexpr int b = 340;

struct Llave_valor {
    int llave;
    float valor;
} ;

// Constantes globales para contabilizar la cantidad de I/Os de árbol B guardadas en una estructura

struct IOStats {
    int lecturas = 0;
    int escrituras = 0;
};

IOStats io;


struct Nodo {
    int es_interno;
    int k;
    Llave_valor llaves_valores[b];
    int hijos[b+1];
    int siguiente = -1;
} ;

#endif