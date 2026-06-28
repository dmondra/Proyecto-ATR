import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def graficar_malla_2d(malla, titulo="Distribución en la Malla"):
    """Genera un mapa de calor (Heatmap) de la simulación."""
    plt.figure(figsize=(8, 6))
    plt.imshow(malla, cmap='inferno', origin='lower', interpolation='gaussian')
    plt.colorbar(label='Magnitud Física (F)')
    plt.title(titulo)
    plt.xlabel('Eje X (Nodos)')
    plt.ylabel('Eje Y (Nodos)')
    plt.grid(False)
    plt.savefig('mapa_calor_malla.png', dpi=300)
    plt.show()

def graficar_malla_3d(malla, titulo="Superficie de la Malla"):
    """Genera una gráfica de superficie en 3D."""
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    
    X, Y = np.meshgrid(np.arange(malla.shape[1]), np.arange(malla.shape[0]))
    surf = ax.plot_surface(X, Y, malla, cmap='viridis', edgecolor='none')
    
    fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label='Magnitud Física (F)')
    ax.set_title(titulo)
    ax.set_xlabel('Eje X')
    ax.set_ylabel('Eje Y')
    ax.set_zlabel('Valor')
    plt.savefig('superficie_3d_malla.png', dpi=300)
    plt.show()

if __name__ == "__main__":
    print("Ejecutando demostración del módulo de visualización...")
    
    # Simulación de una malla de prueba de 50x50 nodos
    Lx, Ly = 50, 50
    malla_prueba = np.zeros((Lx + 1, Ly + 1))
    
    # Añadimos condiciones de frontera ficticias (ej: calor en los bordes)
    malla_prueba[0, :] = 100.0   # Borde inferior
    malla_prueba[-1, :] = 50.0   # Borde superior
    malla_prueba[:, 0] = 75.0    # Borde izquierdo
    
    # Generamos los gráficos de muestra
    graficar_malla_2d(malla_prueba, "Demostración: Mapa de Calor Inicial")
    graficar_malla_3d(malla_prueba, "Demostración: Relieve de la Malla")
    print("Gráficas generadas y guardadas exitosamente como imágenes.")

