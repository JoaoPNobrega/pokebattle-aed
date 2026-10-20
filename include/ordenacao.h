/*
 * ordenacao.h
 * -----------
 * Dois algoritmos feitos à mão, os dois dentro da regra do jogo.
 *
 * Insertion sort — quem age primeiro no turno.
 *   A chave é (prioridade do golpe, velocidade). Ataque Rápido tem
 *   prioridade 1 e por isso sai na frente mesmo de um Pokémon mais
 *   rápido. O algoritmo é estável: empate de prioridade e velocidade
 *   mantém a ordem original do vetor. A batalha coloca o jogador
 *   antes no vetor, então o empate favorece o jogador.
 *   Também reordena o time (ordenarTime), mudando quem entra primeiro.
 *   Não é usado para placar nem ranking.
 *
 * Merge sort — organizar a Pokédex.
 *   A hash não tem ordem. O merge sort rearruma um vetor copiado dela
 *   por número, nome ou velocidade. É estável: espécies empatadas
 *   ficam na ordem em que estavam.
 */

#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "lista_turno.h"
#include "pokemon.h"

typedef enum {
    ORDENAR_POR_NUMERO,
    ORDENAR_POR_NOME,
    ORDENAR_POR_VELOCIDADE,
    ORDENAR_POR_NIVEL
} CriterioOrdenacao;

/*
 * ordenarTurnoPorInsercao
 * Ordena os atores do turno: maior prioridade primeiro; em empate de
 * prioridade, maior velocidade primeiro.
 *
 * Retorno: nenhum. Vetor NULL ou n <= 1 não muda nada.
 * Complexidade: O(n^2) no pior caso, O(n) se já estiver ordenado.
 */
void ordenarTurnoPorInsercao(AtorTurno *vetor, int n);

/*
 * ordenarPokemonPorInsercao
 * Insertion sort de Pokémon. Por velocidade e por nível, o maior vem
 * primeiro. Por número e por nome, a ordem é crescente.
 *
 * Complexidade: O(n^2).
 */
void ordenarPokemonPorInsercao(Pokemon *vetor, int n, CriterioOrdenacao criterio);

/*
 * ordenarPokedex
 * Merge sort pelo critério pedido. Aloca um vetor auxiliar e libera
 * antes de voltar.
 *
 * Retorno: 1 se ordenou, 0 se faltou memória. n <= 1 devolve 1.
 * Complexidade: O(n log n) em tempo e O(n) de memória extra.
 */
int ordenarPokedex(Pokemon *vetor, int n, CriterioOrdenacao criterio);

#endif
