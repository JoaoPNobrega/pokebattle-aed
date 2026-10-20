/*
 * jogo.h
 * ------
 * O estado de uma partida: Pokédex (hash + árvore), time do jogador,
 * fila da arena e pilha do histórico.
 *
 * Nada aqui imprime. A interface pede emprestado este struct e chama
 * as operações. Separar assim é o que permite trocar o terminal pelo
 * raylib sem reescrever a luta.
 */

#ifndef JOGO_H
#define JOGO_H

#include "arvore_pokedex.h"
#include "dados_pokemon.h"
#include "hash_pokedex.h"
#include "pilha_log.h"

typedef struct {
    HashPokedex pokedex;
    ArvorePokedex arvore;
    ListaTime time;
    FilaTreinadores fila;
    PilhaLog log;
} Jogo;

/*
 * iniciarJogo
 * Cria as estruturas vazias e carrega a Pokédex. Ainda não monta a
 * arena: consultar a Pokédex não exige começar uma jornada.
 *
 * Retorno: 1 se a Pokédex foi carregada, 0 se faltou memória.
 * Complexidade: O(n log n), n = espécies.
 */
int iniciarJogo(Jogo *jogo);

/*
 * encerrarJogo
 * Libera tudo o que iniciarJogo e as jornadas alocaram.
 *
 * Complexidade: O(n) somando Pokédex, times e log.
 */
void encerrarJogo(Jogo *jogo);

/*
 * reiniciarDesafio
 * Limpa time, fila e histórico, e monta uma arena nova. Usado a cada
 * "Nova jornada", para o jogador poder tentar de novo sem vazar a
 * tentativa anterior.
 *
 * Retorno: 1 se a arena foi montada, 0 em falha de memória.
 */
int reiniciarDesafio(Jogo *jogo);

#endif
