/* Pokédex em árvore AVL, por número */

#ifndef ARVORE_POKEDEX_H
#define ARVORE_POKEDEX_H

#include "pokemon.h"

typedef struct NoAvl {
    Pokemon pokemon;
    struct NoAvl *esq;
    struct NoAvl *dir;
    int altura;
} NoAvl;

typedef struct ArvorePokedex {
    NoAvl *raiz;
    int quantidade;
} ArvorePokedex;

/* Visita um Pokémon no percurso */
typedef void (*VisitaPokemon)(const Pokemon *pokemon, void *contexto);

void criarArvorePokedex(ArvorePokedex *arvore);
void liberarArvorePokedex(ArvorePokedex *arvore);

/* Insere por número e rebalanceia */
int inserirNaArvore(ArvorePokedex *arvore, const Pokemon *pokemon);

/* Busca binária */
const Pokemon *buscarPorNumero(const ArvorePokedex *arvore, int numero);

/* Remove e rebalanceia */
int removerDaArvore(ArvorePokedex *arvore, int numero);

/* Percurso em ordem */
void percorrerEmOrdem(const ArvorePokedex *arvore, VisitaPokemon visita, void *contexto);

int alturaDaArvore(const ArvorePokedex *arvore);
int quantidadeDaArvore(const ArvorePokedex *arvore);

#endif
