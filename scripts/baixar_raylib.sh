#!/bin/sh
# Baixa e compila a raylib 5.5 em third_party/ (não vai para o git).
# Linux e macOS. No Windows (MSYS2) prefira: pacman -S mingw-w64-x86_64-raylib
set -e

RAIZ=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
DESTINO="$RAIZ/third_party/raylib"
VERSAO="5.5"
URL="https://github.com/raysan5/raylib/archive/refs/tags/${VERSAO}.tar.gz"

if [ ! -f "$DESTINO/src/Makefile" ]; then
    mkdir -p "$RAIZ/third_party"
    TMP=$(mktemp -d)
    echo "Baixando raylib ${VERSAO}..."
    curl -fsSL "$URL" -o "$TMP/raylib.tar.gz"
    tar -xzf "$TMP/raylib.tar.gz" -C "$TMP"
    rm -rf "$DESTINO"
    mv "$TMP/raylib-${VERSAO}" "$DESTINO"
    rm -rf "$TMP"
fi

echo "Compilando raylib..."
# nproc existe no Linux; no macOS caímos em 2.
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
make -C "$DESTINO/src" PLATFORM=PLATFORM_DESKTOP -j"$JOBS"
echo "Pronto: $DESTINO/src/libraylib.a"
