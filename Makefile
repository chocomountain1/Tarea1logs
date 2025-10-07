# Compilador y flags
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

# Archivos fuente
BP_SRC  := Arbol_B+/bplus.cpp
B_SRC   := Arbol_B/Metodos_Btree.cpp
BP_TEST := Arbol_B+/testbplus.cpp
B_TEST  := Arbol_B/prueba.cpp
EXP     := experimentacion.cpp

# Binarios
BP_BIN  := testbplus
B_BIN   := testbtree
EXP_BIN := experimentacion

# Regla por defecto: compilar todo
all: $(BP_BIN) $(B_BIN) $(EXP_BIN)

# Ejecutables separados
$(BP_BIN): $(BP_SRC) $(BP_TEST)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(B_BIN): $(B_SRC) $(B_TEST)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(EXP_BIN): $(EXP) $(B_SRC) $(BP_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Atajos de ejecución
run: $(EXP_BIN)
	./$(EXP_BIN)

test-bplus: $(BP_BIN)
	./$(BP_BIN)

test-btree: $(B_BIN)
	./$(B_BIN)

# Ejecutar ambos tests uno tras otro
tests: $(BP_BIN) $(B_BIN)
	./$(BP_BIN)
	./$(B_BIN)

# Limpiar binarios generados
clean:
	rm -f $(BP_BIN) $(B_BIN) $(EXP_BIN) pares.bin BP.bin btree_disco data/* data_out.txt *.o