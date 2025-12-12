# Trabajo Práctico No. 4 - Aplicaciones en C++

Este repositorio contiene los 3 programas desarrollados para el TP4 de Programación Avanzada. Cada programa está en su propio archivo `.cpp` y se puede compilar y ejecutar de forma independiente.

---

## Como Compilar y Ejecutar

### En Windows (con g++):

1. Abrir la terminal (CMD o PowerShell) en la carpeta donde están los archivos.
2. Para compilar cada programa usar:
   ```
   g++ nombre_archivo.cpp -o nombre_programa.exe
   ```
3. Ejecutar con:
   ```
   nombre_programa.exe
   ```

### Ejemplo:
```
g++ puzzle_8.cpp -o puzzle_8.exe
puzzle_8.exe
```

**Nota:** Cada programa es independiente, hay que compilarlos por separado.

---

## Ejercicio 1 (10%) — 8-Puzzle Solver

**Archivo:** `puzzle_8.cpp`

**Qué hace:**
- Resuelve el rompecabezas de 8 piezas (tablero 3x3).
- El usuario ingresa la configuración inicial y el programa encuentra la solución óptima.
- Usa el algoritmo A* con heurística de distancia Manhattan.
- Muestra la secuencia de movimientos para resolver el puzzle.

**Cómo usarlo:**
1. Ejecutar el programa.
2. Ingresar los 9 números del tablero inicial (usar 0 para el espacio vacío).
3. El objetivo está fijo: `1 2 3 / 8 0 4 / 7 6 5`
4. El programa dirá si tiene solución y mostrará los movimientos.

**Ejemplo de entrada:**
```
5 7 2
4 0 1
3 8 6
```

**Metodología:**
- Implementamos A* para buscar la solución más corta.
- Usamos una cola de prioridad para explorar estados.
- Verificamos la paridad de inversiones para saber si tiene solución.
- Si no tiene solución matemáticamente posible, el programa lo indica.

---

## Ejercicio 2 (50%) — Simulador HIL (Hardware in the Loop)

**Archivo:** `hil_simulator.cpp`

**Qué hace:**
- Simula una planta SISO (sistema de control) en tiempo real.
- Implementa un derivador de orden superior según las ecuaciones del enunciado.
- Genera señales de referencia (escalón, rampa, senoidal).
- Guarda los resultados en un archivo `.txt` para graficar después.

**Cómo usarlo:**
1. Ejecutar el programa.
2. Elegir tipo de señal (1=escalón, 2=rampa, 3=senoidal).
3. Ingresar tiempo de simulación (recomendado: 10-40 segundos).
4. El programa simula todo y guarda los datos en `resultados_hil_TipoSenal.txt`.
5. Opcionalmente genera un script de Python para graficar los resultados.

**Metodología:**
- Discretizamos la planta G(s) usando el método de Tustin.
- Implementamos el derivador con las ecuaciones exactas del PDF.
- Frecuencia de muestreo: 250 Hz (tau_s = 0.004 s).
- Los coeficientes de discretización los calculamos con herramientas numéricas.

**Para graficar:**
- Si generaste el script Python, ejecuta: `python graficar_resultados.py`
- O abre el archivo .txt con Excel/MATLAB y grafica las columnas.

---

## Ejercicio 3 (40%) — Red Neuronal para Clasificación de Dígitos

**Archivo:** `red_neuronal.cpp`

**Qué hace:**
- Red neuronal artificial que clasifica dígitos del 0 al 9.
- Los dígitos están en formato de imagen 7x5 píxeles (como arte ASCII).
- Lee los patrones desde el archivo `digitos.txt`.
- Clasifica cada dígito en 4 categorías: PAR, IMPAR, PRIMO, COMPUESTO.

**Cómo usarlo:**
1. Asegurarse de que el archivo `digitos.txt` esté en la misma carpeta.
2. Ejecutar el programa.
3. La red se entrena automáticamente (tarda unos segundos).
4. Muestra los resultados de clasificación para cada dígito.
5. Genera un reporte en `reporte_clasificacion.txt`.

**Formato del archivo digitos.txt:**
- Cada dígito es una matriz de 7 filas x 5 columnas.
- Se representa con 0s y 1s (como píxeles).
- Ver el ejemplo en el archivo adjunto.

**Clases:**
- Clase 0: Números PARES (0, 2, 4, 6, 8)
- Clase 1: Números IMPARES (1, 3, 5, 7, 9)
- Clase 2: Números PRIMOS (2, 3, 5, 7)
- Clase 3: Números COMPUESTOS (4, 6, 8, 9)

**Metodología:**
- Arquitectura: 35 neuronas entrada (7×5 píxeles) → 20 neuronas ocultas → 4 salidas (clases).
- Función de activación: Sigmoid.
- Entrenamiento: Backpropagation con ~10,000 épocas.
- Inicialización de pesos: Xavier initialization.
- El programa reconoce visualmente el dígito y lo clasifica en las categorías.

---

## Archivos Importantes

- `digitos.txt` - Archivo con los patrones de dígitos para el ejercicio 3 (obligatorio).
- `resultados_hil_*.txt` - Resultados de la simulación HIL (se genera automáticamente).
- `reporte_clasificacion.txt` - Reporte de la red neuronal (se genera automáticamente).
- `graficar_resultados.py` - Script opcional para graficar (si se genera).

---

## Notas y Problemas Comunes

### Ejercicio 1 (8-Puzzle):
- Si dice "no tiene solución", es porque matemáticamente no se puede resolver.
- Prueba con la configuración del ejemplo: `5 7 2 4 0 1 3 8 6`
- Algunas configuraciones tardan más en resolverse (son normales).

### Ejercicio 2 (HIL):
- La simulación puede tardar dependiendo del tiempo elegido.
- Los archivos .txt se pueden abrir con Excel para graficar manualmente.
- Si genera el script Python, necesitas tener instalado: `pandas` y `matplotlib`.

### Ejercicio 3 (Red Neuronal):
- **MUY IMPORTANTE:** El archivo `digitos.txt` debe estar en la misma carpeta.
- El entrenamiento tarda ~10-30 segundos (es normal).
- Si los resultados no son buenos, aumentar las épocas de entrenamiento en el código.

---

## Compilación con Standard C++11

Algunos programas usan características de C++11, así que recomendamos compilar con:

```
g++ archivo.cpp -o programa.exe -std=c++11
```

---

## Autores

Juan Suárez Herron  
Santiago Torres Guerrero  
Marbin Fabián Rivero  

**Curso:** Programación Avanzada  
**Profesor:** Mauro Baquero-Suárez  
**Programa:** Ingeniería Mecatrónica  
**Fecha de Entrega:** 16 de Diciembre de 2025

---

## Referencias

- Algoritmo A* para 8-puzzle: consultamos varios tutoriales en línea.
- Discretización de sistemas: apuntes de clase y Wikipedia.
- Redes neuronales: vimos ejemplos en YouTube y blogs de ML.
- La mayoría del código lo hicimos nosotros con ayuda de internet para partes específicas.