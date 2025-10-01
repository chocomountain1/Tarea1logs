# Compilador y flags
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

# Archivos fuente
SRC := Arbol_B+/testbplus.cpp Arbol_B+/bplus.cpp
TARGET := Arbol_B+/testbplus
# Regla por defecto
all: $(TARGET)

# Compilar el test
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Ejecutar el test después de compilar
run: $(TARGET)
	./$(TARGET)

# Limpiar binarios generados
clean:
	rm -f $(TARGET) *.o BP.bin
