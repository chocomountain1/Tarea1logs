#include <iostream>
#include <vector>
#include <fstream>
#include "Estructura_nodo.cpp"
#include <tuple>
#include <math.h>


//Función para inicializar un nodo (debido a que constantemente se utiliza el crear uno nuevo con campos predeterminados)
void initNodo(Nodo &n) {
    n.k = 0;
    n.es_interno = 0;
    n.siguiente = -1; //Como estamos en los métodos que corresponden a un Btree este campo se deja en -1 por defecto
    for (int i = 0; i < b; ++i) { //Vamos a hacer que los pares llave-valor sean todos inicializados en (0,0.0) para este nodo
        n.llaves_valores[i].llave = 0;
        n.llaves_valores[i].valor = 0.0f;
        n.hijos[i] = -1;
    }
    n.hijos[b] = -1; // b+1 hijos posibles los reservamos con índices -1
}


std::tuple<Nodo, Nodo, Llave_valor> split(const Nodo &nodo_lleno) {
    int k_total = nodo_lleno.k;
    int mid = k_total / 2; // índice de la mediana: mid, usaremos mid en vez de b/2 o k_total para evitar segfault
    Llave_valor mediano = nodo_lleno.llaves_valores[mid];

    Nodo nodo_izq, nodo_der;
    initNodo(nodo_izq);
    initNodo(nodo_der);
    nodo_izq.es_interno = nodo_lleno.es_interno;
    nodo_der.es_interno = nodo_lleno.es_interno;

    for (int i = 0; i < mid; ++i) {
        nodo_izq.llaves_valores[i] = nodo_lleno.llaves_valores[i]; //copiamos los pares a la izquierda del mediano
    }
    nodo_izq.k = mid;

    int right_count = 0;
    for (int i = mid + 1; i < k_total; ++i) {
        nodo_der.llaves_valores[right_count++] = nodo_lleno.llaves_valores[i];//copiamos los pares a la derecha en el nodo derecho
    }
    nodo_der.k = right_count;

    if (nodo_lleno.es_interno) { //guardar los hijos solo si el nodo es interno
        for (int i = 0; i <= mid; ++i) {
            nodo_izq.hijos[i] = nodo_lleno.hijos[i];
        }
        int idx = 0;
        for (int i = mid + 1; i <= k_total; ++i) {
            nodo_der.hijos[idx++] = nodo_lleno.hijos[i];
        }
    }

    return std::make_tuple(nodo_izq, nodo_der, mediano); //retornamos una triple tupla nodo_izq,nodo_der,mediano
}

void insertNonFull(std::vector<Nodo>& btree, int idxNodo, Llave_valor par) {
    int i = btree[idxNodo].k - 1; //usamos un contador que parte al final del btree

    if (!btree[idxNodo].es_interno) {
        //el nodo era una hoja
        while (i >= 0 && btree[idxNodo].llaves_valores[i].llave > par.llave) { //Insertar el par en 𝐻, de tal forma que los pares queden ordenados según sus llaves
            btree[idxNodo].llaves_valores[i + 1] = btree[idxNodo].llaves_valores[i];
            --i;
        }
        btree[idxNodo].llaves_valores[i + 1] = par;
        btree[idxNodo].k++;
        return;
    }

    //si no, es un nodo interno
    while (i >= 0 && btree[idxNodo].llaves_valores[i].llave > par.llave) --i; //Usando las llaves del nodo, encontrar el hijo 𝑈 en el cual insertar el nuevo par
    ++i; // hijo a bajar

    int idxHijo = btree[idxNodo].hijos[i];

    // Si el hijo está lleno (hijo.k == max permitido), hacer split.
    if (btree[idxHijo].k >= b) {
        // Hacemos split del hijo actual 
        auto [hijo_izq, hijo_der, mediano] = split(btree[idxHijo]);

        // Reemplazamos el hijo viejo por hijo_izq y empujamos hijo_der al final.
        int idxIzq = idxHijo;
        int idxDer = (int)btree.size(); // hijo der al final

        btree[idxIzq] = hijo_izq;     
        btree.push_back(hijo_der);     

        int kpadre = btree[idxNodo].k;
        // mover llaves y punteros en el padre hacia la derecha para hacer espacio en i
        for (int j = kpadre; j > i; --j) {
            btree[idxNodo].llaves_valores[j] = btree[idxNodo].llaves_valores[j - 1];
            btree[idxNodo].hijos[j + 1] = btree[idxNodo].hijos[j];
        }
        btree[idxNodo].llaves_valores[i] = mediano;
        btree[idxNodo].hijos[i + 1] = idxDer;
        btree[idxNodo].k++;

        if (par.llave <= mediano.llave) {
            insertNonFull(btree, idxIzq, par);
        } else {
            insertNonFull(btree, idxDer, par);
        }
    } else {
        // si el hijo no está lleno insertar en el hijo
        insertNonFull(btree, idxHijo, par);
    }
}

void insert(std::vector<Nodo>& btree, Llave_valor par) {
    if (btree.empty()) {
        Nodo raiz; initNodo(raiz);
        btree.push_back(raiz);
    }
    int idxRaiz = 0;

    if (btree[idxRaiz].k < b) {
        insertNonFull(btree, idxRaiz, par);
    } else {
        //si la raiz esta lllena splitear la raiz y crear una nueva
        auto [nodo_izq, nodo_der, mediano] = split(btree[idxRaiz]);

        int idxIzq = (int)btree.size();
        btree.push_back(nodo_izq);
        int idxDer = (int)btree.size();
        btree.push_back(nodo_der);

        Nodo nueva_raiz; initNodo(nueva_raiz);
        nueva_raiz.es_interno = 1;
        nueva_raiz.k = 1;
        nueva_raiz.llaves_valores[0] = mediano;
        nueva_raiz.hijos[0] = idxIzq;
        nueva_raiz.hijos[1] = idxDer;

        // reemplazar la raíz antigua en la posición 0
        btree[idxRaiz] = nueva_raiz;

        // insertar en el hijo correcto
        if (par.llave <= mediano.llave) insertNonFull(btree, idxIzq, par);
        else insertNonFull(btree, idxDer, par);
    }
}


std::vector<Nodo> crearBtree(std::ifstream &archivo, int N) {
    std::vector<Nodo> btree;

    // inicializamos el árbol con un nodo raíz vacío
    Nodo raiz;
    raiz.k = 0;
    raiz.es_interno = 0;
    btree.push_back(raiz);

    int contador = 0;
    Llave_valor lv;
    
    // leer N pares llave-valor desde el archivo
    while (contador < N && archivo.read(reinterpret_cast<char*>(&lv), sizeof(Llave_valor))) {
        insert(btree, lv);  
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

//Función auxiliar que es recursiva pero ignora el caso en donde es raiz
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

//Función de buscar por rango que sí empieza por la raíz y utiliza rangeSearchRec para buscar en la profundidad del arbol
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

    int N = pow(2,16); 

    std::vector<Nodo> btree = crearBtree(archivo, N);

    // Mostrar lo leído
    for (size_t i = 0; i < btree.size(); i++) {
        std::cout << "Nodo " << i << " con " << btree[i].k << " llaves:\n";
    }
    archivo.close();
    
    escribirBTreeADisco(btree, "btree_disco");


     // Rango a buscar
    int l = 1546300800, u = l+ 604800;

    std::vector<Llave_valor> resultado = rangeSearch("btree_disco", l, u);

    int contador = 0;
    std::cout << "Resultados de la búsqueda en rango [" << l << ", " << u << "]:" << std::endl;
    for (auto &lv : resultado) {
        std::cout << "llave=" << lv.llave << ", valor=" << lv.valor << std::endl;
        contador++;
    }
    std::cout << contador << std::endl;
    return 0;
}