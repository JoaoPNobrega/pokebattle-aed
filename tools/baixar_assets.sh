#!/bin/sh
# Baixa sprites e dados para assets/pokemon/
set -eu
cd "$(dirname "$0")/.."
python3 tools/baixar_assets.py
python3 tools/baixar_personagens.py
