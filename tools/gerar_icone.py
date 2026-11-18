"""Gera o ícone original de lanterna em PNG e ICO, sem dependências externas."""

import argparse
from pathlib import Path
import struct
import zlib

ESCALA_BASE = 32
TRANSPARENTE = (0, 0, 0, 0)
AZUL_ESCURO = (11, 20, 39, 255)
AZUL = (25, 41, 70, 255)
BORDA = (56, 74, 112, 255)
ROXO = (148, 110, 212, 255)
AMARELO = (255, 211, 94, 255)
CLARO = (255, 241, 185, 255)


def cor(x, y):
    """Devolve a cor de cada ponto na tela lógica de 32 por 32."""
    canto_x = min(x, 31 - x)
    canto_y = min(y, 31 - y)
    if canto_x < 1 or canto_y < 1 or canto_x + canto_y < 5:
        return TRANSPARENTE
    if canto_x == 1 or canto_y == 1 or canto_x + canto_y == 5:
        return BORDA
    if ((x, y) in {(21, 5), (24, 10), (27, 7), (27, 13)}):
        return CLARO
    # O feixe abre na diagonal a partir da boca da lanterna.
    if x >= 17 and 3 <= y <= 16 and x + y >= 25 and x + y <= 37:
        return ROXO if y < 7 or x + y >= 35 else AMARELO
    if 15 <= x <= 21 and 13 <= y <= 19 and 30 <= x + y <= 37:
        return CLARO if x + y >= 35 else AMARELO
    if 10 <= x <= 18 and 15 <= y <= 23 and 30 <= x + y <= 34:
        return BORDA if y >= 21 else (198, 208, 237, 255)
    if 6 <= x <= 14 and 19 <= y <= 26 and 29 <= x + y <= 33:
        return (101, 130, 177, 255) if x + y < 32 else BORDA
    return AZUL if y < 16 else AZUL_ESCURO


def bloco_png(tipo, dados):
    return (struct.pack(">I", len(dados)) + tipo + dados
            + struct.pack(">I", zlib.crc32(tipo + dados)))


def imagem_png(tamanho):
    pixels = b"".join(
        b"\0" + b"".join(
            bytes(cor(x * ESCALA_BASE // tamanho, y * ESCALA_BASE // tamanho))
            for x in range(tamanho)
        )
        for y in range(tamanho)
    )
    cabecalho = struct.pack(">IIBBBBB", tamanho, tamanho, 8, 6, 0, 0, 0)
    return (b"\x89PNG\r\n\x1a\n" + bloco_png(b"IHDR", cabecalho)
            + bloco_png(b"IDAT", zlib.compress(pixels, 9)) + bloco_png(b"IEND", b""))


def gravar_ico(caminho):
    tamanhos = (16, 24, 32, 48, 64, 128, 256)
    entradas = []
    imagens = []
    deslocamento = 6 + 16 * len(tamanhos)
    for tamanho in tamanhos:
        png = imagem_png(tamanho)
        dimensao = tamanho % 256
        entradas.append(struct.pack("<BBBBHHII", dimensao, dimensao, 0, 0, 1, 32,
                                    len(png), deslocamento))
        imagens.append(png)
        deslocamento += len(png)
    caminho.write_bytes(struct.pack("<HHH", 0, 1, len(tamanhos))
                        + b"".join(entradas) + b"".join(imagens))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("formato", choices=("ico", "iconset"))
    parser.add_argument("destino", type=Path)
    args = parser.parse_args()
    if args.formato == "ico":
        gravar_ico(args.destino)
        return
    args.destino.mkdir(parents=True, exist_ok=True)
    for tamanho in (16, 32, 128, 256, 512):
        for fator in (1, 2):
            escala = "@2x" if fator == 2 else ""
            arquivo = args.destino / f"icon_{tamanho}x{tamanho}{escala}.png"
            arquivo.write_bytes(imagem_png(tamanho * fator))


if __name__ == "__main__":
    main()
