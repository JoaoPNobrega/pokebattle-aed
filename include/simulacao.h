/*
 * simulacao.h
 * -----------
 * Uma partida inteira sem janela: puzzle do laboratório, avanço de
 * sala e luta contra o capanga. Os dois lados escolhem o golpe de
 * maior dano. `./bin/pokebattle --simular` usa isto.
 */

#ifndef SIMULACAO_H
#define SIMULACAO_H

/*
 * simularArenaAutomatica
 * Monta o jogo, joga a arena e libera tudo.
 *
 * Retorno: 1 vitória, 0 derrota, -1 falha ao montar (memória ou dado).
 */
int simularArenaAutomatica(void);

#endif
