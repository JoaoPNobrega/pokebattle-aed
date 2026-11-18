/* Insertion sort do turno e merge sort da Pokédex */

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

/* Insertion sort: prioridade, depois velocidade */
void ordenarTurnoPorInsercao(AtorTurno *vetor, int n);

/* Insertion sort de Pokémon */
void ordenarPokemonPorInsercao(Pokemon *vetor, int n, CriterioOrdenacao criterio);

/* Merge sort da Pokédex */
int ordenarPokedex(Pokemon *vetor, int n, CriterioOrdenacao criterio);

#endif
