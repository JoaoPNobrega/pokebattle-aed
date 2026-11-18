/* Espécies da 1ª geração e a fila da arena */

#ifndef DADOS_POKEMON_H
#define DADOS_POKEMON_H

#include "arvore_pokedex.h"
#include "fila_treinadores.h"
#include "hash_pokedex.h"

/* Cada espécie entra na hash e na AVL */
int registrarEspecies(HashPokedex *pokedex, ArvorePokedex *arvore);

/* Enfileira os desafiantes */
int montarArena(FilaTreinadores *fila, HashPokedex *pokedex);

#endif
