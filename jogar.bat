@echo off
cd /d "%~dp0"
if not exist "bin\pokebattle.exe" (
    echo Compilando PokeBattle...
    mingw32-make all
)
start "" "bin\pokebattle.exe"
