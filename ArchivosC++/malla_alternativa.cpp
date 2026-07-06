#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <omp.h>
#include <fftw3.h>
//construcción de la clase
class Habitacion {
private:
    // Atributos privados: no se pueden modificar desde fuera de la clase
    double hx;
    double hy;
    std::vector<int> idx_sensores;
public:
    // CONSTRUCTOR: Reemplaza a 'construir_malla_perimetral'
    // Inicializa el objeto y calcula automáticamente la malla perimetral
    Habitacion(int Nx, int Ny, double dx, double dy) {
        hx = dx / double(Nx);
        hy = dy / double(Ny);
        // Mapeo de paredes fijas (Norte y Sur)
        for (int j = 0; j < Ny; j++) {
            idx_sensores.push_back(0 * Ny + j);
            idx_sensores.push_back((Nx - 1) * Ny + j);
        }
        // Mapeo de paredes laterales (Este y Oeste)
        for (int i = 1; i < Nx - 1; i++) {
            idx_sensores.push_back(i * Ny + 0);
            idx_sensores.push_back(i * Ny + (Ny - 1));
        }
    }
    // MÉTODOS DE ACCESO (Getters): Permiten leer los datos protegiendo su integridad
    double get_hx() const { return hx; }
    double get_hy() const { return hy; }
    int get_num_sensores() const { return static_cast<int>(idx_sensores.size()); }
    int get_sensor_idx(int s) const { return idx_sensores[s]; }
};
// Módulo del motor de cálculo por Fourier (Se adapta para recibir el objeto 'Habitacion')
void ejecutar_solver_atr_optimizado(int Nx, int Ny, int Nt, double dx, double dy, double v, const Habitacion& sala) {
    std::ofstream archivo_binario("viaje_sonido.bin", std::ios::binary);
    const int tamano = Nx * Ny;
    // OOP en uso: Ahora usamos los métodos 'get' para acceder a los datos de la sala
    const double termino_espacial = std::sqrt((1.0 / (sala.get_hx() * sala.get_hx())) + (1.0 / (sala.get_hy() * sala.get_hy())));
    const double dt = (2.0 / (M_PI * v * termino_espacial)) * 0.9; 
    double* u_pasado   = new double[tamano]();
    double* u_presente = new double[tamano]();
    double* u_futuro   = new double[tamano]();
    fftw_complex* u_real   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* u_hat    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* lap_hat  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* lap_real = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_plan plan_directo = fftw_plan_dft_2d(Nx, Ny, u_real, u_hat, FFTW_FORWARD, FFTW_MEASURE);
    fftw_plan plan_inverso = fftw_plan_dft_2d(Nx, Ny, lap_hat, lap_real, FFTW_BACKWARD, FFTW_MEASURE);

    std::vector<double> kx(Nx), ky(Ny);
    for (int i = 0; i < Nx; i++) {
        kx[i] = (i < Nx / 2) ? i * (2.0 * M_PI / dx) : (i - Nx) * (2.0 * M_PI / dx);
    }
    for (int j = 0; j < Ny; j++) {
        ky[j] = (j < Ny / 2) ? j * (2.0 * M_PI / dy) : (j - Ny) * (2.0 * M_PI / dy);
    }

    // OOP en uso: sala.get_num_sensores()
    std::vector<std::vector<double>> grabacion_paredes(sala.get_num_sensores(), std::vector<double>(Nt, 0.0));

    std::cout << "Iniciando simulacion directa..." << std::endl;
    for (int n = 0; n < Nt - 1; n++) {
        if (n == 0) u_presente[(Nx / 2) * Ny + (Ny / 2)] = 10.0;
        if (n % 5 == 0) {
            archivo_binario.write(reinterpret_cast<char*>(u_presente), sizeof(double) * tamano);
        }

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            u_real[k][0] = u_presente[k];
            u_real[k][1] = 0.0;
        }

        fftw_execute(plan_directo);

        #pragma omp parallel for collapse(2) schedule(static)
        for (int i = 0; i < Nx; i++) {
            for (int j = 0; j < Ny; j++) {
                int idx = i * Ny + j;
                double k2 = (kx[i] * kx[i]) + (ky[j] * ky[j]);
                lap_hat[idx][0] = -k2 * u_hat[idx][0];
                lap_hat[idx][1] = -k2 * u_hat[idx][1];
            }
        }

        fftw_execute(plan_inverso);

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            double lap_normalizado = lap_real[k][0] / double(tamano);
            u_futuro[k] = 2.0 * u_presente[k] - u_pasado[k] + (v * v * dt * dt) * lap_normalizado;
        }

        // OOP en uso: sala.get_num_sensores() y sala.get_sensor_idx(s)
        for (int s = 0; s < sala.get_num_sensores(); s++) {
            grabacion_paredes[s][n] = u_presente[sala.get_sensor_idx(s)];
        }

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            u_pasado[k] = u_presente[k];
            u_presente[k] = u_futuro[k];
        }
    }
    archivo_binario.close();

    std::ofstream archivo1("prueba.csv");
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            archivo1 << u_presente[i * Ny + j] << (j == Ny - 1 ? "" : ",");
        }
        archivo1 << "\n";
    }
    archivo1.close();

    std::ofstream archivo_binario2("inversion_temporal.bin", std::ios::binary);
    std::cout << "Iniciando inversion temporal..." << std::endl;

    #pragma omp parallel for schedule(static)
    for (int k = 0; k < tamano; k++) {
        u_pasado[k] = 0.0; u_presente[k] = 0.0; u_futuro[k] = 0.0;
    }

    for (int n = Nt - 1; n > 0; n--) {
        if (n % 5 == 0) {
            archivo_binario2.write(reinterpret_cast<char*>(u_presente), sizeof(double) * tamano);
        }

        // OOP en uso: sala.get_num_sensores() y sala.get_sensor_idx(s)
        for (int s = 0; s < sala.get_num_sensores(); s++) {
            u_presente[sala.get_sensor_idx(s)] += grabacion_paredes[s][n];
        }

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            u_real[k][0] = u_presente[k];
            u_real[k][1] = 0.0;
        }

        fftw_execute(plan_directo);

        #pragma omp parallel for collapse(2) schedule(static)
        for (int i = 0; i < Nx; i++) {
            for (int j = 0; j < Ny; j++) {
                int idx = i * Ny + j;
                double k2 = (kx[i] * kx[i]) + (ky[j] * ky[j]);
                lap_hat[idx][0] = -k2 * u_hat[idx][0];
                lap_hat[idx][1] = -k2 * u_hat[idx][1];
            }
        }

        fftw_execute(plan_inverso);

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            double lap_normalizado = lap_real[k][0] / double(tamano);
            u_futuro[k] = 2.0 * u_presente[k] - u_pasado[k] + (v * v * dt * dt) * lap_normalizado;
        }

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            u_pasado[k] = u_presente[k];
            u_presente[k] = u_futuro[k];
        }
    }
    archivo_binario2.close();

    std::ofstream archivo("resultado_forense.csv");
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            archivo << u_presente[i * Ny + j] << (j == Ny - 1 ? "" : ",");
        }
        archivo << "\n";
    }
    archivo.close();

    fftw_destroy_plan(plan_directo); fftw_destroy_plan(plan_inverso);
    fftw_free(u_real); fftw_free(u_hat); fftw_free(lap_hat); fftw_free(lap_real);
    delete[] u_pasado; delete[] u_presente; delete[] u_futuro;
}

int main() {
    const int Nx = 128;
    const int Ny = 128;
    const int Nt = 300;
    const double dx = 10.0;
    const double dy = 10.0;
    const double v = 343.0;

    // OOP en uso: Instanciamos directamente el objeto 'sala' invocando su constructor
    Habitacion sala(Nx, Ny, dx, dy);

    ejecutar_solver_atr_optimizado(Nx, Ny, Nt, dx, dy, v, sala);

    return 0;
}
