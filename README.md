# Tarea 1: Árboles B y B+

Implementación en C++ de estructuras de datos **B-Tree** y **B+Tree** con soporte para:
- Inserción uno a uno de pares `(key:int, value:float)` desde un dataset binario.
- Consultas por rango `[l,u]` tanto en RAM como desde disco simulado/real.
- Medición de I/O simuladas, tiempos de construcción y de consultas.
- Exportación de resultados en formato CSV  para análisis y gráficos.

--

## 📂 Estructura del proyecto

Arbol_B: Carpeta con modulo de arbol B
Arbol_B+: Carpeta  con modulo de arbol B+
data: datos de experimentación
plots: carpeta con graficos de graph.py

requirimientos:

C++17 para compilar y ejecutar arboles. 
Python3, pandas y matplotlib para graficar con graph.py

## Como compilar

compilar y ejecutar experimentacion:  
make run

graficar resultados:  
python3 graph.py