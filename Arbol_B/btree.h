
#include <tuple>
#include <vector>

#include "estructura_nodo.h"


// Funciones auxiliares
void initNodo(Nodo &n);
std::tuple<Nodo, Nodo, Llave_valor> split(const Nodo &nodo_lleno, IOStats* io);
void insertNonFull(std::vector<Nodo> &btree, int idxNodo, Llave_valor par, IOStats* io);
void insert(std::vector<Nodo> &btree, Llave_valor par, IOStats* io);

// Funciones principales
std::vector<Nodo> crearBtree(std::ifstream &archivo, int N, IOStats* io);
void escribirBTreeADisco(const std::vector<Nodo> &btree, const std::string &nombreArchivo, IOStats* io);
Nodo leerNodo(std::ifstream &archivo, int idxNodo, IOStats* io);

// Búsqueda por rango
void rangeSearchRec(std::ifstream &archivo, int idxNodo, int l, int u, std::vector<Llave_valor> &resultado, IOStats* io);
std::vector<Llave_valor> rangeSearch(const std::string &nombreArchivo, int l, int u, IOStats* io);
