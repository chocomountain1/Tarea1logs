#include <string>

const int SIGMA_SIZE = 27;
const char END_CHAR = '$';  // Indica terminacion de string, $ no pertenece al alfabeto
int char_to_index(char c);
char index_to_char(int i);

struct TNode {
    TNode* parent;
    TNode* next[SIGMA_SIZE];
    int priority;
    std::string str;
    TNode* best_terminal;
    int best_priority;

    TNode() {
        parent = nullptr;
        for (int i = 0; i < SIGMA_SIZE; ++i) next[i] = nullptr;
        priority = 0;
        best_terminal = nullptr;
        best_priority = 0;
        str = "";
    }
};

class Trie {
   public:
    Trie();
    ~Trie();
    void insert(std::string w);
    TNode* descend(TNode* v, char c);
    TNode* autocomplete(TNode* v);
    void update_priority(TNode* v);

    TNode* get_root();
    long long get_mem_usage();

   private:
    void free_node(TNode* node);
    long long mem_usage;
    TNode* root;
};