#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <omp.h>
#include <fftw3.h>

int main() {
    // Parametros espaciales y temporales
    const int Nx = 128;        
    const int Ny = 128;        
    const int Nt = 300;        
    
    const double dx = 10.0;     
    const double dy = 10.0;     
    const double v = 343.0;    
    
    const double hx = dx / double(Nx);
    const double hy = dy / double(Ny);
    
    // Condicion de estabilidad CFL espectral
    const double termino_espacial = std::sqrt((1.0 / (hx * hx)) + (1.0 / (hy * hy)));
    const double dt = (2.0 / (M_PI * v * termino_espacial)) * 0.9; 

    const int tamano = Nx * Ny;
    
    // Arreglos para la evolucion temporal real
    double* u_pasado   = new double[tamano]();
    double* u_presente = new double[tamano]();
    double* u_futuro   = new double[tamano]();

    // Variables complejas alineadas para FFTW
    fftw_complex* u_real   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* u_hat    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* lap_hat  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);
    fftw_complex* lap_real = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tamano);

    // Planes de ejecución FFTW
    fftw_plan plan_directo = fftw_plan_dft_2d(Nx, Ny, u_real, u_hat, FFTW_FORWARD, FFTW_MEASURE);
    fftw_plan plan_inverso = fftw_plan_dft_2d(Nx, Ny, lap_hat, lap_real, FFTW_BACKWARD, FFTW_MEASURE);

    // Definición del espacio de frecuencias (k-space)
    std::vector<double> kx(Nx), ky(Ny);
    for (int i = 0; i < Nx; i++) {
        kx[i] = (i < Nx / 2) ? i * (2.0 * M_PI / dx) : (i - Nx) * (2.0 * M_PI / dx);
    }
    for (int j = 0; j < Ny; j++) {
        ky[j] = (j < Ny / 2) ? j * (2.0 * M_PI / dy) : (j - Ny) * (2.0 * M_PI / dy);
    }

    // Identificación de nodos de frontera para sensores
    std::vector<int> idx_sensores;
    for (int j = 0; j < Ny; j++) {
        idx_sensores.push_back(0 * Ny + j);
        idx_sensores.push_back((Nx - 1) * Ny + j);
    }
    for (int i = 1; i < Nx - 1; i++) {
        idx_sensores.push_back(i * Ny + 0);
        idx_sensores.push_back(i * Ny + (Ny - 1));
    }
    int num_sensores = idx_sensores.size();
    std::vector<std::vector<double>> grabacion_paredes(num_sensores, std::vector<double>(Nt, 0.0));

    // Evolución hacia el futuro
    std::cout << "Ejecutando simulacion directa..." << std::endl;

    for (int n = 0; n < Nt - 1; n++) {
        if (n == 0) {
            u_presente[(Nx / 2) * Ny + (Ny / 2)] = 10.0;
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

        for (int s = 0; s < num_sensores; s++) {
            grabacion_paredes[s][n] = u_presente[idx_sensores[s]];
        }

        #pragma omp parallel for schedule(static)
        for (int k = 0; k < tamano; k++) {
            u_pasado[k] = u_presente[k];
            u_presente[k] = u_futuro[k];
        }
    }

    // Reversión temporal hacia el pasado
    std::cout << "Ejecutando inversion temporal..." << std::endl;

    #pragma omp parallel for schedule(static)
    for (int k = 0; k < tamano; k++) {
        u_pasado[k] = 0.0; u_presente[k] = 0.0; u_futuro[k] = 0.0;
    }

    for (int n = Nt - 1; n > 0; n--) {
        for (int s = 0; s < num_sensores; s++) {
            u_presente[idx_sensores[s]] = grabacion_paredes[s][n];
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

    // Almacenamiento del estado reconstruido en t=0
    std::ofstream archivo("resultado_forense.csv");
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            archivo << u_presente[i * Ny + j] << (j == Ny - 1 ? "" : ",");
        }
        archivo << "\n";
    }
    archivo.close();

    // Liberacion de recursos complejos y vectores discretos
    fftw_destroy_plan(plan_directo);
    fftw_destroy_plan(plan_inverso);
    fftw_free(u_real); fftw_free(u_hat); fftw_free(lap_hat); fftw_free(lap_real);
    delete[] u_pasado; delete[] u_presente; delete[] u_futuro;

    return 0;
}
