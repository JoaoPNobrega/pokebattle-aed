/* Insertion sort e merge sort */

#include "ordenacao.h"

#include <stdlib.h>
#include <string.h>

static int compararPokemon(const Pokemon *a, const Pokemon *b, CriterioOrdenacao criterio) {
    switch (criterio) {
    case ORDENAR_POR_NUMERO:
        return (a->numero > b->numero) - (a->numero < b->numero);
    case ORDENAR_POR_NOME:
        return strcmp(a->nome, b->nome);
    case ORDENAR_POR_VELOCIDADE:
        /* Maior velocidade primeiro */
        return (b->velocidade > a->velocidade) - (b->velocidade < a->velocidade);
    case ORDENAR_POR_NIVEL:
        return (b->nivel > a->nivel) - (b->nivel < a->nivel);
    default:
        return 0;
    }
}

/* Insertion sort estável: empate não troca */
static int atorVemAntes(const AtorTurno *a, const AtorTurno *b) {
    if (a->prioridade != b->prioridade) {
        return a->prioridade > b->prioridade;
    }
    return a->velocidade > b->velocidade;
}

void ordenarTurnoPorInsercao(AtorTurno *vetor, int n) {
    int i;

    if (vetor == NULL || n <= 1) {
        return;
    }

    for (i = 1; i < n; i++) {
        AtorTurno chave = vetor[i];
        int j = i - 1;

        while (j >= 0 && atorVemAntes(&chave, &vetor[j])) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }
}

void ordenarPokemonPorInsercao(Pokemon *vetor, int n, CriterioOrdenacao criterio) {
    int i;

    if (vetor == NULL || n <= 1) {
        return;
    }

    for (i = 1; i < n; i++) {
        Pokemon chave = vetor[i];
        int j = i - 1;

        while (j >= 0 && compararPokemon(&chave, &vetor[j], criterio) < 0) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }
}

static void mesclar(Pokemon *vetor, Pokemon *aux, int inicio, int meio, int fim,
                    CriterioOrdenacao criterio) {
    int i = inicio;
    int j = meio + 1;
    int k = inicio;
    int t;

    while (i <= meio && j <= fim) {
        /* Merge estável: empate fica com a esquerda */
        if (compararPokemon(&vetor[j], &vetor[i], criterio) < 0) {
            aux[k] = vetor[j];
            j++;
        } else {
            aux[k] = vetor[i];
            i++;
        }
        k++;
    }

    while (i <= meio) {
        aux[k] = vetor[i];
        i++;
        k++;
    }
    while (j <= fim) {
        aux[k] = vetor[j];
        j++;
        k++;
    }

    for (t = inicio; t <= fim; t++) {
        vetor[t] = aux[t];
    }
}

static void mergeSortRec(Pokemon *vetor, Pokemon *aux, int inicio, int fim,
                         CriterioOrdenacao criterio) {
    int meio;

    if (inicio >= fim) {
        return;
    }

    meio = inicio + (fim - inicio) / 2;
    mergeSortRec(vetor, aux, inicio, meio, criterio);
    mergeSortRec(vetor, aux, meio + 1, fim, criterio);
    mesclar(vetor, aux, inicio, meio, fim, criterio);
}

int ordenarPokedex(Pokemon *vetor, int n, CriterioOrdenacao criterio) {
    Pokemon *aux;

    if (vetor == NULL || n < 0) {
        return 0;
    }
    if (n <= 1) {
        return 1;
    }

    aux = (Pokemon *)malloc((size_t)n * sizeof *aux);
    if (aux == NULL) {
        return 0;
    }

    mergeSortRec(vetor, aux, 0, n - 1, criterio);
    free(aux);
    return 1;
}
