/*
 * arvore_pokedex.h
 * ----------------
 * A mesma Pokédex, organizada por número numa árvore AVL.
 *
 * A AVL é uma árvore binária de busca que se rebalanceia depois de
 * cada inserção e remoção (rotações simples e duplas). A busca por
 * número fica O(log n) mesmo se os Pokémon forem cadastrados em
 * ordem (1, 3, 4, 6...), caso em que uma BST comum viraria uma lista
 * comprida.
 *
 * No jogo, digitar "150" na montagem do time cai aqui. O percurso em
 * ordem lista a Pokédex do menor número ao maior sem precisar ordenar
 * de novo: a ordem já é a estrutura.
 */

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

/* Recebe um Pokémon durante o percurso. O contexto é do chamador. */
typedef void (*VisitaPokemon)(const Pokemon *pokemon, void *contexto);

/*
 * criarArvorePokedex / liberarArvorePokedex
 * criar: O(1).
 * liberar: O(n), percurso que libera os nós.
 */
void criarArvorePokedex(ArvorePokedex *arvore);
void liberarArvorePokedex(ArvorePokedex *arvore);

/*
 * inserirNaArvore
 * Insere por número e rebalanceia. Número repetido é recusado
 * (a espécie já está na Pokédex).
 *
 * Retorno: 1 se inseriu, 0 se já existia ou faltou memória.
 * Complexidade: O(log n).
 */
int inserirNaArvore(ArvorePokedex *arvore, const Pokemon *pokemon);

/*
 * buscarPorNumero
 * Busca binária na AVL.
 *
 * Retorno: ponteiro interno ou NULL.
 * Complexidade: O(log n).
 */
const Pokemon *buscarPorNumero(const ArvorePokedex *arvore, int numero);

/*
 * removerDaArvore
 * Remove o número e rebalanceia. Usada nos testes da estrutura;
 * a Pokédex da partida não apaga espécies.
 *
 * Retorno: 1 se removeu, 0 se o número não estava na árvore.
 * Complexidade: O(log n).
 */
int removerDaArvore(ArvorePokedex *arvore, int numero);

/*
 * percorrerEmOrdem
 * Visita esquerda, nó, direita. A ordem de visita é a ordem crescente
 * de número da Pokédex.
 *
 * Complexidade: O(n).
 */
void percorrerEmOrdem(const ArvorePokedex *arvore, VisitaPokemon visita, void *contexto);

/*
 * alturaDaArvore / quantidadeDaArvore
 * altura de árvore vazia é 0. Nó único tem altura 1.
 * Ambas O(1): a altura fica guardada na raiz e a quantidade no struct.
 */
int alturaDaArvore(const ArvorePokedex *arvore);
int quantidadeDaArvore(const ArvorePokedex *arvore);

#endif
