/*
 * dados_pokemon.h
 * ---------------
 * As espécies da 1ª geração usadas no jogo e a fila de desafiantes
 * da arena, montada com cara de "Pokémon: O Filme — Mewtwo Contra-Ataca":
 * Brock, Misty, Equipe Rocket, Ash e, no fim, o Mewtwo.
 *
 * Os dados nascem no código de propósito. A etapa 1 não lê arquivo
 * externo, então o projeto compila com um gcc só.
 */

#ifndef DADOS_POKEMON_H
#define DADOS_POKEMON_H

#include "arvore_pokedex.h"
#include "fila_treinadores.h"
#include "hash_pokedex.h"

/*
 * registrarEspecies
 * Coloca cada espécie na hash (busca por nome) e na AVL (busca por
 * número). As duas estruturas recebem uma cópia; nenhuma é dona da outra.
 *
 * Retorno: 1 se todas entraram, 0 se alguma inserção falhou.
 * Complexidade: O(n log n), n = quantidade de espécies, por causa da AVL.
 */
int registrarEspecies(HashPokedex *pokedex, ArvorePokedex *arvore);

/*
 * montarArena
 * Enfileira os desafiantes, cada um com o próprio time já no nível
 * da luta. A fila fica dona desses times.
 *
 * Retorno: 1 se a arena foi montada, 0 se faltou espécie ou memória.
 *          Em caso de falha a fila pode ter ficado pela metade: quem
 *          chamou deve liberá-la.
 * Complexidade: O(n) treinadores, cada inserção na fila é O(1).
 */
int montarArena(FilaTreinadores *fila, HashPokedex *pokedex);

#endif
