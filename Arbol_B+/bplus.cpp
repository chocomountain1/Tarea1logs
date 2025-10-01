#include "bplus.h"
#include <algorithm>
#include <fstream>
#include <cstring> 
#include <climits>

// Construccion
BPlusTree::BPlusTree() {
    BPNode r{};
    r.es_interno = 0;
    r.k = 0;
    r.siguiente = -1;
    for (int i = 0; i < BP_B + 1; ++i) r.hijos[i] = -1;
    append_node(r); // idx = 0 (raíz)
}

// Simulacino de accesos a disco 
const BPNode& BPlusTree::read_node(int idx) const { ++lecturas_; return disco_[idx]; }
void BPlusTree::write_node(int idx, const BPNode& n) { ++escrituras_; disco_[idx] = n; }
int  BPlusTree::append_node(const BPNode& n) { ++escrituras_; disco_.push_back(n); return (int)disco_.size()-1; }


int BPlusTree::lower_bound_in_node(const BPNode& v, int key) {
    int lo = 0, hi = v.k;
    while (lo < hi) {
        int mid = (lo + hi) >> 1;
        if (v.llaves_valores[mid].key < key) lo = mid + 1; else hi = mid;
    }
    return lo;
}

// hijos[i] guarda llaves en (key_{i-1}, key_i] 
int BPlusTree::child_index_for(const BPNode& v, int key) {
    return lower_bound_in_node(v, key);
}

// Split (B+)
// si se divide una HOJA, la mediana también queda en la hoja izquierda;
// además, enlaza el 'siguiente' entre hojas.
std::pair<BPKeyValue,int> BPlusTree::split_node_bplus(int idx) {
    BPNode v = read_node(idx);

    // v está lleno: v.k == BP_B
    const int left_count_leaf  = (BP_B / 2);     // en hoja: deja mediana también a la izq (cuenta BP_B/2)
    const int left_count_int   = (BP_B / 2) - 1; // en interno: típico B, izquierda BP_B/2 - 1
    const int right_count      = (BP_B / 2);     // derecha siempre BP_B/2
    const int mid_pos_for_B    = (BP_B / 2) - 1; // índice de la mediana “que sube” (igual que b tree)

    BPKeyValue median = v.llaves_valores[mid_pos_for_B];

    BPNode right{};
    right.es_interno = v.es_interno;
    right.k = right_count;
    right.siguiente = -1;
    for (int i = 0; i < BP_B + 1; ++i) right.hijos[i] = -1;

    if (v.es_interno == 0) {
        // Hoja: la mediana también queda en izquierda
        // Izquierda (idx): toma BP_B/2 elementos (hasta (BP_B/2)-1)
        // Derecha: los BP_B/2 elementos restantes (BP_B/2 en adelante)
        // Copiar derecha
        std::memcpy(right.llaves_valores,
                    v.llaves_valores + (BP_B / 2),
                    sizeof(BPKeyValue) * right_count);

        // Ajustar izquierda
        v.k = left_count_leaf;

        // Enlazar hojas siguiente
        right.siguiente = v.siguiente;
        int right_idx = append_node(right);
        v.siguiente = right_idx;

        write_node(idx, v);

        // La mediana que sube al padre debe ser la primer clave del nodo derecho
        // pero como guardamos pares completos en internos ( por simplificación del enunciado),
        // hacemos (right.llaves_valores[0].key, right.llaves_valores[0].value).
        BPKeyValue promote = right.llaves_valores[0];
        return {promote, right_idx};
    } else {
        // Interno: aqui es igual a un arbol B
        // Izquierda: queda con (BP_B/2 - 1) claves
        // Derecha: toma BP_B/2 claves desde (mid_pos_for_B + 1)
        std::memcpy(right.llaves_valores,
                    v.llaves_valores + (mid_pos_for_B + 1),
                    sizeof(BPKeyValue) * right_count);

        // Hijos: izquierda conserva los primeros BP_B/2 hijos
        // derecha toma los siguientes BP_B/2 + 1
        for (int i = 0; i < (BP_B / 2) + 1; ++i) {
            right.hijos[i] = v.hijos[(BP_B / 2) + i];
        }
        // Limpiar punteros sobrantes del izquierdo
        for (int i = (BP_B / 2); i < BP_B + 1; ++i) {
            v.hijos[i] = -1;
        }

        v.k = left_count_int;
        write_node(idx, v);
        int right_idx = append_node(right);
        // Promocionamos la mediana calculada (estaba en mid_pos_for_B)
        return {median, right_idx};
    }
}

// Inserción
void BPlusTree::insert(int key, float value) {
    BPNode root = read_node(0);
    if (root.k == BP_B) {
        // dividir raíz (puede ser hoja o interna)
        auto [median, right_idx] = split_node_bplus(0);

        // FIX raíz: mover el hijo izquierdo recortado fuera del índice 0 
        BPNode left_copy = read_node(0);
        int left_idx = append_node(left_copy);

        // Escribir la nueva raíz en el índice 0
        BPNode new_root{};
        new_root.es_interno = 1;
        new_root.k = 1;
        new_root.llaves_valores[0] = median;   // en caso de hoja, es la 1ª clave del derecho
        new_root.hijos[0] = left_idx;
        new_root.hijos[1] = right_idx;
        new_root.siguiente = -1;
        for (int i = 2; i < BP_B + 1; ++i) new_root.hijos[i] = -1;

        write_node(0, new_root);
    }
    insert_nonfull(0, key, value);
}

void BPlusTree::insert_nonfull(int idx, int key, float value) {
    BPNode v = read_node(idx);

    if (v.es_interno == 0) {
        // Hoja no llena: insertar ordenado (se asume que todos los datos son distintos)
       int pos = lower_bound_in_node(v, key);
        if (pos < v.k && v.llaves_valores[pos].key == key) {
            v.llaves_valores[pos].value = value;
            write_node(idx, v);
            return;
        }
        for (int i = v.k; i > pos; --i)
            v.llaves_valores[i] = v.llaves_valores[i-1];
        v.llaves_valores[pos] = {key, value};
        v.k += 1;
        write_node(idx, v);
        return;
    }

    // Interno: decidir hijo
    int i = child_index_for(v, key);
    int child_idx = v.hijos[i];
    if (child_idx < 0) return; 

    BPNode child = read_node(child_idx);
    bool child_was_leaf = (child.es_interno == 0);

    if (child.k == BP_B) {
        // split del hijo según reglas B+
        auto [promote, right_idx] = split_node_bplus(child_idx);

        // Insertar promote en v, con el nuevo puntero a la derecha
        for (int j = v.k; j > i; --j) {
            v.llaves_valores[j] = v.llaves_valores[j-1];
            v.hijos[j+1] = v.hijos[j];
        }
        v.llaves_valores[i] = promote;
        v.hijos[i+1] = right_idx;
        v.k += 1;
        write_node(idx, v);

        // Releer 'v' y decidir a qué hijo bajar según si el split fue de hoja o interno
        v = read_node(idx);
        if (child_was_leaf) {
            // En hoja B+: la clave promovida es la 1ª del derecho; usar >=
            i = (key >= promote.key) ? (i + 1) : i;
        } else {
            // En interno B: usar >
            i = (key > promote.key) ? (i + 1) : i;
        }
        child_idx = v.hijos[i];
    }

    insert_nonfull(child_idx, key, value);
}

// Bajar a hoja de l
int BPlusTree::find_leaf_for(int key) const {
    int idx = 0;
    while (true) {
        const BPNode& v = read_node(idx);
        if (v.es_interno == 0) return idx;
        int i = child_index_for(v, key);
        idx = v.hijos[i];
    }
}

// Range query (B+)
// 1) Bajar a la hoja donde caería l 2) Recorrer hojas via siguiente acumulando
// mientras key <= u. Detener cuando sobrepasemos u.
std::vector<BPKeyValue> BPlusTree::range_query(int l, int u) const {
    std::vector<BPKeyValue> out;
    if (disco_.empty()) return out;
    int leaf = find_leaf_for(l);
    range_from_leaf(leaf, l, u, out);
    return out;
}

void BPlusTree::range_from_leaf(int leaf_idx, int l, int u, std::vector<BPKeyValue>& out) const {
    int idx = leaf_idx;
    while (idx != -1) {
        const BPNode& v = read_node(idx);
        // recolectar pares en [l, u]
        for (int i = 0; i < v.k; ++i) {
            int key = v.llaves_valores[i].key;
            if (key >= l && key <= u) {
                out.push_back(v.llaves_valores[i]);
            } else if (key > u) {
                return; // nos pasamos
            }
        }
        idx = v.siguiente;
    }
}

// Serialización real
bool BPlusTree::save_to_file(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) return false;
    for (const auto& n : disco_) {
        out.write(reinterpret_cast<const char*>(&n), sizeof(BPNode));
        if (!out) return false;
    }
    return true;
}
bool BPlusTree::load_from_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;
    disco_.clear(); lecturas_ = escrituras_ = 0;
    BPNode tmp;
    while (in.read(reinterpret_cast<char*>(&tmp), sizeof(BPNode))) {
        disco_.push_back(tmp);
    }
    return !disco_.empty();
}

// 
int BPlusTree::lower_bound_in_node_file(const BPNode& v, int key) {
    int lo = 0, hi = v.k;
    while (lo < hi) {
        int mid = (lo + hi) >> 1;
        if (v.llaves_valores[mid].key < key) lo = mid + 1; else hi = mid;
    }
    return lo;
}
int BPlusTree::child_index_for_file(const BPNode& v, int key) {
    return lower_bound_in_node_file(v, key);
}
bool BPlusTree::read_node_file(std::ifstream& in, int idx, BPNode& n, long long* file_io_reads) {
    in.seekg(static_cast<std::streamoff>(idx) * static_cast<std::streamoff>(sizeof(BPNode)));
    if (!in.good()) return false;
    in.read(reinterpret_cast<char*>(&n), sizeof(BPNode));
    if (file_io_reads) (*file_io_reads)++;
    return in.good();
}

// Bajar a la hoja de l (desde archivo) 
int BPlusTree::find_leaf_for_file(std::ifstream& in, int key, long long* file_io_reads) const {
    int idx = 0;
    while (true) {
        BPNode v{};
        if (!read_node_file(in, idx, v, file_io_reads)) return -1;
        if (v.es_interno == 0) return idx;
        int i = child_index_for_file(v, key);
        idx = v.hijos[i];
        if (idx < 0) return -1;
    }
}

// Recorrer hojas enlazadas
void BPlusTree::range_from_leaf_file(std::ifstream& in, int leaf_idx, int l, int u, std::vector<BPKeyValue>& out, long long* file_io_reads) const {
    int idx = leaf_idx;
    while (idx != -1) {
        BPNode v{};
        if (!read_node_file(in, idx, v, file_io_reads)) return;
        for (int i = 0; i < v.k; ++i) {
            int key = v.llaves_valores[i].key;
            if (key >= l && key <= u) {
                out.push_back(v.llaves_valores[i]);
            } else if (key > u) {
                return; // cortar cuando nos pasamos de u
            }
        }
        idx = v.siguiente; // saltar a la hoja siguiente (B+)
    }
}

// Range query (desde archivo) 
std::vector<BPKeyValue> BPlusTree::range_query_from_file(const std::string& filename, int l, int u, long long* file_io_reads) const {
    std::vector<BPKeyValue> out;
    std::ifstream in(filename, std::ios::binary);
    if (!in) return out;
    if (file_io_reads) *file_io_reads = 0;

    // 1) Bajar hasta la hoja donde caería l
    // /*  */2) Recorrer 'siguiente' acumulando
    int leaf = find_leaf_for_file(in, l, file_io_reads);
    if (leaf >= 0) range_from_leaf_file(in, leaf, l, u, out, file_io_reads);
    return out;
}
