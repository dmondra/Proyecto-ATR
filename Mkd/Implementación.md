## Software en C++ y Computación de Alto Rendimiento (HPC)

El archivo `malla_alternativa.cpp` comienza definiendo una clase que permite encapsular tanto los datos como la lógica necesaria para construir la geometría de la habitación.

## Clase `Habitacion`
Por medio de `Habitacion`, el constructor calcula automáticamente el espaciamiento de la malla y la ubicación de los sensores. Esto evita que estos cálculos deban realizarse de manera externa, favoreciendo una implementación más modular y eficiente.

---

## Optimización y Paralelización con OpenMP
Posterior a la creación del espacio en el que se trabajará, la función `ejecutar_solver_atr_optimizado` es la que realiza la simulación. Esta se hace con archivos binarios para evitar tener que guardar muchos archivos csv. La variable `dt` es simplemente una condición CFL que aporta estabilidad al método.

El algoritmo se paralelizó mediante OpenMP, empleando directivas `pragma omp parallel for` en las partes repetitivas del algoritmo en donde no hubiera dependencias temporales:

## Zonas Paralelizadas:
* **Copia del Campo:** La copia del campo a la entrada de FFTW, que depende enteramente de la dimensión de la malla (establecida en $128 \times 128$), en donde los hilos se reparten este trabajo puesto que no importa el orden en que se haga la actualización.
* **Operador Espectral:** El cálculo del operador espectral $-k^{2}\hat{u}$, en donde como cada punto en el espacio de Fourier es independiente del otro, los hilos pueden optimizarlo.
* **Actualización Leapfrog:** La parte de la ecuación de actualización temporal que se hizo con Leapfrog, donde entre los hilos se reparten el cálculo de los nodos a futuro y se evalúa de manera independiente para cada punto de la grilla.
* **Actualización de Arreglos:** Finalmente, lo último que se paralelizó fue la actualización de los arreglos, donde se reparten distintas partes de este.

## Restricciones de Paralelización (Zonas Secuenciales):
Es importante tomar en cuenta que las partes del código en donde hay que actualizar los nodos a futuro donde depende del anterior no pueden paralelizarse. Como en la parte `for (int n=0; n < Nt-1; n++)`, el instante $n+1$ depende del instante anterior $n$.

Todos estos procesos tienen en común que cada hilo realiza la misma tarea, pero desde distintos puntos de inicio en la malla. Al finalizar la inversión temporal, el programa guarda los estados en un archivo `.csv` y escribe una matriz reconstruida para ser leída por Python. Por último, borra las variables definidas para evitar memory leaks.
