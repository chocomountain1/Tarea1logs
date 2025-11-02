#include "trie.h"

#include <cstdlib>
#include <iostream>

Trie::Trie() {
    root = new TNode();
    mem_usage = 1;
};

Trie::~Trie() {
    free_node(root);
}

void Trie::free_node(TNode* node) {
    if (node) {
        for (int i = 0; i < SIGMA_SIZE; i++) {
            if (node->next[i]) free_node(node->next[i]);
        }
        delete node;
    }
}

long long Trie::get_mem_usage() { return mem_usage; }
TNode* Trie::get_root() { return root; }

int char_to_index(char c) {
    if (c == END_CHAR) return 26;
    if (c >= 'a' && c <= 'z') return c - 'a';  // truquito pss
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return -1;
}

char index_to_char(int i) {
    if (i >= 0 && i < 26) return 'a' + i;
    if (i == 26) return END_CHAR;
    return '\0';  // índice inválido
}

void Trie::insert(std::string w) {
    TNode* current = root;
    for (std::string::size_type i = 0; i < w.size(); ++i) {
        int index = char_to_index(w[i]);
        TNode* next = current->next[index];
        if (!next) {  // debemos crear un nuevo nodo
            next = new TNode();
            next->parent = current;
            current->next[index] = next;
            if (w[i] == END_CHAR) next->str = w; // si es terminal guardamos la string
            mem_usage += 1;
        }
        current = next;
    }
}

TNode* Trie::descend(TNode* v, char c) {
    return v->next[char_to_index(c)];
}