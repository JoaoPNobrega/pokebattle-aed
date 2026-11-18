/* Fila de treinadores */

#include "fila_treinadores.h"

#include "util.h"

#include <stdlib.h>

void criarTreinador(Treinador *treinador, const char *nome, const char *fala) {
    if (treinador == NULL) {
        return;
    }
    criarListaTime(&treinador->time);
    copiarTexto(treinador->nome, sizeof treinador->nome, nome);
    copiarTexto(treinador->fala, sizeof treinador->fala, fala);
}

void criarFilaTreinadores(FilaTreinadores *fila) {
    if (fila == NULL) {
        return;
    }
    fila->frente = NULL;
    fila->tras = NULL;
    fila->tamanho = 0;
}

void liberarFilaTreinadores(FilaTreinadores *fila) {
    if (fila == NULL) {
        return;
    }

    while (!filaVazia(fila)) {
        /* Sem destino, libera o time */
        desenfileirarTreinador(fila, NULL);
    }
}

int enfileirarTreinador(FilaTreinadores *fila, const Treinador *treinador) {
    NoFila *no;

    if (fila == NULL || treinador == NULL) {
        return 0;
    }

    no = (NoFila *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }

    no->treinador = *treinador;
    no->prox = NULL;

    if (fila->tras != NULL) {
        fila->tras->prox = no;
    } else {
        fila->frente = no;
    }
    fila->tras = no;
    fila->tamanho++;
    return 1;
}

int desenfileirarTreinador(FilaTreinadores *fila, Treinador *destino) {
    NoFila *no;

    if (fila == NULL || fila->frente == NULL) {
        return 0;
    }

    no = fila->frente;
    if (destino != NULL) {
        *destino = no->treinador;
    } else {
        liberarListaTime(&no->treinador.time);
    }

    fila->frente = no->prox;
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }
    fila->tamanho--;
    free(no);
    return 1;
}

int proximoTreinador(FilaTreinadores *fila, Treinador *destino) {
    return desenfileirarTreinador(fila, destino);
}

const Treinador *frenteDaFila(const FilaTreinadores *fila) {
    if (fila == NULL || fila->frente == NULL) {
        return NULL;
    }
    return &fila->frente->treinador;
}

int filaVazia(const FilaTreinadores *fila) {
    return fila == NULL || fila->frente == NULL;
}

int tamanhoFila(const FilaTreinadores *fila) {
    if (fila == NULL) {
        return 0;
    }
    return fila->tamanho;
}
