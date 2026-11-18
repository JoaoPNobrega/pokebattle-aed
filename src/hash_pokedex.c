/* Tabela hash. Colisão fica numa lista. */

#include "hash_pokedex.h"

#include "util.h"

#include <ctype.h>
#include <stdlib.h>

void criarHashPokedex(HashPokedex *pokedex) {
    int i;

    if (pokedex == NULL) {
        return;
    }

    for (i = 0; i < TAM_HASH; i++) {
        pokedex->baldes[i] = NULL;
    }
    pokedex->quantidade = 0;
}

void liberarHashPokedex(HashPokedex *pokedex) {
    int i;

    if (pokedex == NULL) {
        return;
    }

    for (i = 0; i < TAM_HASH; i++) {
        NoHash *no = pokedex->baldes[i];
        while (no != NULL) {
            NoHash *prox = no->prox;
            free(no);
            no = prox;
        }
        pokedex->baldes[i] = NULL;
    }
    pokedex->quantidade = 0;
}

int funcaoHashNome(const char *nome) {
    /* Hash djb2 */
    unsigned long hash = 5381;
    const unsigned char *p;

    if (nome == NULL) {
        return 0;
    }

    for (p = (const unsigned char *)nome; *p != '\0'; p++) {
        unsigned char c = (unsigned char)tolower(*p);
        hash = ((hash << 5) + hash) + c;
    }
    return (int)(hash % TAM_HASH);
}

int inserirNaPokedex(HashPokedex *pokedex, const Pokemon *pokemon) {
    int indice;
    NoHash *no;

    if (pokedex == NULL || pokemon == NULL || pokemon->nome[0] == '\0') {
        return 0;
    }
    if (buscarPokemon(pokedex, pokemon->nome) != NULL) {
        return 0;
    }

    no = (NoHash *)calloc(1, sizeof *no);
    if (no == NULL) {
        return 0;
    }

    no->pokemon = *pokemon;
    indice = funcaoHashNome(pokemon->nome);
    /* Colisão: entra no começo da lista do balde */
    no->prox = pokedex->baldes[indice];
    pokedex->baldes[indice] = no;
    pokedex->quantidade++;
    return 1;
}

const Pokemon *buscarPokemon(const HashPokedex *pokedex, const char *nome) {
    int indice;
    const NoHash *no;

    if (pokedex == NULL || nome == NULL || nome[0] == '\0') {
        return NULL;
    }

    indice = funcaoHashNome(nome);
    for (no = pokedex->baldes[indice]; no != NULL; no = no->prox) {
        if (textosIguaisIgnorandoCaixa(no->pokemon.nome, nome)) {
            return &no->pokemon;
        }
    }
    return NULL;
}

int removerDaPokedex(HashPokedex *pokedex, const char *nome) {
    int indice;
    NoHash *no;
    NoHash *anterior = NULL;

    if (pokedex == NULL || nome == NULL) {
        return 0;
    }

    indice = funcaoHashNome(nome);
    no = pokedex->baldes[indice];
    while (no != NULL && !textosIguaisIgnorandoCaixa(no->pokemon.nome, nome)) {
        anterior = no;
        no = no->prox;
    }
    if (no == NULL) {
        return 0;
    }

    if (anterior == NULL) {
        pokedex->baldes[indice] = no->prox;
    } else {
        anterior->prox = no->prox;
    }
    free(no);
    pokedex->quantidade--;
    return 1;
}

int listarPokedex(const HashPokedex *pokedex, Pokemon *destino, int capacidade) {
    int i;
    int n = 0;

    if (pokedex == NULL || destino == NULL || capacidade <= 0) {
        return 0;
    }

    for (i = 0; i < TAM_HASH && n < capacidade; i++) {
        const NoHash *no;
        for (no = pokedex->baldes[i]; no != NULL && n < capacidade; no = no->prox) {
            destino[n] = no->pokemon;
            n++;
        }
    }
    return n;
}

int quantidadePokedex(const HashPokedex *pokedex) {
    if (pokedex == NULL) {
        return 0;
    }
    return pokedex->quantidade;
}
