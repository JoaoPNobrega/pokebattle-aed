#!/usr/bin/env python3
"""Baixa sprites e dados da PokéAPI para assets/pokemon/.

O jogo em C só lê esses arquivos. Não há HTTP em tempo de execução.
Uso acadêmico: créditos à PokéAPI e à Nintendo/Game Freak no README.
"""

import json
import pathlib
import urllib.request

AGENTE = "PokebattleAcademico/1.0 (CESAR School; uso academico)"

RAIZ = pathlib.Path(__file__).resolve().parents[1]
DESTINO = RAIZ / "assets" / "pokemon"
DESTINO.mkdir(parents=True, exist_ok=True)

# Números da Pokédex usados em dados_pokemon.c
NUMEROS = [
    1, 3, 4, 6, 7, 9, 18, 25, 52, 54,
    65, 68, 74, 94, 95, 130, 143, 149, 150, 151,
]

SPRITES = "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon"


def abrir(url):
    pedido = urllib.request.Request(url, headers={"User-Agent": AGENTE})
    return urllib.request.urlopen(pedido, timeout=40)


def baixar(url, caminho):
    try:
        with abrir(url) as resposta:
            dados = resposta.read()
    except Exception as erro:
        print(f"falhou {url}: {erro}")
        return False
    caminho.write_bytes(dados)
    return True


def sprite_firered(numero, costas):
    pasta = f"{SPRITES}/versions/generation-iii/firered-leafgreen"
    if costas:
        return f"{pasta}/back/{numero}.png"
    return f"{pasta}/{numero}.png"


def main():
    linhas = [
        "# numero nome_api tipo1 tipo2 hp ataque defesa ataque_especial defesa_especial velocidade",
        "# Sprites: {numero}_front.png {numero}_back.png {numero}_icon.png",
        "# Fonte: https://pokeapi.co  —  arte Nintendo/Game Freak, uso acadêmico",
    ]
    for numero in NUMEROS:
        url = f"https://pokeapi.co/api/v2/pokemon/{numero}"
        print(f"PokéAPI {numero}")
        with abrir(url) as resposta:
            dados = json.load(resposta)
        tipos = [t["type"]["name"] for t in dados["types"]]
        while len(tipos) < 2:
            tipos.append("-")
        stats = {s["stat"]["name"]: s["base_stat"] for s in dados["stats"]}
        linhas.append(
            "{n} {nome} {t1} {t2} {hp} {atk} {defe} {spa} {spd} {spe}".format(
                n=numero,
                nome=dados["name"],
                t1=tipos[0],
                t2=tipos[1],
                hp=stats["hp"],
                atk=stats["attack"],
                defe=stats["defense"],
                spa=stats["special-attack"],
                spd=stats["special-defense"],
                spe=stats["speed"],
            )
        )
        frente = DESTINO / f"{numero}_front.png"
        costas = DESTINO / f"{numero}_back.png"
        icone = DESTINO / f"{numero}_icon.png"
        if not baixar(sprite_firered(numero, False), frente):
            baixar(f"{SPRITES}/{numero}.png", frente)
        if not baixar(sprite_firered(numero, True), costas):
            baixar(f"{SPRITES}/back/{numero}.png", costas)
        baixar(
            f"{SPRITES}/versions/generation-vii/icons/{numero}.png",
            icone,
        )
    try:
        from PIL import Image
    except ImportError:
        Image = None
    if Image is not None:
        for png in DESTINO.glob("*.png"):
            Image.open(png).convert("RGBA").save(png)
    (DESTINO / "dados.txt").write_text("\n".join(linhas) + "\n", encoding="utf-8")
    print(f"Pronto em {DESTINO}")


if __name__ == "__main__":
    main()
