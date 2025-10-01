#pragma once
#include <vector>
#include <string>
#include <cstdint>

constexpr int BP_B = 340; // 4096 bytes por nodo

struct BPKeyValue {
    int   key;    
    float value;  
};

struct BPNode {
    int es_interno;                 // variable booleana; 1 si interno, 0 hoja
    int k;                          // cantidad de pares posibles 
    BPKeyValue llaves_valores[BP_B];
    int hijos[BP_B + 1];            // índices de hijos para nodos internos
    int siguiente;                  // índice de la próxima hoja (Esto diferencia B+ de los B normales) si no tiene siguiente es -1
};

static_assert(sizeof(BPNode) == 4096, "BPNode must be exactly 4096 bytes.");

class BPlusTree {
public:
    BPlusTree();

    // Inserción  (key, value)
    void insert(int key, float value);

    // Buscar en un rango [l, u] (desde archivo en RAM simulado)
    std::vector<BPKeyValue> range_query(int l, int u) const;

    // I/O simuladas
    long long io_reads()  const { return lecturas_;  }
    long long io_writes() const { return escrituras_;}

    // Serialización real a disco / carga
    bool save_to_file(const std::string& filename) const;
    bool load_from_file(const std::string& filename);

    //contar nodos
    size_t node_count() const { return disco_.size(); }

private:
    // “Disco” simulado
    mutable long long lecturas_  = 0;
    mutable long long escrituras_ = 0;
    std::vector<BPNode> disco_; // disco_[0] = raíz

    // Accesos con conteo I/O
    const BPNode& read_node(int idx) const;
    void write_node(int idx, const BPNode& n);
    int  append_node(const BPNode& n);

    // otros
    static int lower_bound_in_node(const BPNode& v, int key);
    static int child_index_for(const BPNode& v, int key);

    // Split B+:
    //  - Si es hoja: la mediana también queda en la hoja izquierda y se enlaza el siguiente
    //  - Si es interno: mediana sube y no queda en el izquierdo 
    // Retorna {median_promoted, idx_nuevo_derecho}
    std::pair<BPKeyValue,int> split_node_bplus(int idx);

    // Inserción recursiva; garantiza que el nodo idx no esté lleno antes de bajar
    void insert_nonfull(int idx, int key, float value);

    // Bajar hasta la hoja donde caería 'key'
    int find_leaf_for(int key) const;

    // Búsqueda por rango: bajar hoja y recorrer el siguiente
    void range_from_leaf(int leaf_idx, int l, int u, std::vector<BPKeyValue>& out) const;

    public:
    std::vector<BPKeyValue> range_query_from_file(const std::string& filename, int l, int u, long long* file_io_reads = nullptr) const;

private:
    // 
    static int lower_bound_in_node_file(const BPNode& v, int key);
    static int child_index_for_file(const BPNode& v, int key);
    static bool read_node_file(std::ifstream& in, int idx, BPNode& n, long long* file_io_reads);
    int  find_leaf_for_file(std::ifstream& in, int key, long long* file_io_reads) const;
    void range_from_leaf_file(std::ifstream& in, int leaf_idx, int l, int u, std::vector<BPKeyValue>& out, long long* file_io_reads) const;

};