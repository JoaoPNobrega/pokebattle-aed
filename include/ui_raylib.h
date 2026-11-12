/*
 * ui_raylib.h
 * -----------
 * A janela do jogo. Só esta camada chama a raylib.
 *
 * A regra (sala, puzzle, dano, ordem do turno) mora em caso.c e
 * batalha.c. Trocar um retângulo de lugar não muda o mistério.
 */

#ifndef UI_RAYLIB_H
#define UI_RAYLIB_H

/*
 * executarInterfaceRaylib
 * Abre a janela, mostra o menu e devolve quando o jogador sai.
 *
 * Retorno: 0 em saída normal, 1 se a Pokédex não carregou.
 */
int executarInterfaceRaylib(void);

#endif
