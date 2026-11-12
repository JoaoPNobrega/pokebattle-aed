#!/usr/bin/env python3
"""Gera o detetive, o Giovanni e a Poké Bola em pixel art.

Não são sprites da Nintendo. Os Pokémon vêm da PokéAPI
(tools/baixar_assets.py). Estes três são desenhados para o projeto.
"""

import pathlib

from PIL import Image

RAIZ = pathlib.Path(__file__).resolve().parents[1]
DESTINO = RAIZ / "assets" / "personagens"
DESTINO.mkdir(parents=True, exist_ok=True)

C = {
    ".": (0, 0, 0, 0),
    "k": (16, 12, 20, 255),
    "h": (52, 32, 24, 255),
    "H": (92, 58, 36, 255),
    "s": (232, 186, 142, 255),
    "S": (255, 214, 176, 255),
    "e": (32, 28, 36, 255),
    "b": (28, 44, 96, 255),
    "B": (48, 72, 148, 255),
    "c": (72, 104, 184, 255),
    "w": (236, 236, 228, 255),
    "p": (36, 36, 48, 255),
    "P": (58, 58, 74, 255),
    "f": (20, 18, 22, 255),
    "r": (176, 32, 40, 255),
    "R": (220, 48, 52, 255),
    "g": (72, 76, 84, 255),
    "G": (108, 112, 122, 255),
    "n": (28, 30, 36, 255),
    "N": (48, 50, 58, 255),
    "m": (90, 42, 36, 255),
}


def desenhar(linhas, escala=1):
    altura = len(linhas)
    largura = len(linhas[0])
    imagem = Image.new("RGBA", (largura * escala, altura * escala), (0, 0, 0, 0))
    for y, linha in enumerate(linhas):
        if len(linha) != largura:
            raise SystemExit(f"linha {y} tem {len(linha)}, esperado {largura}: {linha}")
        for x, ch in enumerate(linha):
            cor = C[ch]
            if cor[3] == 0:
                continue
            for dy in range(escala):
                for dx in range(escala):
                    imagem.putpixel((x * escala + dx, y * escala + dy), cor)
    return imagem


def espelhar(linhas):
    return [linha[::-1] for linha in linhas]


# 16×32. Pés na última linha. Três quadros: parado, passo esquerdo, passo direito.
DET_BAIXO = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHssssHh....",
    "...hHsSeSeHHh...",
    "...hHssssssHh...",
    "....hHssssHh....",
    ".....hSSSSh.....",
    "......ssss......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBwwwwBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "....pp....pp....",
    "....pp....pp....",
    "....PP....PP....",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

DET_BAIXO_A = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHssssHh....",
    "...hHsSeSeHHh...",
    "...hHssssssHh...",
    "....hHssssHh....",
    ".....hSSSSh.....",
    "......ssss......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBwwwwBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "....pp..........",
    "....pp..........",
    "....PP..........",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "..........pp....",
    "..........PP....",
    "..........PP....",
    "..........ff....",
    ".........ff.....",
    "................",
]

DET_BAIXO_B = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHssssHh....",
    "...hHsSeSeHHh...",
    "...hHssssssHh...",
    "....hHssssHh....",
    ".....hSSSSh.....",
    "......ssss......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBwwwwBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "..........pp....",
    "..........pp....",
    "..........PP....",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "....pp..........",
    "....PP..........",
    "....PP..........",
    "....ff..........",
    ".....ff.........",
    "................",
]

DET_CIMA = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHHHHHHh....",
    "...hHHHHHHHHh...",
    "...hHHhhhhHHh...",
    "....hHHHHHHh....",
    ".....hHHHHh.....",
    "......hhhh......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "....pp....pp....",
    "....pp....pp....",
    "....PP....PP....",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

DET_CIMA_A = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHHHHHHh....",
    "...hHHHHHHHHh...",
    "...hHHhhhhHHh...",
    "....hHHHHHHh....",
    ".....hHHHHh.....",
    "......hhhh......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "....pp..........",
    "....pp..........",
    "....PP..........",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "..........pp....",
    "..........PP....",
    "..........PP....",
    "..........ff....",
    ".........ff.....",
    "................",
]

DET_CIMA_B = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHHh....",
    "....hHHHHHHh....",
    "...hHHHHHHHHh...",
    "...hHHhhhhHHh...",
    "....hHHHHHHh....",
    ".....hHHHHh.....",
    "......hhhh......",
    "....bbbbbbbb....",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBBBBBBBb...",
    "...bBBccccBBb...",
    "...bBBBBBBBBb...",
    "....bBBBBBBb....",
    "....bb....bb....",
    "..........pp....",
    "..........pp....",
    "..........PP....",
    "....PP....PP....",
    "....PP....PP....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "....pp..........",
    "....PP..........",
    "....PP..........",
    "....ff..........",
    ".....ff.........",
    "................",
]

DET_ESQ = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHH.....",
    "....hHssssH.....",
    "...hHsSeeSH.....",
    "...hHsssssH.....",
    "....hHsssSH.....",
    ".....hSSSh......",
    "......ss........",
    "....bbbbbb......",
    "...bBBBBBBb.....",
    "...bBBwwBBb.....",
    "...bBBBBBBb.....",
    "...bBBccBBb.....",
    "...bBBBBBBb.....",
    "....bBBBBb......",
    ".....bb.bb......",
    ".....pp.pp......",
    ".....pp.pp......",
    ".....PP.PP......",
    ".....PP.PP......",
    ".....PP.PP......",
    ".....ff.ff......",
    "....ff...ff.....",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

DET_ESQ_A = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHH.....",
    "....hHssssH.....",
    "...hHsSeeSH.....",
    "...hHsssssH.....",
    "....hHsssSH.....",
    ".....hSSSh......",
    "......ss........",
    "....bbbbbb......",
    "...bBBBBBBb.....",
    "...bBBwwBBb.....",
    "...bBBBBBBb.....",
    "...bBBccBBb.....",
    "...bBBBBBBb.....",
    "....bBBBBb......",
    ".....bb.bb......",
    ".....pp.........",
    ".....pp.........",
    ".....PP.PP......",
    ".....PP.PP......",
    ".....ff.PP......",
    "....ff..ff......",
    "................",
    ".........pp.....",
    ".........PP.....",
    ".........PP.....",
    ".........ff.....",
    "........ff......",
    "................",
    "................",
]

DET_ESQ_B = [
    "................",
    "................",
    ".....hhhhhh.....",
    "....hHHHHHH.....",
    "....hHssssH.....",
    "...hHsSeeSH.....",
    "...hHsssssH.....",
    "....hHsssSH.....",
    ".....hSSSh......",
    "......ss........",
    "....bbbbbb......",
    "...bBBBBBBb.....",
    "...bBBwwBBb.....",
    "...bBBBBBBb.....",
    "...bBBccBBb.....",
    "...bBBBBBBb.....",
    "....bBBBBb......",
    ".....bb.bb......",
    "........pp......",
    "........pp......",
    ".....PP.PP......",
    ".....PP.PP......",
    ".....PP.ff......",
    ".....ff..ff.....",
    "................",
    "....pp..........",
    "....PP..........",
    "....PP..........",
    "....ff..........",
    ".....ff.........",
    "................",
    "................",
]

# Giovanni de costas/frente no overworld, 16×32, terno e gravata.
GIO_BAIXO = [
    "................",
    "................",
    "......nnnn......",
    ".....nNNNNn.....",
    "....nNssssNn....",
    "...nNsSeSeNNn...",
    "...nNssssssNn...",
    "....nNsmmsNn....",
    ".....nSSSSn.....",
    "......ssss......",
    "....gggggggg....",
    "...gGGGGGGGGg...",
    "...gGGwwwwGGg...",
    "...gGGwRRwGGg...",
    "...gGGwRRwGGg...",
    "...gGGGGGGGGg...",
    "....gGGGGGGg....",
    "....gg....gg....",
    "....nn....nn....",
    "....nn....nn....",
    "....NN....NN....",
    "....NN....NN....",
    "....NN....NN....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

GIO_CIMA = [
    "................",
    "................",
    "......nnnn......",
    ".....nNNNNn.....",
    "....nNNNNNNn....",
    "...nNNNNNNNNn...",
    "...nNNnnnnNNn...",
    "....nNNNNNNn....",
    ".....nNNNNn.....",
    "......nnnn......",
    "....gggggggg....",
    "...gGGGGGGGGg...",
    "...gGGGGGGGGg...",
    "...gGGGGGGGGg...",
    "...gGGggggGGg...",
    "...gGGGGGGGGg...",
    "....gGGGGGGg....",
    "....gg....gg....",
    "....nn....nn....",
    "....nn....nn....",
    "....NN....NN....",
    "....NN....NN....",
    "....NN....NN....",
    "....ff....ff....",
    "...ff......ff...",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

GIO_ESQ = [
    "................",
    "................",
    ".....nnnnn......",
    "....nNNNNN......",
    "....nNsssN......",
    "...nNsSeeN......",
    "...nNssssN......",
    "....nNsmSN......",
    ".....nSSSn......",
    "......ss........",
    "....gggggg......",
    "...gGGGGGg......",
    "...gGGwGGg......",
    "...gGGRGGg......",
    "...gGGRGGg......",
    "...gGGGGGg......",
    "....gGGGGg......",
    ".....gg.gg......",
    ".....nn.nn......",
    ".....nn.nn......",
    ".....NN.NN......",
    ".....NN.NN......",
    ".....NN.NN......",
    ".....ff.ff......",
    "....ff...ff.....",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
]

# Sprite de batalha desenhado em 32×48 e ampliado ×2 (filtro de ponto),
# para ficar no tamanho de um treinador do GBA sem parecer bloco liso.
GIOVANNI_FRENTE = [
    ".........kkkkkkkkk..............",
    ".......kkhhhhhhhhhkk............",
    "......khhhhsssssshhhk...........",
    ".....khhssseeeessshhk...........",
    ".....khssseeeeesssshk...........",
    ".....kssssssssssssssk...........",
    ".....kssssnnnnsssssk............",
    "......kksssssssskk..............",
    "......kgggggggggggk.............",
    ".....kgGGGGGGGGGGGgk............",
    "....kgGGGwwwwwwGGGGgk...........",
    "....kgGGGwrrrrwGGGGgk...........",
    "....kgGGGGwrrwGGGGGgk...........",
    "....kgGGGGGGGGGGGGGGgk..........",
    "...kgssGGGGGGGGGGGGssgk.........",
    "...ksssggggggggggggsssk.........",
    "....kppppppppppppppk............",
    "....kppppppppppppppk............",
    "....kppppkkkkkkppppk............",
    "....kpppk......kpppk............",
    "....kpppk......kpppk............",
    "....kNNNk......kNNNk............",
    "....kNNNk......kNNNk............",
    "....kNNNk......kNNNk............",
    "....kfffk......kfffk............",
    "...kffffk......kffffk...........",
    "...kffffk......kffffk...........",
    "....kkkk........kkkk............",
    "................................",
    "................................",
    "................................",
    "................................",
]


def giovanni_batalha():
    miudo = desenhar(GIOVANNI_FRENTE)
    return miudo.resize((64, 64), Image.NEAREST)


POKEBOLA = [
    "................",
    ".....kkkkkk.....",
    "...kkRRRRRRkk...",
    "..kRRRRRRRRRRk..",
    ".kRRRRRRRRRRRRk.",
    ".kRRRRkkkkRRRRk.",
    "kRRRRkwwwwkRRRRk",
    "kkkkkkwwwwkkkkkk",
    "kwwwwwwwwwwwwwwk",
    ".kwwwwkwwkwwwwk.",
    ".kwwwkkwwkkwwwk.",
    "..kwwwwwwwwwwk..",
    "...kkwwwwwwkk...",
    ".....kkkkkk.....",
    "................",
    "................",
]


def main():
    quadros_baixo = [DET_BAIXO, DET_BAIXO_A, DET_BAIXO_B]
    quadros_cima = [DET_CIMA, DET_CIMA_A, DET_CIMA_B]
    quadros_esq = [DET_ESQ, DET_ESQ_A, DET_ESQ_B]
    quadros_dir = [espelhar(q) for q in quadros_esq]
    folha = Image.new("RGBA", (16 * 3, 32 * 4), (0, 0, 0, 0))
    linhas = [quadros_baixo, quadros_esq, quadros_dir, quadros_cima]
    for linha, grupo in enumerate(linhas):
        for coluna, quadro in enumerate(grupo):
            folha.paste(desenhar(quadro), (coluna * 16, linha * 32))
    folha.save(DESTINO / "detetive.png")

    gio = Image.new("RGBA", (16 * 3, 32 * 4), (0, 0, 0, 0))
    grupos = [
        [GIO_BAIXO, GIO_BAIXO, GIO_BAIXO],
        [GIO_ESQ, GIO_ESQ, GIO_ESQ],
        [espelhar(GIO_ESQ), espelhar(GIO_ESQ), espelhar(GIO_ESQ)],
        [GIO_CIMA, GIO_CIMA, GIO_CIMA],
    ]
    for linha, grupo in enumerate(grupos):
        for coluna, quadro in enumerate(grupo):
            gio.paste(desenhar(quadro), (coluna * 16, linha * 32))
    gio.save(DESTINO / "giovanni_overworld.png")
    giovanni_batalha().save(DESTINO / "giovanni.png")
    desenhar(POKEBOLA).save(DESTINO / "pokebola.png")
    print(DESTINO)


if __name__ == "__main__":
    main()
