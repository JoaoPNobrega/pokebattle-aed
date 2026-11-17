@echo off
cd /d "%~dp0"
echo Iniciando pokebattle...
bin\pokebattle.exe
echo Codigo de saida: %ERRORLEVEL%
pause
