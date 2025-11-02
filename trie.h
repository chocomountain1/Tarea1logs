const int SIGMA_SIZE = 27;
const char END_CHAR = '$';  // Indica terminacion de string, $ no pertenece al alfabeto

struct TNode {
    TNode* parent;
    TNode* next[SIGMA_SIZE];
    int priority;
    char* str;
    TNode* best_terminal;
    int best_priority;
};

class Trie {
   public:
    Trie();
    void insert(char* w);
    TNode* descend(TNode* v, char c);
    TNode* autocomplete(TNode* v);
    void update_priority(TNode* v);

    TNode* get_root();
    int get_mem_usage();

   private:
    long long mem_usage;
    TNode* root;
};