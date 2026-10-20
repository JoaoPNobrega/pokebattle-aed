/*
 * simulacao.h
 * -----------
 * Uma partida inteira jogada pela regra do adversário, dos dois lados.
 * Serve para validar a arena sem depender de um script frágil de
 * teclado: o programa `./bin/pokebattle --simular` usa isto.
 *
 * O time automático entra no nível 50, na ordem Blastoise, Snorlax,
 * Dragonite, Venusauro, Gengar e Alakazam. Entre treinadores o HP
 * volta, como numa jornada normal.
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
