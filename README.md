# Cluster-Beowulf
Cluster Beowulf para algoritmo de procesamiento de imágenes.

Aplica el filtro Kuwahara.

## Dependencias
Requiere OpenCV 3.4.1.

Para instalar OpenCV ejecutar el script config.sh

Dar permisos: 

$ sudo chmod +x installOpenCV.sh

Y ejecutarlo: 

$ ./installOpenCV.sh

## Ejecución

El programa se puede ejecutar de manera local o utilizando máquinas conectadas a la red

Para ejecutarlo local con dos procesadores se debe correr el comando:

$ mpirun -np 2 ./myprogram

Para ejecutarlo utilizando máquinas conectadas a la red:

$ mpirun -np 2 --hostfile .mpi_hostfile ./myprogram

## Información
Instituto Tecnológico de Costa Rica

Área Académica de Ingeniería en Computadores

**Curso:** Arquitectura de Computadores II

**Profesor:** MSc. Jeferson González Gómez

**Integrantes:**

* Francisco Alvarado Ferllini
* Frander Granados Vega
* Andrés Arias Vargas
