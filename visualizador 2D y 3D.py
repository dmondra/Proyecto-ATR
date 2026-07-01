import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from IPython import display

def generar_heatmap():
    print("--- Iniciando Visualización Forense ---")
    
    try:
        # 1. Cargar la matriz que calculó tu motor de C++
        # Como C++ la guardó separada por comas, usamos delimiter=","
        malla_t0 = np.loadtxt("resultado_forense.csv", delimiter=",")
        
        print(f"Éxito: Matriz cargada con dimensiones {malla_t0.shape}")
        
        # 2. Configurar la figura estética para el reporte
        plt.figure(figsize=(9, 7), facecolor='white')
        
        # 3. Graficar el mapa de calor
        # 'inferno' es excelente porque el negro representa silencio (0) 
        # y el amarillo brillante representa la energía máxima del disparo.
        heatmap = plt.imshow(malla_t0, cmap='inferno', origin='upper', interpolation='nearest')
        
        # 4. Agregar la barra de escala de presión de la onda
        cbar = plt.colorbar(heatmap)
        cbar.set_label('Amplitud de Presión Numérica (u)', fontsize=11, fontweight='bold', labelpad=10)
        
        # 5. Formatear títulos y ejes estilo "Física Computacional"
        plt.title("Localización de la Fuente Acústica en t = 0\n(Reconstrucción Espectral por Inversión Temporal)", 
                  fontsize=13, fontweight='bold', pad=15)
        plt.xlabel("Nodos Eje Y (Espacio Discreto)", fontsize=11, labelpad=8)
        plt.ylabel("Nodos Eje X (Espacio Discreto)", fontsize=11, labelpad=8)
        
        # Ajuste fino de la cuadrícula para que encaje perfecto
        plt.tight_layout()
        
        # 6. Guardar el gráfico en alta resolución (300 DPI) para el PDF final
        nombre_imagen = "reconstruccion_escena.png"
        plt.savefig(nombre_imagen, dpi=300)
        print(f"Imagen guardada con éxito como '{nombre_imagen}'")
        
        # Mostrar en pantalla
        plt.show()
        
    except FileNotFoundError:
        print("\n[ERROR]: No se encontró el archivo 'resultado_forense.csv'.")
        print("Asegúrate de ejecutar primero el binario de C++ (./atr_solver) en esta misma carpeta.")
    except Exception as e:
        print(f"\n[ERROR INESPERADO]: {e}")

def graficar_malla_3d():
    """Genera una gráfica de superficie en 3D."""
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    malla_t0 = np.loadtxt("resultado_forense.csv", delimiter=",")
    print(f"Éxito: Matriz cargada con dimensiones {malla_t0.shape}")
    X, Y = np.meshgrid(np.arange(malla_t0.shape[1]), np.arange(malla_t0.shape[0]))
    surf = ax.plot_surface(X, Y, malla_t0, cmap='inferno')
    
    fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label='Magnitud Física (F)')
    ax.set_title("Demostración: Relieve de la Malla", fontsize=14, fontweight='bold', pad=20)
    ax.set_xlabel('Eje X')
    ax.set_ylabel('Eje Y')
    ax.set_zlabel('Valor')
    plt.savefig('superficie_3d_malla.png', dpi=300)
    plt.show()

if __name__ == "__main__":
    generar_heatmap()
    graficar_malla_3d()
