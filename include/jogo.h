/* Partida: Pokédex, time, fila e histórico */

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

int iniciarJogo(Jogo *jogo);
void encerrarJogo(Jogo *jogo);

/* Limpa a tentativa e monta a arena de novo */
int reiniciarDesafio(Jogo *jogo);

#endif
