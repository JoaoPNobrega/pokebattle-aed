/*
 * main.c
 * ------
 * PokéBattle: O Caso do Laboratório.
 *
 * Sem argumentos, abre a janela (raylib).
 * Com --simular, joga o caso sozinho, sem janela, e devolve
 * 0 se o detetive vence o capanga, 1 se perde, 2 se nem monta o caso.
 */

#include "simulacao.h"
#include "ui_raylib.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc > 1 && strcmp(argv[1], "--simular") == 0) {
        int resultado = simularArenaAutomatica();
        if (resultado < 0) {
            return 2;
        }
        return resultado == 1 ? 0 : 1;
    }

    return executarInterfaceRaylib();
}
