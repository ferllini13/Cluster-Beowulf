#!/bin/bash

##########################
# Variables              #
##########################

INSTALL='sudo install --owner=root --group=root --mode=644'


##########################
# Check permissions      #
##########################

# Check for permissions errors
if [ `id -u` == 0 ]; then
    echo "[ERROR] This script should not be executed as root. Run it a a sudo-capable user."
    exit 1
fi

# Check if user can do sudo
echo "Se necesitan permisos de administrador"
if [ `sudo id -u` != 0 ]; then
    echo "This user cannot cast sudo or you typed an incorrect password (several times)."
    exit 1
else
    echo "Correctly authenticated."
fi

echo "Se descargan paquetes de OpenCV para la instalacion"
wget -O opencv.zip https://github.com/opencv/opencv/archive/3.4.1.zip
echo "primera descarga completa"
wget -O opencv_contrib.zip https://github.com/Itseez/opencv_contrib/archive/3.4.1.zip
echo "segunda descarga completa, se procede a la compilacion"
unzip opencv.zip
opencv-3.4.1/ opencv
mv opencv /opt/
unzip opencv_contrib.zip
mv opencv_contrib-3.4.1/ opencv_contrib
mv opencv_contrib /opt/
cd /opt/opencv
sudo mkdir release && cd release
sudo cmake -D BUILD_TIFF=ON -D WITH_CUDA=OFF -D ENABLE_AVX=OFF -D WITH_OPENGL=OFF -D WITH_OPENCL=OFF -D WITH_IPP=OFF -D WITH_TBB=ON -D BUILD_TBB=ON -D WITH_EIGEN=OFF -D WITH_V4L=OFF -D WITH_VTK=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules /opt/opencv/
sudo make -j4
sudo make install
sudo ldconfig
echo "verificando version instalada"
pkg-config --modversion opencv
echo "instalacion terminada"