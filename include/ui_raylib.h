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
 * Abre o jogo, mostra o menu e devolve quando o jogador sai.
 * Parâmetro: tela_cheia diferente de zero abre em tela cheia sem mudar o monitor.
 * Complexidade: O(quadros), com custo de desenho por quadro limitado pela tela.
 *
 * Retorno: 0 em saída normal, 1 se a Pokédex não carregou.
 */
int executarInterfaceRaylib(int tela_cheia);

/*
 * capturarTelasDemonstracao
 * Abre a janela, desenha menu, sala, puzzle resolvido e batalha,
 * e grava um PNG de cada uma no diretório atual. Serve para o xvfb.
 *
 * Retorno: 0 se gravou, 1 se a Pokédex não carregou.
 */
int capturarTelasDemonstracao(void);

#endif
