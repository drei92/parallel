#!/bin/bash
# Instala las dependencias de Python necesarias para graficar y generar tablas
sudo apt update
sudo apt install -y python3 python3-pip
pip3 install --user pandas matplotlib
