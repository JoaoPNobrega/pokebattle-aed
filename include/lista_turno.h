/* Lista circular do turno */

#ifndef LISTA_TURNO_H
#define LISTA_TURNO_H

#include "pokemon.h"

#define LADO_JOGADOR 0
#define LADO_OPONENTE 1

typedef struct {
    Pokemon *pokemon;
    int lado;          /* jogador ou oponente */
    int indice_golpe;  /* -1 se não age */
    int prioridade;    /* prioridade do golpe */
    int velocidade;    /* velocidade no nível */
} AtorTurno;

typedef struct NoTurno {
    AtorTurno ator;
    struct NoTurno *prox;
} NoTurno;

typedef struct ListaTurno {
    NoTurno *inicio; /* primeiro a agir */
    NoTurno *fim;    /* fim->prox volta ao início */
    int tamanho;
} ListaTurno;

void criarListaTurno(ListaTurno *lista);
void liberarListaTurno(ListaTurno *lista);

/* Insere no fim e fecha o ciclo */
int inserirFimTurno(ListaTurno *lista, const AtorTurno *ator);

NoTurno *inicioDoCiclo(const ListaTurno *lista);
NoTurno *proximoDoCiclo(const NoTurno *no);

int tamanhoListaTurno(const ListaTurno *lista);

#endif
