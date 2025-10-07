#ifndef STRUCT_NODO_H
#define STRUCT_NODO_H

constexpr int b = 340;

struct Llave_valor {
    int llave;
    float valor;
} ;

struct Nodo {
    int es_interno;
    int k;
    Llave_valor llaves_valores[b];
    int hijos[b+1];
    int siguiente = -1;
} ;

#endif