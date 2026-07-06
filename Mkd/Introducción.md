# Reconstrucción de Escenas mediante Reversión Temporal Acústica (ATR)

**Autores:** Gabriel Briceño, Fiorella Calvo, Rolando Saborío, David Mondragón

---

## Justificación
Tradicionalmente, las ciencias naturales utilizan el estado presente de un sistema para predecir su evolución futura. Sin embargo, en la investigación forense y la acústica aplicada, surge el reto del **Problema Inverso**: utilizar datos capturados en el presente para deducir con exactitud el origen de un evento. 

Esta propuesta plantea invertir la "flecha del tiempo" mediante la técnica de **Reversión Temporal Acústica (ATR)**. Al explotar la simetría temporal de la ecuación de onda, el software propuesto procesará señales acústicas grabadas para retroceder numéricamente hasta el instante inicial. El objetivo es transformar el ruido capturado por sensores en una herramienta de deducción científica capaz de localizar fuentes sonoras y reconstruir escenas con una precisión superior a los métodos convencionales.

---

## Índice de Documentación
Para conocer a fondo el desarrollo técnico del proyecto, navega por las siguientes secciones:

1. [**Metodología Matemática**](docs/01_metodologia.md): Modelado físico, Fourier y discretización por Leapfrog.
2. [**Implementación en C++ y Paralelización**](docs/02_implementacion_cpp.md): Arquitectura de software, gestión de memoria y optimización con OpenMP.
3. [**Visualización en Python y Análisis de Estabilidad**](docs/03_visualizacion_resultados.md): Scripts de graficación, resultados experimentales y fallas por condición CFL.
