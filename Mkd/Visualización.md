## Post-procesamiento en Python, Resultados y Análisis de Estabilidad

Python se empleó para la graficación de los mapas de calor tanto en 2D como en 3D.

---

##  Estructura del Script de Visualización
Del archivo `visualizador_an.py`, las funciones se ejecutan en el siguiente orden para el análisis gráfico:

1. `antes_regresion`: Genera la animación de la onda de choque en el centro de la habitación, distribuyéndose uniformemente hasta chocar con los sensores en los extremos de esta.
2. `generar_reconstruccion`: Genera el heatmap del momento final de la regresión temporal, donde el sonido termina de converger en el punto inicial.
3. `graficar_malla_3d`: Hace lo mismo que la función anterior, pero graficado a 3 dimensiones, aumentando la altura de los valores de la presión.
4. `animar_malla`: Genera el mapa de calor animado correspondiente al archivo `viaje_del_sonido.mp4`.
5. `animar_reconstruccion`: Genera el video correspondiente a la regresión temporal animada llamado `inversion_temporal.mp4`.

---

##  Análisis de Resultados

* **Reconstrucción Espectral ($t=0$):** El primer heatmap representa el punto en donde se da la mayor amplitud de presión. Esto tiene sentido considerando que se colocó el origen del sonido en el centro, porque se necesita una posición arbitraria para tener el fenómeno de estudio.
* **Viaje de la Onda:** Las animaciones muestran el proceso inverso exacto, donde las ondas capturadas por los sensores perimetrales retroceden en el tiempo enfocándose simétricamente hacia el origen.

---

##  Análisis de Fallas de Simulación (Condición de Estabilidad CFL)
La parte del código en la que puede provocarse una falla en la simulación viene dada por la configuración de la condición del número de Courant:

```cpp
 const double termino_espacial = std::sqrt((1.0 / (sala.hx * sala.hx)) + (1.0 / (sala.hy * sala.hy)));
const double dt = (2.0 / (M_PI * v * termino_espacial))*2.0; 
