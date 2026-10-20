/*
 * ui_terminal.h
 * -------------
 * A única parte do programa que lê o teclado e escreve no terminal.
 *
 * O resto do jogo devolve estados e textos pela pilha de log e pelos
 * narradores. Quando a etapa do raylib chegar, este arquivo ganha um
 * irmão (ui_raylib.c) e a batalha continua a mesma.
 */

#ifndef UI_TERMINAL_H
#define UI_TERMINAL_H

#include "jogo.h"

/*
 * executarInterfaceTerminal
 * Menu principal. Sai com a opção 5 ou quando a entrada acaba.
 *
 * Retorno: 0 em saída normal.
 * Complexidade: depende do que o jogador fizer.
 */
int executarInterfaceTerminal(Jogo *jogo);

#endif
