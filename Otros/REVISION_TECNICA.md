# Revisión Técnica del Estado del Código

Este documento contiene observaciones constructivas tras revisar los scripts iniciales del repositorio con el fin de asegurar una transición limpia hacia las siguientes fases del proyecto final.

## 1. Correcciones de Sintaxis Requeridas en `malla.cpp`
El archivo actual presenta problemas sintácticos menores que impiden su compilación con `g++`:

* **Uso de Namespace:** Se están utilizando llamadas directas a `cout` y `endl` en las líneas 21 y 25 sin haber declarado `using namespace std;`. Se debe corregir cambiando a `std::cout` y `std::endl`.
* **Case Sensitivity (Mayúsculas/Minúsculas):** En la línea 7 se inicializa el vector como `Malla` (M mayúscula). Sin embargo, en las líneas 14 y 15 se intenta asignar valores usando `malla` (m minúscula). C++ es sensible a mayúsculas, por lo que arrojará un error de "variable no declarada".
* **Punto y coma faltante:** En la función `main()`, la línea `return 0` carece del punto y coma `;` reglamentario.

## 2. Recomendaciones de Integración (C++ -> Python)
Para que el script `Proyecto Computacional.py` o el archivo de Jupyter puedan procesar los resultados de los ejecutables de C++, se sugiere:

1. **Modificar el `main` de C++** para que reciba parámetros de malla (`Lx`, `Ly`, `dx`, `dy`) mediante argumentos de línea de comandos (`argc`, `argv`).
2. **Exportar a archivo plano:** Implementar la librería `<fstream>` en C++ para escribir los valores finales del vector `Malla` en un archivo estructurado como `.csv` o `.txt`.
3. **Lectura en Python:** Utilizar `numpy.loadtxt()` para importar esos datos directamente al flujo de visualización de gráficos.

## 3. Corrección en el código:
Nos dimos cuenta que en la línea 111 se tenía: 
```
u_presente[sala.get_sensor_idx(s)] = grabacion_paredes[s][n];

```
Esto al graficar nos daba el t final donde ya estaba todo el sonido en el centro, donde converge.
Sin embargo al hacer esta corrección:

```
u_presente[sala.get_sensor_idx(s)] += grabacion_paredes[s][n];
```
Nos dimos cuenta que parece tardar más en llegar a el punto anterior lo cual hace mucho sentido.

Lo que sucede con el = al no guardar si no reemplazar, es como tener un coro, donde solo nos interesa ir a escuchar al último que entró cuando nos movemos hacia la salida.
Lo que hace el += es que tengamos que ir persona por persona hasta llegar a la salida.
