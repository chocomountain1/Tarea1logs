#include <bits/stdc++.h>
#include "bplus.h"
using namespace std;

int main() {
    BPlusTree T;

    // Inserta una secuencia que fuerce varios splits de hojas e internos.
    // (ajusta M si quieres forzar más splits)
    const int M = 3000;
    for (int i = 0; i < M; ++i)
        T.insert(1000 + i * 2, float(i));   // claves pares

    for (int i = 0; i < M; i += 5)
        T.insert(1001 + i * 2, float(i));   // intercalar algunas impares para reordenar

    // 1) Rango completo
    auto all = T.range_query(INT_MIN, INT_MAX);
    if (!is_sorted(all.begin(), all.end(), [](auto &a, auto &b){ return a.key <= b.key; })) {
        cerr << "FAIL: range_query no devuelve ordenado\n"; return 1;
    }

    // 2) Ventana que cae en una sola hoja
    auto mid = T.range_query(5000, 5100);
    for (size_t i = 1; i < mid.size(); ++i)
        if (mid[i-1].key > mid[i].key) { cerr << "FAIL: ventana desordenada\n"; return 1; }

    // 3) Ventana que cruza varias hojas (prueba 'siguiente')
    auto wide = T.range_query(1000, 20000);
    if (wide.empty()) { cerr << "FAIL: ventana amplia vacía\n"; return 1; }
    for (size_t i = 1; i < wide.size(); ++i)
        if (wide[i-1].key > wide[i].key) { cerr << "FAIL: recorrido de hojas roto\n"; return 1; }

    // 4) guardar -> consultar desde archivo
    if (!T.save_to_file("BP.bin")) { cerr << "FAIL: save_to_file\n"; return 1; }
    long long ioR = 0;
    auto disk = T.range_query_from_file("BP.bin", 5000, 5100, &ioR);
    if (disk.size() != mid.size()) { cerr << "FAIL: RAM vs disco (tamaño)\n"; return 1; }
    for (size_t i = 0; i < mid.size(); ++i)
        if (disk[i].key != mid[i].key || fabs(disk[i].value - mid[i].value) > 1e-6f) {
            cerr << "FAIL: RAM vs disco (contenido)\n"; return 1;
        }

    cout << "Test completado\n";
    return 0;
}
