#!/usr/bin/env python3
"""Monta os sprites de pessoa e o tileset do laboratório.

Fontes, todas de uso acadêmico (arte Nintendo/Game Freak):
- overworld e tiles: decompilação pret/pokefirered
- Giovanni na batalha: Pokémon Showdown, sprite da geração III
- Pikachu seguidor: animações de overworld de HeartGold/SoulSilver
  publicadas em pokeamice.com

O jogo em C não baixa nada. Este script só refaz os PNG locais.
"""

import struct
import urllib.request
from pathlib import Path

from PIL import Image

RAIZ = Path(__file__).resolve().parents[1]
PESSOAS = RAIZ / "assets" / "personagens"
TILES = RAIZ / "assets" / "tiles"
PESSOAS.mkdir(parents=True, exist_ok=True)
TILES.mkdir(parents=True, exist_ok=True)

PRET = "https://raw.githubusercontent.com/pret/pokefirered/master"
AGENTE = "PokebattleAcademico/1.0 (CESAR School; uso academico)"


def baixar(url, destino):
    pedido = urllib.request.Request(url, headers={"User-Agent": AGENTE})
    destino.write_bytes(urllib.request.urlopen(pedido, timeout=40).read())


def rgba_indice_zero_transparente(caminho):
    imagem = Image.open(caminho)
    if imagem.mode != "P":
        return imagem.convert("RGBA")
    imagem = imagem.convert("RGBA")
    pixels = imagem.load()
    # O índice 0 do PNG do decomp é o fundo, não uma cor do personagem.
    fundo = Image.open(caminho).convert("P")
    origem = fundo.load()
    for y in range(imagem.height):
        for x in range(imagem.width):
            if origem[x, y] == 0:
                pixels[x, y] = (0, 0, 0, 0)
    return imagem


def quadro(folha, indice, largura, altura, espelhar=False):
    recorte = folha.crop((indice * largura, 0, indice * largura + largura, altura))
    if espelhar:
        recorte = recorte.transpose(Image.FLIP_LEFT_RIGHT)
    return recorte


def folha_frlg(origem, destino):
    """A folha do decomp é uma fileira. O jogo usa 4 direções e 3 passos."""
    imagem = rgba_indice_zero_transparente(origem)
    # 0 sul parado, 3 e 4 passos ao sul, 2/7/8 oeste, 1/5/6 norte.
    linhas = [
        [quadro(imagem, 0, 16, 32), quadro(imagem, 3, 16, 32), quadro(imagem, 4, 16, 32)],
        [quadro(imagem, 2, 16, 32), quadro(imagem, 7, 16, 32), quadro(imagem, 8, 16, 32)],
        [
            quadro(imagem, 2, 16, 32, True),
            quadro(imagem, 7, 16, 32, True),
            quadro(imagem, 8, 16, 32, True),
        ],
        [quadro(imagem, 1, 16, 32), quadro(imagem, 5, 16, 32), quadro(imagem, 6, 16, 32)],
    ]
    saida = Image.new("RGBA", (48, 128), (0, 0, 0, 0))
    for linha, grupo in enumerate(linhas):
        for coluna, pedaco in enumerate(grupo):
            saida.paste(pedaco, (coluna * 16, linha * 32), pedaco)
    saida.save(destino)


def pikachu_seguidor():
    """Quatro GIFs do HeartGold: baixo, cima, esquerda, direita, 4 quadros."""
    ordem = [1, 3, 4, 2]
    saida = Image.new("RGBA", (96, 128), (0, 0, 0, 0))
    for linha, numero in enumerate(ordem):
        gif = Image.open(PESSOAS / f"_pika_{numero}.gif")
        # 0 parado, 1 passo, 3 o outro passo. O quadro 2 quase repete o parado.
        for coluna, quadro_gif in enumerate((0, 1, 3)):
            gif.seek(quadro_gif)
            pedaco = gif.convert("RGBA")
            saida.paste(pedaco, (coluna * 32, linha * 32), pedaco)
    saida.save(PESSOAS / "pikachu_overworld.png")


def costas_do_red():
    imagem = rgba_indice_zero_transparente(PESSOAS / "_red_back.png")
    # Cinco quadros de 64×64, um em cima do outro. O jogo lê na horizontal.
    saida = Image.new("RGBA", (64 * 5, 64), (0, 0, 0, 0))
    for i in range(5):
        pedaco = imagem.crop((0, i * 64, 64, i * 64 + 64))
        saida.paste(pedaco, (i * 64, 0), pedaco)
    saida.save(PESSOAS / "detetive_costas.png")


def carregar_paleta(caminho):
    linhas = caminho.read_text(encoding="utf-8").splitlines()
    cores = []
    for linha in linhas[3:]:
        if not linha.strip():
            continue
        r, g, b = (int(parte) for parte in linha.split())
        cores.append((r, g, b, 255))
    return cores


def carregar_tiles(caminho):
    imagem = Image.open(caminho)
    if imagem.mode != "P":
        imagem = imagem.convert("P")
    pixels = imagem.load()
    tiles = []
    for ty in range(imagem.height // 8):
        for tx in range(imagem.width // 8):
            tile = []
            for y in range(8):
                tile.append([pixels[tx * 8 + x, ty * 8 + y] for x in range(8)])
            tiles.append(tile)
    return tiles


def pintar(destino, dx, dy, tile, paleta, espelhar_x, espelhar_y):
    pixels = destino.load()
    for y in range(8):
        for x in range(8):
            sx = 7 - x if espelhar_x else x
            sy = 7 - y if espelhar_y else y
            indice = tile[sy][sx]
            if indice == 0 or indice >= len(paleta):
                continue
            pixels[dx + x, dy + y] = paleta[indice]


def montar_tileset():
    predio = carregar_tiles(TILES / "_building.png")
    lab = carregar_tiles(TILES / "_lab.png")
    pal_predio = [carregar_paleta(TILES / "_pals" / "building" / f"{i:02d}.pal") for i in range(16)]
    pal_lab = [carregar_paleta(TILES / "_pals" / "lab" / f"{i:02d}.pal") for i in range(16)]
    meta_predio = (TILES / "_building_meta.bin").read_bytes()
    meta_lab = (TILES / "_lab_meta.bin").read_bytes()
    total = 640 + len(meta_lab) // 16
    colunas = 32
    linhas = (total + colunas - 1) // colunas
    folha = Image.new("RGBA", (colunas * 16, linhas * 16), (0, 0, 0, 0))
    for mid in range(total):
        if mid < 640:
            palavras = struct.unpack_from("<8H", meta_predio, mid * 16)
        else:
            palavras = struct.unpack_from("<8H", meta_lab, (mid - 640) * 16)
        celula = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
        posicoes = [(0, 0), (8, 0), (0, 8), (8, 8)]
        for camada in (0, 1):
            for i in range(4):
                palavra = palavras[camada * 4 + i]
                tid = palavra & 0x3FF
                espelhar_x = bool(palavra & 0x400)
                espelhar_y = bool(palavra & 0x800)
                paleta_i = (palavra >> 12) & 0xF
                if tid >= 640:
                    if tid - 640 >= len(lab):
                        continue
                    tile = lab[tid - 640]
                    paleta = pal_lab[paleta_i]
                else:
                    if tid >= len(predio):
                        continue
                    tile = predio[tid]
                    paleta = pal_predio[paleta_i]
                pintar(celula, posicoes[i][0], posicoes[i][1], tile, paleta, espelhar_x, espelhar_y)
        folha.paste(celula, ((mid % colunas) * 16, (mid // colunas) * 16), celula)
    folha.save(TILES / "frlg_lab.png")
    print(f"tileset {folha.size} com {total} metatiles")


def baixar_tudo():
    pessoas = f"{PRET}/graphics/object_events/pics/people"
    baixar(f"{pessoas}/gentleman.png", PESSOAS / "_gentleman.png")
    baixar(f"{pessoas}/giovanni.png", PESSOAS / "_giovanni_ow.png")
    baixar(f"{PRET}/graphics/trainers/back_pics/red_back_pic.png", PESSOAS / "_red_back.png")
    baixar(
        "https://play.pokemonshowdown.com/sprites/trainers/giovanni-gen3.png",
        PESSOAS / "giovanni.png",
    )
    Image.open(PESSOAS / "giovanni.png").convert("RGBA").save(PESSOAS / "giovanni.png")
    for numero in (1, 2, 3, 4):
        baixar(
            f"https://pokeamice.com/wp-content/uploads/2023/02/25-{numero}-HGSS.gif",
            PESSOAS / f"_pika_{numero}.gif",
        )
    baixar(f"{PRET}/data/tilesets/primary/building/tiles.png", TILES / "_building.png")
    baixar(f"{PRET}/data/tilesets/primary/building/metatiles.bin", TILES / "_building_meta.bin")
    baixar(f"{PRET}/data/tilesets/secondary/lab/tiles.png", TILES / "_lab.png")
    baixar(f"{PRET}/data/tilesets/secondary/lab/metatiles.bin", TILES / "_lab_meta.bin")
    for nome, pasta in (("building", "primary/building"), ("lab", "secondary/lab")):
        destino = TILES / "_pals" / nome
        destino.mkdir(parents=True, exist_ok=True)
        for i in range(16):
            baixar(
                f"{PRET}/data/tilesets/{pasta}/palettes/{i:02d}.pal",
                destino / f"{i:02d}.pal",
            )


def limpar_temporarios():
    for caminho in PESSOAS.glob("_*"):
        caminho.unlink()
    for caminho in (TILES / "_pals").rglob("*"):
        if caminho.is_file():
            caminho.unlink()
    for nome in ("_building.png", "_building_meta.bin", "_lab.png", "_lab_meta.bin"):
        alvo = TILES / nome
        if alvo.exists():
            alvo.unlink()
    pals = TILES / "_pals"
    if pals.exists():
        for pasta in sorted(pals.rglob("*"), reverse=True):
            if pasta.is_dir():
                pasta.rmdir()
        if pals.exists():
            pals.rmdir()


def main():
    print("Baixando personagens e o laboratório da FireRed...")
    baixar_tudo()
    folha_frlg(PESSOAS / "_gentleman.png", PESSOAS / "detetive.png")
    folha_frlg(PESSOAS / "_giovanni_ow.png", PESSOAS / "giovanni_overworld.png")
    costas_do_red()
    pikachu_seguidor()
    montar_tileset()
    limpar_temporarios()
    print(f"Pronto em {PESSOAS} e {TILES}")


if __name__ == "__main__":
    main()
