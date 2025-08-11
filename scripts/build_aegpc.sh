#!/bin/bash

# Verzeichnis, in dem dein lokales nlohmann-json liegt
JSON_DIR="$HOME/json/single_include"

# Setze das Build-Verzeichnis
BUILD_DIR="build"

echo "Verwende nlohmann_json aus: $JSON_DIR"

# Falls das Build-Verzeichnis noch nicht existiert, erstelle es
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR" || exit 1

# CMake-Konfiguration (mit Include-Pfad)
# Falls dein CMakeLists.txt schon target_include_directories benutzt, reicht CMAKE_PREFIX_PATH nicht — besser direkt ein Flag setzen
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -Dnlohmann_json_INCLUDE_DIR="$JSON_DIR" \
    -DCMAKE_CXX_FLAGS="-I$JSON_DIR"

# Baue das Projekt mit mehreren Threads
echo "Baue das Projekt..."
cmake --build . --parallel $(nproc)

echo "Fertig!"
