#!/bin/bash

# Setze das Build-Verzeichnis
BUILD_DIR="build"

# Überprüfe, ob nlohmann_json installiert ist
echo "Überprüfe, ob nlohmann_json installiert ist..."
if ! [ -d "/usr/include/nlohmann" ]; then
    echo "nlohmann_json ist nicht installiert. Installiere es jetzt..."
    sudo apt-get update
    sudo apt-get install -y nlohmann-json3-dev
else
    echo "nlohmann_json ist bereits installiert."
fi

# Falls das Verzeichnis noch nicht existiert, erstelle es
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"



# ich sette hier einfach den build type, obwohl dadrunter diese abfrage ist
# ich bin einfach faul!
cmake .. -DCMAKE_BUILD_TYPE=Release



# Führe CMake nur aus, wenn es noch nicht konfiguriert wurde
if [ ! -f "CMakeCache.txt" ]; then
    echo "Konfiguriere das Projekt mit CMake..."
    cmake ..
fi



# Baue das Projekt mit mehreren Threads
echo "Baue das Projekt..."
cmake --build . --parallel $(nproc)

echo "Fertig!"
