/*
 * caso.h
 * ------
 * O estado do mistério, sem desenhar nada.
 *
 * Junta o que já existia (Pokédex, time, fila de confrontos, pilha
 * de histórico) com a lista de salas. A fila deixa de ser a arena
 * do filme e passa a ser a fila de lutas do caso: o capanga sai
 * primeiro; o chefão, quando existir, entra atrás dele.
 */

#ifndef CASO_H
#define CASO_H

#include "batalha.h"
#include "jogo.h"
#include "sala.h"

typedef struct {
    Jogo nucleo;
    ListaSalas salas;
    Treinador oponente_atual;
    int oponente_carregado; /* 1 = oponente_atual é dono de um time */
    int em_batalha;
    int batalha_vencida;
} Caso;

/*
 * iniciarCaso
 * Carrega a Pokédex e deixa salas, time e fila vazios.
 * Retorno: 1 se a Pokédex entrou, 0 se faltou memória.
 * Complexidade: O(n log n), n = espécies.
 */
int iniciarCaso(Caso *caso);

/*
 * encerrarCaso
 * Libera Pokédex, time, fila, log, salas e o oponente carregado.
 * Complexidade: O(n) somando essas estruturas.
 */
void encerrarCaso(Caso *caso);

/*
 * comecarCaso
 * Recomeça a investigação: time só com o Pikachu, laboratório e
 * corredor na lista de salas, um capanga na fila de lutas.
 * Retorno: 1 se montou, 0 se faltou dado ou memória.
 * Complexidade: O(1) além das liberações do estado anterior.
 */
int comecarCaso(Caso *caso);

/*
 * casoUsarHabilidade / casoAvancar / casoVoltar
 * Atalhos que operam na sala atual e anotam o histórico.
 * Retorno: 1 se a ação aconteceu, 0 se foi recusada.
 * Complexidade: O(t) na habilidade (percorre o time); O(1) ao andar.
 */
int casoUsarHabilidade(Caso *caso);
int casoAvancar(Caso *caso);
int casoVoltar(Caso *caso);

/*
 * casoIniciarBatalha
 * Se a sala atual pede luta e ela ainda não foi vencida, tira o
 * próximo treinador da fila (só na primeira vez) e entra em batalha.
 * Retorno: 1 se a luta começou, 0 se não havia luta.
 * Complexidade: O(1).
 */
int casoIniciarBatalha(Caso *caso);

/*
 * casoExecutarGolpe
 * Um turno da luta, com o golpe escolhido pelo jogador (índice no
 * Pokémon ativo). O oponente escolhe o dele por maior dano. A ordem
 * sai do insertion sort e da lista circular, como na batalha antiga.
 *
 * Retorno:
 *   1  a luta continua
 *   0  o jogador venceu
 *  -1  o jogador perdeu
 *  -2  não havia luta, ou o índice do golpe é inválido
 * Complexidade: O(g + n^2) com g <= 4 golpes e n <= 2 atores no turno.
 */
int casoExecutarGolpe(Caso *caso, int indice_golpe, SaidaBatalha *saida);

#endif
