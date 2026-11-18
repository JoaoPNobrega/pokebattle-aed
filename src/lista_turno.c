/* Lista circular do turno */

#include "lista_turno.h"

#include <stdlib.h>

void criarListaTurno(ListaTurno *lista) {
    if (lista == NULL) {
        return;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

void liberarListaTurno(ListaTurno *lista) {
    NoTurno *comeco;
    NoTurno *no;

    if (lista == NULL) {
        return;
    }
    if (lista->inicio == NULL) {
        criarListaTurno(lista);
        return;
    }

    comeco = lista->inicio;
    no = comeco;
    do {
        NoTurno *prox = no->prox;
        free(no);
        no = prox;
    } while (no != comeco);

    criarListaTurno(lista);
}

int inserirFimTurno(ListaTurno *lista, const AtorTurno *ator) {
    NoTurno *no;

    if (lista == NULL || ator == NULL) {
        return 0;
    }

    no = (NoTurno *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }
    no->ator = *ator;

    /* Um nó aponta para si; os outros fecham o ciclo no início */
    if (lista->inicio == NULL) {
        no->prox = no;
        lista->inicio = no;
        lista->fim = no;
    } else {
        no->prox = lista->inicio;
        lista->fim->prox = no;
        lista->fim = no;
    }
    lista->tamanho++;
    return 1;
}

NoTurno *inicioDoCiclo(const ListaTurno *lista) {
    if (lista == NULL) {
        return NULL;
    }
    return lista->inicio;
}

NoTurno *proximoDoCiclo(const NoTurno *no) {
    if (no == NULL) {
        return NULL;
    }
    return no->prox;
}

int tamanhoListaTurno(const ListaTurno *lista) {
    if (lista == NULL) {
        return 0;
    }
    return lista->tamanho;
}
