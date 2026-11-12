#!/bin/sh
# Baixa sprites e dados da PokéAPI para assets/pokemon/.
# O jogo em C não faz HTTP: só lê o que este script gravou.
# Uso acadêmico. Créditos à PokéAPI e à Nintendo/Game Freak no README.
set -eu
cd "$(dirname "$0")/.."
exec python3 tools/baixar_assets.py
