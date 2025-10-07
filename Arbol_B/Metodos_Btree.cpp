#include <math.h>

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "estructura_nodo.h"

// Función para inicializar un nodo (debido a que constantemente se utiliza el crear uno nuevo con campos predeterminados)
void initNodo(Nodo& n) {
    n.k = 0;
    n.es_interno = 0;
    n.siguiente = -1;              // Como estamos en los métodos que corresponden a un Btree este campo se deja en -1 por defecto
    for (int i = 0; i < b; ++i) {  // Vamos a hacer que los pares llave-valor sean todos inicializados en (0,0.0) para este nodo
        n.llaves_valores[i].llave = 0;
        n.llaves_valores[i].valor = 0.0f;
        n.hijos[i] = -1;
    }
    n.hijos[b] = -1;  // b+1 hijos posibles los reservamos con índices -1
}

std::tuple<Nodo, Nodo, Llave_valor> split(const Nodo& nodo_lleno, IOStats* io) {
    io->lecturas++;  // nodo_lleno.
    int k_total = nodo_lleno.k;
    int mid = k_total / 2;  // índice de la mediana: mid, usaremos mid en vez de b/2 o k_total para evitar segfault
    Llave_valor mediano = nodo_lleno.llaves_valores[mid];
    io->lecturas++;  // acceso a la mediana

    Nodo nodo_izq, nodo_der;
    initNodo(nodo_izq);
    initNodo(nodo_der);
    nodo_izq.es_interno = nodo_lleno.es_interno;
    io->lecturas++;
    io->escrituras++;
    nodo_der.es_interno = nodo_lleno.es_interno;
    io->lecturas++;
    io->escrituras++;

    for (int i = 0; i < mid; ++i) {
        io->lecturas++;                                             // leer nodo_lleno.llaves_valores[i]
        nodo_izq.llaves_valores[i] = nodo_lleno.llaves_valores[i];  // copiamos los pares a la izquierda del mediano
    }
    nodo_izq.k = mid;

    int right_count = 0;
    for (int i = mid + 1; i < k_total; ++i) {
        io->lecturas++;
        nodo_der.llaves_valores[right_count++] = nodo_lleno.llaves_valores[i];  // copiamos los pares a la derecha en el nodo derecho
    }
    nodo_der.k = right_count;

    if (nodo_lleno.es_interno) {  // guardar los hijos solo si el nodo es interno
        for (int i = 0; i <= mid; ++i) {
            io->lecturas++;
            nodo_izq.hijos[i] = nodo_lleno.hijos[i];
        }
        int idx = 0;
        for (int i = mid + 1; i <= k_total; ++i) {
            io->lecturas++;
            nodo_der.hijos[idx++] = nodo_lleno.hijos[i];
        }
    }

    return std::make_tuple(nodo_izq, nodo_der, mediano);  // retornamos una triple tupla nodo_izq,nodo_der,mediano
}

// Función insertNonFull con conteo
void insertNonFull(std::vector<Nodo>& btree, int idxNodo, Llave_valor par, IOStats* io) {
    io->lecturas++;  // leer btree[idxNodo].k
    int i = btree[idxNodo].k - 1;

    io->lecturas++;
    if (!btree[idxNodo].es_interno) {
        while (i >= 0 && (io->lecturas++, btree[idxNodo].llaves_valores[i].llave) > par.llave) {
            io->lecturas++;
            io->escrituras++;
            btree[idxNodo].llaves_valores[i + 1] = btree[idxNodo].llaves_valores[i];
            --i;
        }
        btree[idxNodo].llaves_valores[i + 1] = par;
        io->escrituras++;
        btree[idxNodo].k++;
        io->escrituras++;
        return;
    }

    while (i >= 0 && (io->lecturas++, btree[idxNodo].llaves_valores[i].llave) > par.llave) --i;
    ++i;
    io->lecturas++;
    int idxHijo = btree[idxNodo].hijos[i];

    io->lecturas++;
    if (btree[idxHijo].k >= b) {
        auto [hijo_izq, hijo_der, mediano] = split(btree[idxHijo], io);

        int idxIzq = idxHijo;
        int idxDer = (int)btree.size();

        btree[idxIzq] = hijo_izq;
        io->escrituras++;
        btree.push_back(hijo_der);
        io->escrituras++;

        int kpadre = btree[idxNodo].k;
        for (int j = kpadre; j > i; --j) {
            io->lecturas++;
            io->lecturas++;
            io->escrituras++;
            io->escrituras++;
            btree[idxNodo].llaves_valores[j] = btree[idxNodo].llaves_valores[j - 1];
            btree[idxNodo].hijos[j + 1] = btree[idxNodo].hijos[j];
        }
        btree[idxNodo].llaves_valores[i] = mediano;
        io->escrituras++;
        btree[idxNodo].hijos[i + 1] = idxDer;
        io->escrituras++;
        btree[idxNodo].k++;
        io->escrituras++;

        if (par.llave <= mediano.llave) {
            insertNonFull(btree, idxIzq, par, io);
        } else {
            insertNonFull(btree, idxDer, par, io);
        }
    } else {
        insertNonFull(btree, idxHijo, par, io);
    }
}

void insert(std::vector<Nodo>& btree, Llave_valor par, IOStats* io) {
    io->lecturas++;
    if (btree.empty()) {
        Nodo raiz;
        initNodo(raiz);
        btree.push_back(raiz);
        io->escrituras++;  // escritura al vector
    }
    int idxRaiz = 0;

    io->lecturas++;
    if (btree[idxRaiz].k < b) {
        insertNonFull(btree, idxRaiz, par, io);
    } else {
        // Si la raíz está llena, splitear la raíz y crear una nueva
        auto [nodo_izq, nodo_der, mediano] = split(btree[idxRaiz], io);
        io->lecturas++;  // lectura de la raíz

        int idxIzq = (int)btree.size();
        btree.push_back(nodo_izq);
        io->escrituras++;  // escritura del hijo izquierdo
        int idxDer = (int)btree.size();
        btree.push_back(nodo_der);
        io->escrituras++;  // escritura del hijo derecho

        Nodo nueva_raiz;
        initNodo(nueva_raiz);
        nueva_raiz.es_interno = 1;
        io->escrituras++;
        nueva_raiz.k = 1;
        io->escrituras++;
        nueva_raiz.llaves_valores[0] = mediano;
        io->escrituras++;
        nueva_raiz.hijos[0] = idxIzq;
        io->escrituras++;
        nueva_raiz.hijos[1] = idxDer;
        io->escrituras++;

        // reemplazar la raíz antigua en la posición 0
        btree[idxRaiz] = nueva_raiz;
        io->escrituras++;

        // insertar en el hijo correcto
        io->lecturas++;
        if (par.llave <= mediano.llave)
            insertNonFull(btree, idxIzq, par, io);
        else
            insertNonFull(btree, idxDer, par, io);
    }
}

std::vector<Nodo> crearBtree(std::ifstream& archivo, int N, IOStats* io) {
    std::vector<Nodo> btree;

    // inicializamos el árbol con un nodo raíz vacío
    Nodo raiz;
    raiz.k = 0;
    io->escrituras++;
    raiz.es_interno = 0;
    io->escrituras++;
    btree.push_back(raiz);
    io->escrituras++;

    int contador = 0;
    Llave_valor lv;

    // leer N pares llave-valor desde el archivo
    while (contador < N && archivo.read(reinterpret_cast<char*>(&lv), sizeof(Llave_valor))) {
        insert(btree, lv, io);  // ya cuenta lecturas/escrituras dentro
        contador++;
    }
    return btree;
}

void escribirBTreeADisco(const std::vector<Nodo>& btree, const std::string& nombreArchivo, IOStats* io) {
    std::ofstream archivo(nombreArchivo, std::ios::binary | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir archivo para escritura: " << nombreArchivo << "\n";
        return;
    }

    for (const Nodo& nodo : btree) {
        if (io != nullptr) {
            io->lecturas++;  // leer nodo antes de escribir
        }
        archivo.write(reinterpret_cast<const char*>(&nodo), sizeof(Nodo));
    }

    archivo.close();
}

Nodo leerNodo(std::ifstream& archivo, int idxNodo, IOStats* io) {
    Nodo nodo;
    archivo.seekg(idxNodo * sizeof(Nodo), std::ios::beg);
    archivo.read(reinterpret_cast<char*>(&nodo), sizeof(Nodo));
    io->escrituras++;  // escritura al variable nodo en RAM
    return nodo;
}

// Función auxiliar recursiva con conteo de IO
void rangeSearchRec(std::ifstream& archivo, int idxNodo, int l, int u, std::vector<Llave_valor>& resultado, IOStats* io) {
    Nodo nodo = leerNodo(archivo, idxNodo, io);  // ya cuenta io.escrituras++

    io->lecturas++;          // leer nodo.es_interno
    if (!nodo.es_interno) {  // hoja
        for (int i = 0; i < nodo.k; i++) {
            io->lecturas++;  // leer llaves_valores[i].llave
            if (nodo.llaves_valores[i].llave >= l && nodo.llaves_valores[i].llave <= u) {
                resultado.push_back(nodo.llaves_valores[i]);
                io->escrituras++;  // agregar a resultado
            }
        }
        return;
    }

    // Nodo interno → recorrer hijos relevantes
    for (int j = 0; j <= nodo.k; j++) {
        io->lecturas++;  // leer llaves_valores de comparación
        if (j == 0) {
            io->lecturas++;
            if (l <= nodo.llaves_valores[0].llave)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado, io);
            io->lecturas++;  // leer hijo
        } else if (j == nodo.k) {
            io->lecturas++;
            if (u >= nodo.llaves_valores[j - 1].llave)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado, io);
            io->lecturas++;
        } else {
            io->lecturas++;
            io->lecturas++;
            if (nodo.llaves_valores[j - 1].llave <= u && nodo.llaves_valores[j].llave >= l)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado, io);
            io->lecturas++;
        }
    }

    // Revisar llaves del nodo actual también
    for (int k = 0; k < nodo.k; k++) {
        io->lecturas++;
        if (nodo.llaves_valores[k].llave >= l && nodo.llaves_valores[k].llave <= u) {
            resultado.push_back(nodo.llaves_valores[k]);
            io->escrituras++;
        }
    }
}

// Función principal de búsqueda por rango con conteo de IO
std::vector<Llave_valor> rangeSearch(const std::string& nombreArchivo, int l, int u, IOStats* io) {
    std::vector<Llave_valor> resultado;
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo " << nombreArchivo << "\n";
        return resultado;
    }

    rangeSearchRec(archivo, 0, l, u, resultado, io);

    archivo.close();
    return resultado;
}

/*
int main() {
    std::ifstream archivo("datos.bin", std::ios::binary);
    if (!archivo) {
        std::cerr << "Error: no se pudo abrir datos.bin\n";
        return 1;
    }

    int N = pow(2,16);

    std::vector<Nodo> btree = crearBtree(archivo, N);

    // Mostrar lo leído
    for (size_t i = 0; i < btree.size(); i++) {
        std::cout << "Nodo " << i << " con " << btree[i].k << " llaves:\n";
    }
    archivo.close();

    escribirBTreeADisco(btree, "btree_disco");

    std::cout << "cantidad de lecturas hechas durante la creación del btree: " <<io.lecturas << std::endl;
    std::cout << "cantidad de escrituras hechas durante la búsqueda del btree: " <<io.escrituras << std::endl;

    //reseteo de las variables globales
    io.escrituras = 0;
    io.lecturas = 0;
     // Rango a buscar
    int l = 1546300800, u = l+ 604800;

    std::vector<Llave_valor> resultado = rangeSearch("btree_disco", l, u);

    int contador = 0;
    std::cout << "Resultados de la búsqueda en rango [" << l << ", " << u << "]:" << std::endl;
    for (auto &lv : resultado) {
        std::cout << "llave=" << lv.llave << ", valor=" << lv.valor << std::endl;
        contador++;
    }
    std::cout << "cantidad de elementos encontrados"<< contador << std::endl;

    std::cout << "cantidad de lecturas hechas durante la búsqueda de rango: " <<io.lecturas << std::endl;
    std::cout << "cantidad de escrituras hechas durante la búsqueda de rango: " <<io.escrituras << std::endl;
    return 0;
}
    */