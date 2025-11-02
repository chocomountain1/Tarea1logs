# Compilador y flags
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

# Archivos fuente
TRIE_SRC := trie.cpp
TEST_SRC := test_trie.cpp

EXP     := experimentacion.cpp

# Binarios
TRIE_BIN  := trie
TEST_BIN   := trie_test
EXP_BIN := experimentacion

# Regla por defecto: compilar todo
all: $(TRIE_BIN) $(EXP_BIN)

# Ejecutables separados
$(TRIE_BIN): $(TRIE_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN): $(TRIE_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Atajos de ejecución
run: $(EXP_BIN)
	./$(EXP_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

# Limpiar binarios generados
clean:
	rm -f $(TEST_BIN) $(TRIE_BIN)  *.o