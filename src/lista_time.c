/* Lista dupla do time */

#include "lista_time.h"

#include "ordenacao.h"
#include "util.h"

#include <stdlib.h>

static NoTime *noNaPosicao(const ListaTime *lista, int posicao) {
    NoTime *no;
    int i;

    if (lista == NULL || posicao < 0 || posicao >= lista->tamanho) {
        return NULL;
    }

    no = lista->inicio;
    for (i = 0; i < posicao && no != NULL; i++) {
        no = no->prox;
    }
    return no;
}

void criarListaTime(ListaTime *lista) {
    if (lista == NULL) {
        return;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->ativo = NULL;
    lista->tamanho = 0;
}

void liberarListaTime(ListaTime *lista) {
    NoTime *no;

    if (lista == NULL) {
        return;
    }

    no = lista->inicio;
    while (no != NULL) {
        NoTime *prox = no->prox;
        free(no);
        no = prox;
    }
    criarListaTime(lista);
}

int adicionarPokemon(ListaTime *lista, const Pokemon *pokemon) {
    NoTime *no;

    if (lista == NULL || pokemon == NULL) {
        return 0;
    }
    if (lista->tamanho >= MAX_TIME) {
        return 0;
    }

    no = (NoTime *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }

    no->pokemon = *pokemon;
    no->prox = NULL;
    no->ant = lista->fim;

    if (lista->fim != NULL) {
        lista->fim->prox = no;
    } else {
        lista->inicio = no;
    }
    lista->fim = no;

    if (lista->ativo == NULL) {
        lista->ativo = no;
    }
    lista->tamanho++;
    return 1;
}

int removerPokemon(ListaTime *lista, const char *nome) {
    NoTime *no;

    if (lista == NULL || nome == NULL) {
        return 0;
    }

    no = lista->inicio;
    while (no != NULL && !textosIguaisIgnorandoCaixa(no->pokemon.nome, nome)) {
        no = no->prox;
    }
    if (no == NULL) {
        return 0;
    }

    /* Remove: religa ant e prox */
    if (no->ant != NULL) {
        no->ant->prox = no->prox;
    } else {
        lista->inicio = no->prox;
    }

    if (no->prox != NULL) {
        no->prox->ant = no->ant;
    } else {
        lista->fim = no->ant;
    }

    if (lista->ativo == no) {
        lista->ativo = (no->prox != NULL) ? no->prox : no->ant;
    }

    free(no);
    lista->tamanho--;
    return 1;
}

int removerPokemonPorPosicao(ListaTime *lista, int posicao) {
    NoTime *no;

    if (lista == NULL || posicao < 0 || posicao >= lista->tamanho) {
        return 0;
    }
    no = noNaPosicao(lista, posicao);
    if (no == NULL) {
        return 0;
    }

    if (no->ant != NULL) {
        no->ant->prox = no->prox;
    } else {
        lista->inicio = no->prox;
    }

    if (no->prox != NULL) {
        no->prox->ant = no->ant;
    } else {
        lista->fim = no->ant;
    }

    if (lista->ativo == no) {
        lista->ativo = (no->prox != NULL) ? no->prox : no->ant;
    }

    free(no);
    lista->tamanho--;
    return 1;
}

int trocarPokemon(ListaTime *lista, int i, int j) {
    NoTime *a;
    NoTime *b;
    Pokemon tmp;

    if (lista == NULL) {
        return 0;
    }
    if (i == j) {
        return 1;
    }

    a = noNaPosicao(lista, i);
    b = noNaPosicao(lista, j);
    if (a == NULL || b == NULL) {
        return 0;
    }

    /* Troca o Pokémon, não o nó */
    tmp = a->pokemon;
    a->pokemon = b->pokemon;
    b->pokemon = tmp;

    if (lista->ativo == a) {
        lista->ativo = b;
    } else if (lista->ativo == b) {
        lista->ativo = a;
    }
    return 1;
}

Pokemon *buscarPokemonNoTime(ListaTime *lista, const char *nome) {
    NoTime *no;

    if (lista == NULL || nome == NULL) {
        return NULL;
    }

    for (no = lista->inicio; no != NULL; no = no->prox) {
        if (textosIguaisIgnorandoCaixa(no->pokemon.nome, nome)) {
            return &no->pokemon;
        }
    }
    return NULL;
}

const Pokemon *pokemonNaPosicao(const ListaTime *lista, int posicao) {
    NoTime *no = noNaPosicao(lista, posicao);
    if (no == NULL) {
        return NULL;
    }
    return &no->pokemon;
}

Pokemon *obterAtivo(ListaTime *lista) {
    if (lista == NULL || lista->ativo == NULL) {
        return NULL;
    }
    return &lista->ativo->pokemon;
}

int definirAtivoPorPosicao(ListaTime *lista, int posicao) {
    NoTime *no = noNaPosicao(lista, posicao);
    if (no == NULL) {
        return 0;
    }
    lista->ativo = no;
    return 1;
}

int posicaoDoAtivo(const ListaTime *lista) {
    const NoTime *no;
    int i;

    if (lista == NULL || lista->ativo == NULL) {
        return -1;
    }

    i = 0;
    for (no = lista->inicio; no != NULL; no = no->prox, i++) {
        if (no == lista->ativo) {
            return i;
        }
    }
    return -1;
}

int avancarAtivo(ListaTime *lista) {
    if (lista == NULL || lista->ativo == NULL) {
        return 0;
    }
    if (lista->ativo->prox != NULL) {
        lista->ativo = lista->ativo->prox;
    } else {
        lista->ativo = lista->inicio;
    }
    return 1;
}

int retrocederAtivo(ListaTime *lista) {
    if (lista == NULL || lista->ativo == NULL) {
        return 0;
    }
    if (lista->ativo->ant != NULL) {
        lista->ativo = lista->ativo->ant;
    } else {
        lista->ativo = lista->fim;
    }
    return 1;
}

int avancarParaProximoVivo(ListaTime *lista) {
    NoTime *partida;

    if (lista == NULL || lista->ativo == NULL || lista->tamanho == 0) {
        return 0;
    }

    partida = lista->ativo;
    do {
        avancarAtivo(lista);
        if (lista->ativo->pokemon.hp_atual > 0) {
            return 1;
        }
    } while (lista->ativo != partida);

    return 0;
}

int primeiraPosicaoViva(const ListaTime *lista) {
    int i;
    int n;

    if (lista == NULL) {
        return -1;
    }

    n = lista->tamanho;
    for (i = 0; i < n; i++) {
        const Pokemon *pokemon = pokemonNaPosicao(lista, i);
        if (pokemon != NULL && pokemon->hp_atual > 0) {
            return i;
        }
    }
    return -1;
}

int tamanhoTime(const ListaTime *lista) {
    if (lista == NULL) {
        return 0;
    }
    return lista->tamanho;
}

int timeFoiDerrotado(const ListaTime *lista) {
    return primeiraPosicaoViva(lista) < 0;
}

void restaurarTime(ListaTime *lista) {
    NoTime *no;

    if (lista == NULL) {
        return;
    }

    for (no = lista->inicio; no != NULL; no = no->prox) {
        no->pokemon.hp_atual = no->pokemon.hp_max;
    }
}

void ordenarTime(ListaTime *lista) {
    Pokemon vetor[MAX_TIME];
    int n = 0;
    NoTime *no;

    if (lista == NULL || lista->tamanho <= 1) {
        return;
    }

    for (no = lista->inicio; no != NULL && n < MAX_TIME; no = no->prox) {
        vetor[n] = no->pokemon;
        n++;
    }

    ordenarPokemonPorInsercao(vetor, n, ORDENAR_POR_VELOCIDADE);

    n = 0;
    for (no = lista->inicio; no != NULL && n < MAX_TIME; no = no->prox) {
        no->pokemon = vetor[n];
        n++;
    }

    /* O mais rápido vira o líder */
    lista->ativo = lista->inicio;
}
