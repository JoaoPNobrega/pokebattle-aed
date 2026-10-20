/*
 * main.c
 * ------
 * Ponto de entrada do PokéBattle: Arena do Mewtwo.
 *
 * Sem argumentos, abre o menu no terminal.
 * Com --simular, joga uma arena inteira sozinho e devolve
 * 0 se o time vencer, 1 se perder, 2 se nem conseguir montar o jogo.
 */

#include "jogo.h"
#include "simulacao.h"
#include "ui_terminal.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    Jogo jogo;
    int codigo;

    /* Em pipe o stdout vem em bloco; linha a linha deixa o teste legível. */
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc > 1 && strcmp(argv[1], "--simular") == 0) {
        int resultado = simularArenaAutomatica();
        if (resultado < 0) {
            return 2;
        }
        return resultado == 1 ? 0 : 1;
    }

    if (!iniciarJogo(&jogo)) {
        fprintf(stderr, "Não foi possível carregar a Pokédex.\n");
        return 1;
    }

    codigo = executarInterfaceTerminal(&jogo);
    encerrarJogo(&jogo);
    return codigo;
}
