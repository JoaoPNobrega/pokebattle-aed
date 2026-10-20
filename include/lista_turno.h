/*
 * lista_turno.h
 * -------------
 * Ordem de ação de um turno, numa lista circular simplesmente encadeada.
 *
 * Dentro do turno cada combatente age uma vez. A lista circular deixa
 * isso explícito: o último nó aponta de volta para o primeiro. Hoje a
 * batalha é 1 contra 1, então a lista tem até 2 nós, mas a estrutura
 * não tem esse limite. Uma batalha dupla (etapa futura) colocaria 4.
 *
 * A lista NÃO é dona dos Pokémon. Ela só guarda ponteiros para os
 * Pokémon que já vivem nos times. Liberar a lista libera os nós da
 * ordem, nunca o Pikachu em si.
 */

#ifndef LISTA_TURNO_H
#define LISTA_TURNO_H

#include "pokemon.h"

#define LADO_JOGADOR 0
#define LADO_OPONENTE 1

typedef struct {
    Pokemon *pokemon;
    int lado;          /* LADO_JOGADOR ou LADO_OPONENTE */
    int indice_golpe;  /* posição no vetor de golpes; -1 se não age */
    int prioridade;    /* copiada do golpe, para a ordenação */
    int velocidade;    /* velocidade já escalada pelo nível */
} AtorTurno;

typedef struct NoTurno {
    AtorTurno ator;
    struct NoTurno *prox;
} NoTurno;

typedef struct ListaTurno {
    NoTurno *inicio; /* primeiro a agir neste turno */
    NoTurno *fim;    /* último; fim->prox aponta para inicio */
    int tamanho;
} ListaTurno;

/*
 * criarListaTurno / liberarListaTurno
 * criar: O(1).
 * liberar: O(n). Quebra o ciclo com cuidado para não liberar o
 *          primeiro nó duas vezes.
 */
void criarListaTurno(ListaTurno *lista);
void liberarListaTurno(ListaTurno *lista);

/*
 * inserirFimTurno
 * Encaixa o ator no fim e mantém o ciclo (o novo fim aponta para o início).
 *
 * Retorno: 1 se inseriu, 0 se faltou memória.
 * Complexidade: O(1), graças ao ponteiro fim.
 */
int inserirFimTurno(ListaTurno *lista, const AtorTurno *ator);

/*
 * inicioDoCiclo / proximoDoCiclo
 * inicioDoCiclo devolve o primeiro nó (ou NULL).
 * proximoDoCiclo devolve no->prox. No último, isso é o início de novo.
 *
 * Complexidade: O(1).
 */
NoTurno *inicioDoCiclo(const ListaTurno *lista);
NoTurno *proximoDoCiclo(const NoTurno *no);

/*
 * tamanhoListaTurno
 * Retorno: quantidade de atores no ciclo.
 * Complexidade: O(1).
 */
int tamanhoListaTurno(const ListaTurno *lista);

#endif
