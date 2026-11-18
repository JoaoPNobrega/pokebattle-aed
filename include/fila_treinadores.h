/* Fila de treinadores */

#ifndef FILA_TREINADORES_H
#define FILA_TREINADORES_H

#include "lista_time.h"

#define NOME_TREINADOR_MAX 48
#define FALA_MAX 160

typedef struct {
    char nome[NOME_TREINADOR_MAX];
    char fala[FALA_MAX];
    ListaTime time;
} Treinador;

typedef struct NoFila {
    Treinador treinador;
    struct NoFila *prox;
} NoFila;

typedef struct FilaTreinadores {
    NoFila *frente; /* próximo a sair */
    NoFila *tras;   /* último a entrar */
    int tamanho;
} FilaTreinadores;

void criarTreinador(Treinador *treinador, const char *nome, const char *fala);

void criarFilaTreinadores(FilaTreinadores *fila);
void liberarFilaTreinadores(FilaTreinadores *fila);

/* Enfileira no fim */
int enfileirarTreinador(FilaTreinadores *fila, const Treinador *treinador);

/* Tira da frente */
int desenfileirarTreinador(FilaTreinadores *fila, Treinador *destino);

/* Próximo da fila, para a luta */
int proximoTreinador(FilaTreinadores *fila, Treinador *destino);

/* Olha a frente sem remover */
const Treinador *frenteDaFila(const FilaTreinadores *fila);

int filaVazia(const FilaTreinadores *fila);
int tamanhoFila(const FilaTreinadores *fila);

#endif
