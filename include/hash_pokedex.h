/*
 * hash_pokedex.h
 * --------------
 * Pokédex com busca por nome.
 *
 * A tabela hash guarda cada espécie num balde. O balde é uma lista
 * simplesmente encadeada: se dois nomes caem no mesmo índice
 * (colisão), eles ficam um atrás do outro. A busca por nome, usada
 * quando o jogador digita "Pikachu" na montagem do time, é O(1) no
 * caso médio — não varremos as 20 espécies toda vez.
 *
 * A ordem dos baldes não é a ordem da Pokédex. Quem precisa da ordem
 * numérica usa a árvore AVL. Quem quer outra chave (nome, velocidade)
 * extrai todo mundo para um vetor e usa o merge sort.
 */

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

/*
 * criarHashPokedex / liberarHashPokedex
 * criar: O(TAM_HASH), zera os baldes.
 * liberar: O(TAM_HASH + n), n = espécies cadastradas.
 */
void criarHashPokedex(HashPokedex *pokedex);
void liberarHashPokedex(HashPokedex *pokedex);

/*
 * funcaoHashNome
 * djb2 em cima do nome, ignorando maiúsculas, depois módulo TAM_HASH.
 *
 * Retorno: índice do balde, entre 0 e TAM_HASH - 1.
 * Complexidade: O(k), k = tamanho do nome.
 */
int funcaoHashNome(const char *nome);

/*
 * inserirNaPokedex
 * Copia a espécie para o começo do balde. Recusa nome repetido.
 *
 * Retorno: 1 se inseriu, 0 se já existia, faltou memória ou o nome é vazio.
 * Complexidade: O(k + c), c = tamanho da lista daquele balde.
 */
int inserirNaPokedex(HashPokedex *pokedex, const Pokemon *pokemon);

/*
 * buscarPokemon
 * Acha a espécie pelo nome, sem diferenciar maiúsculas.
 *
 * Retorno: ponteiro interno (não liberar) ou NULL.
 * Complexidade: O(k + c) no pior caso; O(k) no caso médio.
 */
const Pokemon *buscarPokemon(const HashPokedex *pokedex, const char *nome);

/*
 * removerDaPokedex
 * Tira a espécie do balde. A Pokédex do jogo não remove ninguém;
 * a função existe para a operação da estrutura e é exercitada nos testes.
 *
 * Retorno: 1 se removeu, 0 se não achou.
 * Complexidade: O(k + c).
 */
int removerDaPokedex(HashPokedex *pokedex, const char *nome);

/*
 * listarPokedex
 * Copia as espécies para um vetor, na ordem dos baldes (não ordenada).
 *
 * Retorno: quantas espécies foram copiadas (no máximo `capacidade`).
 * Complexidade: O(TAM_HASH + n).
 */
int listarPokedex(const HashPokedex *pokedex, Pokemon *destino, int capacidade);

/*
 * quantidadePokedex
 * Retorno: número de espécies, ou 0 se a tabela for NULL.
 * Complexidade: O(1).
 */
int quantidadePokedex(const HashPokedex *pokedex);

#endif
