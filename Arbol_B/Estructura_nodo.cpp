#include <iostream>

//Se deja este valor como constante debido a su reiterado uso
constexpr int b = 340;

/**
 * @brief Representa la estructura de una arreglo de a pares llave-valor
 *
 * Esta estructura almacena información a modo de un diccionario
 * incluyendo la llave (int) y valor (float).
**/

typedef struct{
    int llave;
    float valor;
} Llave_valor;

/**
 * @brief Representa la estructura de un nodo perteneciente a un Btreee.
 *
 *• es_interno: un int (4 bytes) que indica si el nodo es interno o un nodo externo (hoja).
 *• k: un int (4 bytes) que guarda la cantidad de pares llave-valor actualmente contenidas en el nodo.
 *• llaves_valores un arreglo de 𝑏 pares llave-valor, con llaves de tipo int (4 bytes), y valores de tipo float (4 bytes).
 *• hijos: un arreglo de 𝑏 + 1 enteros de tipo int (4 bytes), que guarda las posiciones en disco de los 𝑘 + 1 hijos del nodo.
 *• siguiente: un entero de tipo int (4 bytes), que solo se utiliza en las hojas de los árboles B+ (para cualquier otro nodo, se fija este valor como −1).
 */
typedef struct {
    int es_interno;
    int k;
    Llave_valor llaves_valores[b];
    int hijos[b+1];
    int siguiente = -1;
} Nodo;