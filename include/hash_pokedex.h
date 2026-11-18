/* Pokédex em tabela hash, por nome. Colisão vira lista. */

#ifndef HASH_POKEDEX_H
#define HASH_POKEDEX_H

#include "pokemon.h"

#define TAM_HASH 31

typedef struct NoHash {
    Pokemon pokemon;
    struct NoHash *prox;
} NoHash;

typedef struct HashPokedex {
    NoHash *baldes[TAM_HASH];
    int quantidade;
} HashPokedex;

void criarHashPokedex(HashPokedex *pokedex);
void liberarHashPokedex(HashPokedex *pokedex);

/* Função hash djb2 */
int funcaoHashNome(const char *nome);

/* Insere no balde. Nome repetido é recusado. */
int inserirNaPokedex(HashPokedex *pokedex, const Pokemon *pokemon);

/* Busca pelo nome */
const Pokemon *buscarPokemon(const HashPokedex *pokedex, const char *nome);

/* Tira a espécie do balde */
int removerDaPokedex(HashPokedex *pokedex, const char *nome);

/* Copia as espécies para um vetor */
int listarPokedex(const HashPokedex *pokedex, Pokemon *destino, int capacidade);

int quantidadePokedex(const HashPokedex *pokedex);

#endif
