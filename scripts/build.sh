#!/bin/bash

# Setze das Build-Verzeichnis
BUILD_DIR="build"

# Falls das Verzeichnis noch nicht existiert, erstelle es
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Führe CMake nur aus, wenn es noch nicht konfiguriert wurde
if [ ! -f "CMakeCache.txt" ]; then
    echo "Konfiguriere das Projekt mit CMake..."
    cmake ..
fi

# Baue das Projekt mit mehreren Threads
echo "Baue das Projekt..."
cmake --build . --parallel $(nproc)

echo "Fertig!"
