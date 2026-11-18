/* Pilha do histórico */

#include "pilha_log.h"

#include "util.h"

#include <stdlib.h>

void criarPilhaLog(PilhaLog *pilha) {
    if (pilha == NULL) {
        return;
    }
    pilha->topo = NULL;
    pilha->tamanho = 0;
}

void liberarPilhaLog(PilhaLog *pilha) {
    if (pilha == NULL) {
        return;
    }

    while (!pilhaVazia(pilha)) {
        desempilharLog(pilha, NULL, 0);
    }
}

int empilharLog(PilhaLog *pilha, const char *texto) {
    NoLog *no;

    if (pilha == NULL || texto == NULL) {
        return 0;
    }

    no = (NoLog *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }

    copiarTexto(no->texto, sizeof no->texto, texto);
    no->prox = pilha->topo;
    pilha->topo = no;
    pilha->tamanho++;
    return 1;
}

int desempilharLog(PilhaLog *pilha, char *destino, size_t capacidade) {
    NoLog *no;

    if (pilha == NULL || pilha->topo == NULL) {
        return 0;
    }

    no = pilha->topo;
    if (destino != NULL && capacidade > 0) {
        copiarTexto(destino, capacidade, no->texto);
    }

    pilha->topo = no->prox;
    pilha->tamanho--;
    free(no);
    return 1;
}

int topoDoLog(const PilhaLog *pilha, char *destino, size_t capacidade) {
    if (pilha == NULL || pilha->topo == NULL || destino == NULL || capacidade == 0) {
        return 0;
    }
    copiarTexto(destino, capacidade, pilha->topo->texto);
    return 1;
}

int pilhaVazia(const PilhaLog *pilha) {
    return pilha == NULL || pilha->topo == NULL;
}

int tamanhoPilha(const PilhaLog *pilha) {
    if (pilha == NULL) {
        return 0;
    }
    return pilha->tamanho;
}

void mostrarHistorico(PilhaLog *pilha, FILE *saida) {
    PilhaLog auxiliar;
    char linha[LOG_MAX];

    if (pilha == NULL || saida == NULL) {
        return;
    }
    if (pilhaVazia(pilha)) {
        fprintf(saida, "(histórico vazio)\n");
        return;
    }

    criarPilhaLog(&auxiliar);

    /* Duas pilhas: inverte e volta na ordem em que aconteceu */
    while (desempilharLog(pilha, linha, sizeof linha)) {
        empilharLog(&auxiliar, linha);
    }

    while (desempilharLog(&auxiliar, linha, sizeof linha)) {
        fprintf(saida, "%s\n", linha);
        empilharLog(pilha, linha);
    }

    liberarPilhaLog(&auxiliar);
}
