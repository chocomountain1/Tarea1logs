#include <math.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>

#include "Arbol_B+/bplus.h"
#include "Arbol_B/btree.h"
#include "Arbol_B/estructura_nodo.h"

int main() {
    std::ifstream archivo("datos.bin", std::ios::binary);
    if (!archivo) {
        std::cerr << "Error: no se pudo abrir datos.bin\n";
        return 1;
    }

    std::ofstream out;
    out.open("data_out.txt");

    std::string columns[15] = {"n",
                               "b_creation_time", "bp_creation_time",
                               "b_creation_read", "b_creation_write", "bp_creation_read", "bp_creation_write",
                               "b_size", "bp_size",
                               "b_avg_search_time", "bp_avg_search_time",
                               "b_avg_search_read", "b_avg_search_write", "bp_avg_search_read", "bp_avg_search_write"};

    for (int i = 0; i < 15; i++) {
        out << columns[i];
        if (i < 14) {
            out << ",";
        }
    }
    out << "\n";
    int l_min = 1546300800;
    int l_max = 1754006400;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(l_min, l_max);  // distribución aleatoria para l

    // rango para probar con distintos N
    int l_search_test = distrib(gen);
    int u_search_test = l_search_test + 604800;

    for (int i = 16; i < 27; i++) {
        std::cout << "Resultados con n:" << i << std::endl;
        int N = pow(2, i);

        std::chrono::steady_clock::time_point begin;  // para ir registrando delta t
        std::chrono::steady_clock::time_point end;

        // ------- datos pedidos por enunciado -------
        int btree_creation_time = 0;
        int bptree_creation_time = 0;

        IOStats btree_io_creation = {0, 0};
        IOStats bptree_io_creation = {0, 0};

        int btree_size = 0;
        int bptree_size = 0;

        int btree_avg_search_time = 0;
        int bptree_avg_search_time = 0;

        IOStats btree_avg_io_search = {0, 0};
        IOStats bptree_avg_io_search = {0, 0};

        // ------- B TREE insert -------

        IOStats btree_io = {0, 0};  // contabiliza ios totales

        archivo.seekg(0);  // puntero al comienzo del archivo
        begin = std::chrono::steady_clock::now();
        std::vector<Nodo> btree = crearBtree(archivo, N, &btree_io);  // hace una copia?
        end = std::chrono::steady_clock::now();

        btree_creation_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        btree_io_creation.escrituras = btree_io.escrituras;
        btree_io_creation.lecturas = btree_io.lecturas;
        btree_size = btree.size();

        // ------- B+ TREE insert -------
        BPlusTree bp_tree;
        int contador = 0;
        Llave_valor lv;
        // leer N pares llave-valor desde el archivo
        archivo.seekg(0);
        begin = std::chrono::steady_clock::now();
        while (contador < N && archivo.read(reinterpret_cast<char*>(&lv), sizeof(Llave_valor))) {
            bp_tree.insert(lv.llave, lv.valor);
            contador++;
        }
        end = std::chrono::steady_clock::now();

        bptree_creation_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        bptree_io_creation.escrituras = bp_tree.io_writes();

        bptree_io_creation.lecturas = bp_tree.io_reads();
        bptree_size = bp_tree.node_count();

        // ------ Write trees to file ------

        std::string btree_file = "data/btree_" + std::to_string(i + 1) + ".bin";
        std::string bptree_file = "data/bptree_" + std::to_string(i + 1) + ".bin";

        escribirBTreeADisco(btree, btree_file, nullptr);  // no nos interesa contabilizar IOs
        bp_tree.save_to_file(bptree_file);

        //  ------- SEARCH  -------
        // busqueda con dist. uniforme en rango l_min, l_max
        long long bp_ios = 0;
        for (int k = 0; k < 50; k++) {
            int l = distrib(gen);
            int u = l + 604800;
            // b
            begin = std::chrono::steady_clock::now();
            rangeSearch(btree_file, l, u, &btree_avg_io_search);
            end = std::chrono::steady_clock::now();
            btree_avg_search_time += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            // b+
            begin = std::chrono::steady_clock::now();
            bp_tree.range_query_from_file(bptree_file, l, u, &bp_ios);
            end = std::chrono::steady_clock::now();
            bptree_avg_io_search.lecturas += bp_ios;
            bptree_avg_search_time += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        }
        btree_avg_search_time = btree_avg_search_time / 50;
        bptree_avg_search_time = bptree_avg_search_time / 50;

        btree_avg_io_search.escrituras = btree_avg_io_search.escrituras / 50;
        btree_avg_io_search.lecturas = btree_avg_io_search.lecturas / 50;

        bptree_avg_io_search.lecturas = bptree_avg_io_search.lecturas / 50;

        if (i == 16) {
            std::ofstream search_out;
            search_out.open("search_data_16.txt");
            search_out << "llave,valor\n";
            std::vector<Llave_valor> res = rangeSearch(btree_file, l_search_test, u_search_test, &btree_avg_io_search);
            for (int k = 0; k < res.size(); k++) {
                search_out << res[k].llave << "," << res[k].valor << "\n";
            }
            search_out.close();
        }
        if (i == 22) {
            std::ofstream search_out;
            search_out.open("search_data_22.txt");
            search_out << "llave,valor\n";
            std::vector<Llave_valor> res = rangeSearch(btree_file, l_search_test, u_search_test, &btree_avg_io_search);
            for (int k = 0; k < res.size(); k++) {
                search_out << res[k].llave << "," << res[k].valor << "\n";
            }
            search_out.close();
        }
        /* {"i",
         "b_creation_time", "bp_creation_time",
         "b_creation_read", "b_creation_write", "bp_creation_read", "bp_creation_write",
         "b_size", "bp_size",
         "b_avg_search_time", "bp_avg_search_time",
         "b_avg_search_read", "b_avg_search_write", "bp_avg_search_read", "bp_avg_search_write"}; */
        out << i << ",";
        out << btree_creation_time << "," << bptree_creation_time << ",";
        out << btree_io_creation.lecturas << "," << btree_io_creation.escrituras << ",";
        out << bptree_io_creation.lecturas << "," << bptree_io_creation.escrituras << ",";
        out << btree_size << "," << bptree_size << ",";
        out << btree_avg_search_time << "," << bptree_avg_search_time << ",";
        out << btree_avg_io_search.lecturas << "," << btree_avg_io_search.escrituras << ",";
        out << bptree_avg_io_search.lecturas << "," << bptree_avg_io_search.escrituras;
        out << "\n";
    }

    archivo.close();
    out.close();

    // Rango a buscar
    /*
    int l = 1546300800, u = l+ 604800;

    std::vector<Llave_valor> resultado = rangeSearch("btree_disco", l, u);

    int contador = 0;
    std::cout << "Resultados de la búsqueda en rango [" << l << ", " << u << "]:" << std::endl;
    for (auto &lv : resultado) {
       std::cout << "llave=" << lv.llave << ", valor=" << lv.valor << std::endl;
       contador++;
    }
    std::cout << contador << std::endl;
    */
    return 0;
}