/*
 * simulacao.h
 * -----------
 * Uma partida inteira sem janela: desafios espaciais, refletores,
 * captura, dois guardas e Giovanni. Os dois lados escolhem o golpe de
 * maior dano. `./bin/pokebattle --simular` usa isto.
 */

#ifndef SIMULACAO_H
#define SIMULACAO_H
#include "caso.h"

/* simularPuzzleAtual: executa uma solução reproduzível pelas ações públicas.
 * Parâmetro: caso. Retorno: 1 resolveu, 0 falhou. O(v*(v+e)), grades fixas.
 * Apenas testes/--simular/--capturas usam esta rotina; a janela jogável não usa. */
int simularPuzzleAtual(Caso *caso);
/* simularCaptura: envia os cinco comandos válidos e confirma a captura.
 * Parâmetro: caso. Retorno: 1 capturou, 0 indisponível. O(1). Sem relógio. */
int simularCaptura(Caso *caso);

/*
 * simularArenaAutomatica
 * Monta o jogo, joga a arena e libera tudo.
 *
 * Retorno: 1 vitória, 0 derrota, -1 falha ao montar (memória ou dado).
 * Complexidade: O(s * (t + v*(v+e) + b)), s salas, t time, v/e grade e b turnos.
 */
int simularArenaAutomatica(void);

#endif
