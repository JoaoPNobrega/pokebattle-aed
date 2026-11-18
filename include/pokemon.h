/* Pokémon: atributos, golpes e HP */

#ifndef POKEMON_H
#define POKEMON_H

#include "tipos.h"

#define NOME_MAX 40
#define MAX_GOLPES 4

typedef struct {
    char nome[NOME_MAX];
    Tipo tipo;
    int poder;
    int prioridade; /* 1 = Ataque Rápido, age antes */
} Golpe;

typedef struct {
    int numero;
    char nome[NOME_MAX];
    Tipo tipo1;
    Tipo tipo2; /* TIPO_NENHUM se tiver um tipo só */
    int hp_base;
    int ataque;
    int defesa;
    int velocidade;
    int nivel;
    int hp_max;
    int hp_atual;
    Golpe golpes[MAX_GOLPES];
    int qtd_golpes;
    int experiencia; /* 60 XP sobem um nível */
    int brilhante;   /* variante rara */
} Pokemon;

/* HP e atributo no nível, sem IV e sem EV */
int calcularHpMax(int hp_base, int nivel);
int calcularAtributo(int base, int nivel);

int ataqueEmBatalha(const Pokemon *pokemon);
int defesaEmBatalha(const Pokemon *pokemon);
int velocidadeEmBatalha(const Pokemon *pokemon);

/* Copia a espécie e calcula o HP do nível */
Pokemon pokemonPreparar(const Pokemon *modelo, int nivel);

#endif
