/*
 * sala.c
 * ------
 * Lista dupla das salas e a ponte tipo -> habilidade. Ver sala.h.
 */

#include "sala.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void criarListaSalas(ListaSalas *lista) {
    if (lista == NULL) {
        return;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->atual = NULL;
    lista->tamanho = 0;
}

void liberarListaSalas(ListaSalas *lista) {
    NoSala *no;

    if (lista == NULL) {
        return;
    }

    no = lista->inicio;
    while (no != NULL) {
        NoSala *prox = no->prox;
        free(no);
        no = prox;
    }
    criarListaSalas(lista);
}

int adicionarSala(ListaSalas *lista, const NoSala *modelo) {
    NoSala *no;

    if (lista == NULL || modelo == NULL) {
        return 0;
    }

    no = (NoSala *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }

    *no = *modelo;
    no->prox = NULL;
    no->ant = lista->fim;

    if (lista->fim != NULL) {
        lista->fim->prox = no;
    } else {
        lista->inicio = no;
        lista->atual = no;
        no->desbloqueada = 1;
    }
    lista->fim = no;
    lista->tamanho++;
    return 1;
}

NoSala *salaAtual(ListaSalas *lista) {
    if (lista == NULL) {
        return NULL;
    }
    return lista->atual;
}

static void anotar(PilhaLog *log, const char *texto) {
    if (log != NULL && texto != NULL) {
        empilharLog(log, texto);
    }
}

int avancarSala(ListaSalas *lista, PilhaLog *log) {
    char msg[LOG_MAX];

    if (lista == NULL || lista->atual == NULL || lista->atual->prox == NULL) {
        return 0;
    }
    if (!lista->atual->resolvido || !lista->atual->prox->desbloqueada) {
        return 0;
    }

    lista->atual = lista->atual->prox;
    snprintf(msg, sizeof msg, "Voce entrou em: %s", lista->atual->nome);
    anotar(log, msg);
    return 1;
}

int voltarSala(ListaSalas *lista, PilhaLog *log) {
    char msg[LOG_MAX];

    if (lista == NULL || lista->atual == NULL || lista->atual->ant == NULL) {
        return 0;
    }

    lista->atual = lista->atual->ant;
    snprintf(msg, sizeof msg, "Voce voltou para: %s", lista->atual->nome);
    anotar(log, msg);
    return 1;
}

Habilidade habilidadeDoPokemon(const Pokemon *pokemon) {
    Tipo tipo;
    int i;

    if (pokemon == NULL) {
        return HAB_NENHUMA;
    }

    for (i = 0; i < 2; i++) {
        tipo = (i == 0) ? pokemon->tipo1 : pokemon->tipo2;
        if (tipo == TIPO_ELETRICO) {
            return HAB_ELETRICA;
        }
        if (tipo == TIPO_FOGO) {
            return HAB_FOGO;
        }
        if (tipo == TIPO_AGUA) {
            return HAB_AGUA;
        }
        if (tipo == TIPO_PSIQUICO) {
            return HAB_PSIQUICA;
        }
    }
    return HAB_NENHUMA;
}

int timeTemHabilidade(const ListaTime *time, Habilidade habilidade) {
    int i;
    int n;

    if (time == NULL || habilidade == HAB_NENHUMA) {
        return 0;
    }

    n = tamanhoTime(time);
    for (i = 0; i < n; i++) {
        const Pokemon *pokemon = pokemonNaPosicao(time, i);
        if (habilidadeDoPokemon(pokemon) == habilidade) {
            return 1;
        }
    }
    return 0;
}

const char *nomeHabilidade(Habilidade habilidade) {
    switch (habilidade) {
    case HAB_ELETRICA:
        return "Choque";
    case HAB_FOGO:
        return "Chama";
    case HAB_AGUA:
        return "Jato";
    case HAB_PSIQUICA:
        return "Mente";
    case HAB_NENHUMA:
    default:
        return "Nenhuma";
    }
}

int resolverPuzzleDaSala(ListaSalas *lista, const ListaTime *time, PilhaLog *log) {
    NoSala *sala;
    char msg[LOG_MAX];

    if (lista == NULL || lista->atual == NULL) {
        return 0;
    }

    sala = lista->atual;
    if (sala->resolvido) {
        return 1;
    }
    if (sala->habilidade == HAB_NENHUMA) {
        sala->resolvido = 1;
        return 1;
    }
    if (!timeTemHabilidade(time, sala->habilidade)) {
        anotar(log, "Nenhum Pokemon do time resolve este puzzle.");
        return 0;
    }

    sala->resolvido = 1;
    if (sala->prox != NULL) {
        sala->prox->desbloqueada = 1;
    }

    {
        const char *quem = "Parceiro";
        int i;
        int n = tamanhoTime(time);

        for (i = 0; i < n; i++) {
            const Pokemon *pokemon = pokemonNaPosicao(time, i);
            if (habilidadeDoPokemon(pokemon) == sala->habilidade) {
                quem = pokemon->nome;
                break;
            }
        }
        /* O texto longo da sala fica na tela. O log guarda só o lance. */
        snprintf(msg, sizeof msg, "%s usou %s em %s.",
                 quem, nomeHabilidade(sala->habilidade), sala->nome);
    }
    anotar(log, msg);
    return 1;
}

int tamanhoSalas(const ListaSalas *lista) {
    if (lista == NULL) {
        return 0;
    }
    return lista->tamanho;
}
