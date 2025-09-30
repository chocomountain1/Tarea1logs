#include <iostream>
#include <vector>
#include <fstream>
#include "Estructura_nodo.cpp"
#include <tuple>


std::tuple<Nodo,Nodo,Llave_valor> split(Nodo nodo_lleno){
    Llave_valor mediano = nodo_lleno.llaves_valores[b/2];

    Nodo nodo_izq;
    nodo_izq.es_interno = nodo_lleno.es_interno;
    nodo_izq.k = 0;

    Nodo nodo_der;
    nodo_der.es_interno = nodo_lleno.es_interno;
    nodo_der.k = 0;

    // Copiar claves
    for(int i=0; i<b/2; i++){
        nodo_izq.llaves_valores[i] = nodo_lleno.llaves_valores[i];
    }
    nodo_izq.k = b/2;

    for(int i=b/2+1; i<b; i++){
        nodo_der.llaves_valores[i - (b/2+1)] = nodo_lleno.llaves_valores[i];
    }
    nodo_der.k = b - (b/2 + 1);

    // Copiar hijos si es internocd 
    if(nodo_lleno.es_interno){
        for(int i=0; i<= b/2; i++){
            nodo_izq.hijos[i] = nodo_lleno.hijos[i];
        }
        for(int i=b/2+1; i<= b; i++){
            nodo_der.hijos[i - (b/2+1)] = nodo_lleno.hijos[i];
        }
    }

    return std::make_tuple(nodo_izq, nodo_der, mediano);
}

// Inserta un par en un nodo no lleno
void insertNonFull(Nodo& nodo, Llave_valor par, std::vector<Nodo>& btree){
    int i = nodo.k - 1;

    if (!nodo.es_interno) {
        // Caso hoja: insertar directamente manteniendo orden
        while (i >= 0 && nodo.llaves_valores[i].llave > par.llave) {
            nodo.llaves_valores[i + 1] = nodo.llaves_valores[i];
            i--;
        }
        nodo.llaves_valores[i + 1] = par;
        nodo.k++;
    } else {
        // Caso nodo interno: buscar el hijo adecuado
        while (i >= 0 && nodo.llaves_valores[i].llave > par.llave) {
            i--;
        }
        i++; // ahora i es el índice del hijo donde debe ir el par

        Nodo& hijo = btree[nodo.hijos[i]];

        if (hijo.k == b) {
            // Split del hijo lleno
            auto [hijo_izq, hijo_der, mediano] = split(hijo);

            int idxIzq = nodo.hijos[i];   // reutilizamos la posición del hijo
            int idxDer = btree.size();
            btree[idxIzq] = hijo_izq;
            btree.push_back(hijo_der);

            // Insertar la mediana en el nodo padre
            for (int j = nodo.k; j > i; j--) {
                nodo.llaves_valores[j] = nodo.llaves_valores[j - 1];
                nodo.hijos[j + 1] = nodo.hijos[j];
            }
            nodo.llaves_valores[i] = mediano;
            nodo.hijos[i + 1] = idxDer;
            nodo.k++;

            // Elegir a cuál hijo bajar
            if (par.llave <= mediano.llave) {
                insertNonFull(btree[idxIzq], par, btree);
            } else {
                insertNonFull(btree[idxDer], par, btree);
            }
        } else {
            // Si el hijo no está lleno, insertamos ahí
            insertNonFull(hijo, par, btree);
        }
    }
}


void insert(std::vector<Nodo>& btree, Llave_valor par) {
    Nodo& raiz = btree[0];

    if (raiz.k < b) {
        // Si la raíz no está llena, insertar directamente
        insertNonFull(raiz, par,btree);
    } else {
        // Si la raíz está llena, hacer split
        auto [nodo_izq, nodo_der, mediano] = split(raiz);

        int idxIzq = btree.size();
        btree.push_back(nodo_izq);

        int idxDer = btree.size();
        btree.push_back(nodo_der);

        Nodo nueva_raiz;
        nueva_raiz.es_interno = 1;
        nueva_raiz.k = 1;
        nueva_raiz.llaves_valores[0] = mediano;
        nueva_raiz.hijos[0] = idxIzq;
        nueva_raiz.hijos[1] = idxDer;

        // Reemplazar la raíz antigua
        btree[0] = nueva_raiz;

        // Insertar en el hijo correcto
        if (par.llave <= mediano.llave) {
            insertNonFull(btree[idxIzq], par,btree);
        } else {
            insertNonFull(btree[idxDer], par,btree);
        }
    }
}

std::vector<Nodo> crearBtree(std::ifstream &archivo, int N) {
    std::vector<Nodo> btree;

    // Inicializamos el árbol con un nodo raíz vacío
    Nodo raiz;
    raiz.k = 0;
    raiz.es_interno = 0;
    btree.push_back(raiz);

    int contador = 0;
    Llave_valor lv;

    // Leer N pares llave-valor desde el archivo
    while (contador < N && archivo.read(reinterpret_cast<char*>(&lv), sizeof(Llave_valor))) {
        insert(btree, lv);  // 🔹 Aquí usamos la lógica de inserción que maneja splits, raíces, etc.
        contador++;
    }

    return btree;
}

void escribirBTreeADisco(const std::vector<Nodo> &btree, const std::string &nombreArchivo) {
    std::ofstream archivo(nombreArchivo, std::ios::binary | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir archivo para escritura: " << nombreArchivo << "\n";
        return;
    }

    for (const Nodo &nodo : btree) {
        archivo.write(reinterpret_cast<const char*>(&nodo), sizeof(Nodo));
    }

    archivo.close();
}

Nodo leerNodo(std::ifstream &archivo, int idxNodo) {
    Nodo nodo;
    archivo.seekg(idxNodo * sizeof(Nodo), std::ios::beg);
    archivo.read(reinterpret_cast<char*>(&nodo), sizeof(Nodo));
    return nodo;
}

// Función recursiva usando archivo
void rangeSearchRec(std::ifstream &archivo, int idxNodo, int l, int u, std::vector<Llave_valor> &resultado) {
    Nodo nodo = leerNodo(archivo, idxNodo);

    // Caso: hoja
    if (!nodo.es_interno) {
        for (int i = 0; i < nodo.k; i++) {
            if (nodo.llaves_valores[i].llave >= l && nodo.llaves_valores[i].llave <= u) {
                resultado.push_back(nodo.llaves_valores[i]);
            }
        }
        return;
    }

    // Caso: nodo interno → recorrer hijos relevantes
    for (int j = 0; j <= nodo.k; j++) {
        if (j == 0) {
            if (l <= nodo.llaves_valores[0].llave)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado);
        } else if (j == nodo.k) {
            if (u >= nodo.llaves_valores[j - 1].llave)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado);
        } else {
            if (nodo.llaves_valores[j - 1].llave <= u && nodo.llaves_valores[j].llave >= l)
                rangeSearchRec(archivo, nodo.hijos[j], l, u, resultado);
        }
    }

    // Revisar las llaves del nodo actual también
    for (int k = 0; k < nodo.k; k++) {
        if (nodo.llaves_valores[k].llave >= l && nodo.llaves_valores[k].llave <= u) {
            resultado.push_back(nodo.llaves_valores[k]);
        }
    }
}

// Función principal
std::vector<Llave_valor> rangeSearch(const std::string &nombreArchivo, int l, int u) {
    std::vector<Llave_valor> resultado;
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo " << nombreArchivo << "\n";
        return resultado;
    }

    // empezamos desde la raíz (nodo 0)
    rangeSearchRec(archivo, 0, l, u, resultado);

    archivo.close();
    return resultado;
}



int main() {
    std::ifstream archivo("datos.bin", std::ios::binary);
    if (!archivo) {
        std::cerr << "Error: no se pudo abrir datos.bin\n";
        return 1;
    }

    int N = 350; // número de pares que esperas leer (ajusta según tu archivo)

    std::vector<Nodo> btree = crearBtree(archivo, N);

    // Mostrar lo leído
    for (size_t i = 0; i < btree.size(); i++) {
        std::cout << "Nodo " << i << " con " << btree[i].k << " llaves:\n";
        for (int j = 0; j < btree[i].k; j++) {
            std::cout << "  llave=" << btree[i].llaves_valores[j].llave
                      << ", valor=" << btree[i].llaves_valores[j].valor << "\n";
        }
    }

    archivo.close();
    
    escribirBTreeADisco(btree, "btree_disco");


     // Rango a buscar
    int l = 6, u = 15;

    std::vector<Llave_valor> resultado = rangeSearch("btree_disco", l, u);

    std::cout << "Resultados de la búsqueda en rango [" << l << ", " << u << "]:" << std::endl;
    for (auto &lv : resultado) {
        std::cout << "llave=" << lv.llave << ", valor=" << lv.valor << std::endl;
    }

    return 0;
}