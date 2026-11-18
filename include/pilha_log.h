/* Pilha do histórico da batalha */

#ifndef PILHA_LOG_H
#define PILHA_LOG_H

#include <stddef.h>
#include <stdio.h>

#define LOG_MAX 224

typedef struct NoLog {
    char texto[LOG_MAX];
    struct NoLog *prox;
} NoLog;

typedef struct PilhaLog {
    NoLog *topo;
    int tamanho;
} PilhaLog;

void criarPilhaLog(PilhaLog *pilha);
void liberarPilhaLog(PilhaLog *pilha);

/* Empilha o lance mais novo */
int empilharLog(PilhaLog *pilha, const char *texto);

/* Tira o topo */
int desempilharLog(PilhaLog *pilha, char *destino, size_t capacidade);

/* Lê o topo sem remover */
int topoDoLog(const PilhaLog *pilha, char *destino, size_t capacidade);

int pilhaVazia(const PilhaLog *pilha);
int tamanhoPilha(const PilhaLog *pilha);

/* Imprime do antigo ao novo, com uma pilha auxiliar */
void mostrarHistorico(PilhaLog *pilha, FILE *saida);

#endif
