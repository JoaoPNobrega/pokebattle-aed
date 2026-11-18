#!/usr/bin/env bash
# Compila, testa e gera o pacote
set -euo pipefail

cd "$(dirname "$0")/.."
plataforma="$(uname -s)"
build=build/distribuicao
saida="$build/pacotes"
mkdir -p "$saida"

if [[ "$plataforma" == Darwin ]]; then
    arquitetura="$(uname -m)"
    trabalho="$build/macos-$arquitetura"
    app="$trabalho/PokéBattle.app"
    executavel="$app/Contents/MacOS/pokebattle"
    recursos="$app/Contents/Resources"
    pacote="Pokebattle-macOS-$arquitetura.zip"
    lib_raylib="$(pkg-config --variable=libdir raylib)"
    frameworks="-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore"

    mkdir -p "$recursos" "$app/Contents/MacOS"
    make -j2 OBJ_DIR="$trabalho/obj" BIN_DIR="$trabalho/bin" \
        LDFLAGS="$lib_raylib/libraylib.a $frameworks" teste
    cp "$trabalho/bin/pokebattle" "$executavel"
    cp -R assets "$recursos/"
    python3 tools/gerar_icone.py iconset "$trabalho/Pokebattle.iconset"
    iconutil -c icns -o "$recursos/Pokebattle.icns" "$trabalho/Pokebattle.iconset"

    cat > "$app/Contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key><string>pokebattle</string>
    <key>CFBundleIdentifier</key><string>br.com.joaopnobrega.pokebattle</string>
    <key>CFBundleName</key><string>PokéBattle</string>
    <key>CFBundleDisplayName</key><string>PokéBattle</string>
    <key>CFBundleIconFile</key><string>Pokebattle.icns</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>1.0</string>
    <key>CFBundleVersion</key><string>1</string>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
PLIST

    plutil -lint "$app/Contents/Info.plist"
    codesign --force --sign - "$app"
    codesign --verify --strict "$app"
    ditto -c -k --keepParent "$app" "$saida/$pacote"
    teste="$trabalho/teste"
    mkdir -p "$teste"
    ditto -x -k "$saida/$pacote" "$teste"
    (
        cd "$teste"
        ./PokéBattle.app/Contents/MacOS/pokebattle --simular
    ) > "$trabalho/simulacao.log"
    grep -Fx 'RESULTADO: VITORIA' "$trabalho/simulacao.log"

elif [[ "${MSYSTEM:-}" == UCRT64 ]]; then
    trabalho="$build/windows-x64"
    app="$trabalho/Pokebattle"
    executavel="$app/PokeBattle.exe"
    pacote=Pokebattle-Windows-x64.zip
    mkdir -p "$app"
    python3 tools/gerar_icone.py ico "$trabalho/Pokebattle.ico"
    printf '1 ICON "Pokebattle.ico"\n' > "$trabalho/Pokebattle.rc"
    (
        cd "$trabalho"
        windres -i Pokebattle.rc -o icone.o
    )
    # No Windows a janela abre sem console
    make -j2 OBJ_DIR="$trabalho/obj" BIN_DIR="$trabalho/bin" \
        RECURSOS_JOGO="$trabalho/icone.o" LDFLAGS_JOGO=-mwindows teste
    cp "$trabalho/bin/pokebattle.exe" "$executavel"
    cp -R assets "$app/"

    # Copia as DLLs do MSYS que o exe precisa
    copiar_dlls() {
        local binario="$1" nome dll
        local importacoes
        importacoes="$(objdump -p "$binario" | awk '/DLL Name:/ {print $3}')"
        for nome in $importacoes; do
            dll="$MINGW_PREFIX/bin/$nome"
            if [[ -f "$dll" && ! -f "$app/$nome" ]]; then
                cp "$dll" "$app/$nome"
                copiar_dlls "$app/$nome"
            fi
        done
    }
    copiar_dlls "$executavel"
    objdump -x "$executavel" > "$trabalho/cabecalho-pe.log"
    grep -F '.rsrc' "$trabalho/cabecalho-pe.log" > /dev/null
    (
        cd "$trabalho"
        zip -q -r "../pacotes/$pacote" Pokebattle
    )
    teste="$trabalho/teste"
    mkdir -p "$teste"
    unzip -oq "$saida/$pacote" -d "$teste"
    (
        cd "$teste/Pokebattle"
        PATH=/c/Windows/System32:/c/Windows ./PokeBattle.exe --simular
    )
else
    echo 'Use macOS ou o terminal MSYS2 UCRT64 no Windows.' >&2
    exit 1
fi

printf 'Pacote: %s\n' "$saida/$pacote"
