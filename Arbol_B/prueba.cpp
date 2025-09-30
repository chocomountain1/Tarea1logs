#include <fstream>
#include <iostream>
#include <random>

struct Llave_valor {
    int llave;
    float valor;
};

int main() {
    std::ofstream archivo("pares.bin", std::ios::binary);

    // Crear 10 pares de prueba
    std::random_device rd; 
    std::mt19937 gen(rd());  // Mersenne Twister, muy usado
    std::uniform_int_distribution<> distrib(1, 350);
    for (int i = 1; i <= 350; ++i) {
        Llave_valor lv;
        lv.llave = i;
        lv.valor = distrib(gen)*1.1f;
        archivo.write(reinterpret_cast<char*>(&lv), sizeof(Llave_valor));
    }

    archivo.close();
    std::cout << "Archivo binario creado con 1000 pares.\n";
    return 0;
}
