#include <iostream>
#include <string>

#include "trie.h"


// Codigo para imprimir un trie, implementado a partir de la siguiente solucion para 
// imprimir un arbol binario. El unico uso es para verificar la correcta estructura del trie.
// https://stackoverflow.com/a/51730733/23398614
void printTrie(const std::string& prefix, TNode* node, bool isLast, char c) {
    if (node != nullptr) {
        std::cout << prefix;

        std::cout << (isLast ? "└──" : "├──");

        // print the value of the node
        std::cout << c << std::endl;

        // enter the next tree level
        // buscamos el ultimo
        int last = -1;
        for (int i = SIGMA_SIZE - 1; i >= 0; i--) {
            TNode* n = node->next[i];
            if (n) {
                last = i;
                break;
            }
        }
        // imprimimos
        for (int i = 0; i < SIGMA_SIZE; i++) {
            TNode* n = node->next[i];
            if (n) {
                printTrie(prefix + (isLast ? "   " : "|   "), n, i == last, index_to_char(i));
            }
        }
    }
}

int main() {
    std::cout << "=== Test del Trie ===" << std::endl;
    Trie trie;

    std::string words[4] = {"abc$",    // insert una palabra
                            "abb$",    // insert palabra a rama de al lado
                            "xy$",     // insert a otra rama de root
                            "abcd$"};  // insert palabra más larga

    /*           <root>             <- así se deberia ver el trie
                 /     \
                a       x
                |       |
                b       y
               / \      |
              b   c     $
              |   /\
              $  d  $
                 |
                 $
    */
    std::cout << "--- Insertando:" << std::endl;

    for (int i = 0; i < 4; i++) {
        trie.insert(words[i]);
    }
    std::cout << "--- Buscando palabras:" << std::endl;

    // test usar descend hasta encontrar el caracter terminal
    for (int i = 0; i < 4; i++) {
        std::string w = words[i];
        TNode* n = trie.get_root();
        for (std::string::size_type j = 0; j < w.size(); ++j) {
            n = trie.descend(n, w[j]);
        }
        std::cout << "Esperado/Obtenido " << w << "/" << n->str << std::endl;
    }
    std::cout << "--- Trie ---:" << std::endl;

    printTrie("", trie.get_root(), true, '.');
}
