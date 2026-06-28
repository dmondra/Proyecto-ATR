# Configuración del compilador y banderas
CXX = g++
CXXFLAGS = -O3 -Wall
OMPFLAGS = -fopenmp

# Archivos ejecutables de salida
TARGET_SERIAL = malla_ejecutable
TARGET_PARALLEL = malla_paralela_ejecutable

# Regla por defecto: compilar ambos
all: $(TARGET_SERIAL) $(TARGET_PARALLEL)

# Compilación de la versión secuencial corregida
$(TARGET_SERIAL): malla.cpp
	@echo "Compilando versión secuencial..."
	$(CXX) $(CXXFLAGS) malla.cpp -o $(TARGET_SERIAL)

# Compilación de la versión paralela con OpenMP
$(TARGET_PARALLEL): mallaParalelizada.cpp
	@echo "Compilando versión paralela con OpenMP..."
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) mallaParalelizada.cpp -o $(TARGET_PARALLEL)

# Limpieza de ejecutables generados
clean:
	@echo "Limpiando ejecutables..."
	rm -f $(TARGET_SERIAL) $(TARGET_PARALLEL)
	@echo "Limpieza completada."
