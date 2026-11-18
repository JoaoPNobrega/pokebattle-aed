/* Partida: Pokédex, time, fila e log */

#include "jogo.h"

int iniciarJogo(Jogo *jogo) {
    if (jogo == NULL) {
        return 0;
    }

    inicializarTipos();
    criarHashPokedex(&jogo->pokedex);
    criarArvorePokedex(&jogo->arvore);
    criarListaTime(&jogo->time);
    criarFilaTreinadores(&jogo->fila);
    criarPilhaLog(&jogo->log);

    if (!registrarEspecies(&jogo->pokedex, &jogo->arvore)) {
        encerrarJogo(jogo);
        return 0;
    }
    return 1;
}

void encerrarJogo(Jogo *jogo) {
    if (jogo == NULL) {
        return;
    }
    liberarListaTime(&jogo->time);
    liberarFilaTreinadores(&jogo->fila);
    liberarPilhaLog(&jogo->log);
    liberarHashPokedex(&jogo->pokedex);
    liberarArvorePokedex(&jogo->arvore);
}

int reiniciarDesafio(Jogo *jogo) {
    if (jogo == NULL) {
        return 0;
    }

    liberarListaTime(&jogo->time);
    liberarFilaTreinadores(&jogo->fila);
    liberarPilhaLog(&jogo->log);
    criarListaTime(&jogo->time);
    criarFilaTreinadores(&jogo->fila);
    criarPilhaLog(&jogo->log);

    if (!montarArena(&jogo->fila, &jogo->pokedex)) {
        liberarFilaTreinadores(&jogo->fila);
        criarFilaTreinadores(&jogo->fila);
        return 0;
    }
    return 1;
}
