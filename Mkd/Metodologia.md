# 1. Metodología Matemática y Modelado Numérico

Para resolver el problema, se partió desde la ecuación de presión acústica continua $u(x,y,t)$:

$$\nabla^{2}u-\frac{1}{v^{2}}\frac{\partial^{2}u}{\partial t^{2}}=0$$

Al separar las variables de la función $u$, la ecuación queda en términos espaciales y temporales:
* **Parte Espacial:** Se solucionó utilizando la transformada de Fourier.
* **Parte Temporal:** Se solucionó empleando el método de integración Leapfrog.

---

## Estructura de Variables en el Código
En primer lugar se crean tres arreglos de tamaño $N_x N_y$ donde cada uno almacena los momentos acústicos:
* `u_pasado`: Almacena el estado $u^{n-1}$.
* `u_presente`: Almacena el estado $u^n$.
* `u_futuro`: Almacena el estado $u^{n+1}$.

Adicionalmente, se gestionarían las mallas para las transformadas:
* `u_hat`: Contendrá $\hat{u}$, que se define por medio de $\mathcal{F}\{u(x,y)\} = \hat{u}$.
* `lap_hat`: Almacena el cálculo espectral del Laplaciano $-k^{2}\hat{u}$.
* `lap_real`: Almacena el Laplaciano devuelto al espacio real $\nabla^{2}u$.

Después se construyen los vectores $k_x$ y $k_y$ que dan lugar a la igualdad:

$$k^{2} = k_{x}^{2} + k_{y}^{2}$$

Donde $k$ representa los números de onda.

---

## Algoritmo de Evolución Temporal
La parte activa del algoritmo es el ciclo `for (int n=0; n < Nt-1; n++)` que realiza el cálculo de $\nabla^{2}u$ desde el inicio de la simulación mandándolo al espacio de Fourier:

1. Lo primero que hace con `fftw_execute(plan_directo);` es hacer $\mathcal{F}\{u(x,y)\} = \hat{u}(k_x, k_y)$ y lo almacena en la variable respectiva.
2. Luego se multiplica por $-k^{2}$ para obtener el cálculo de $\mathcal{F}\{\nabla^{2}u\} = -k^{2}\hat{u}$.
3. Por último, `fftw_execute(plan_inverso);` aplica Fourier inversa $\mathcal{F}^{-1}\{-k^{2}\hat{u}\} = \nabla^{2}u$ almacenando este resultado en su respectiva variable.
4. Finalmente actualiza la ecuación de ondas con el Laplaciano que ya se conoce, quedando como:
   $$u^{n+1} = 2u^{n} - u^{n-1} + v^{2}(\Delta t)^{2}\nabla^{2}u$$
