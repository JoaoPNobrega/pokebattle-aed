/* Lista dupla do time */

#ifndef LISTA_TIME_H
#define LISTA_TIME_H

#include "pokemon.h"

#define MAX_TIME 6

typedef struct NoTime {
    Pokemon pokemon;
    struct NoTime *prox;
    struct NoTime *ant;
} NoTime;

typedef struct ListaTime {
    NoTime *inicio;
    NoTime *fim;
    NoTime *ativo;
    int tamanho;
} ListaTime;

void criarListaTime(ListaTime *lista);
void liberarListaTime(ListaTime *lista);

/* Insere no fim */
int adicionarPokemon(ListaTime *lista, const Pokemon *pokemon);

/* Remove pelo nome e religa os vizinhos */
int removerPokemon(ListaTime *lista, const char *nome);

/* Remove pela posição */
int removerPokemonPorPosicao(ListaTime *lista, int posicao);

/* Troca dois Pokémon de lugar */
int trocarPokemon(ListaTime *lista, int i, int j);

Pokemon *buscarPokemonNoTime(ListaTime *lista, const char *nome);
const Pokemon *pokemonNaPosicao(const ListaTime *lista, int posicao);

Pokemon *obterAtivo(ListaTime *lista);
int definirAtivoPorPosicao(ListaTime *lista, int posicao);
int posicaoDoAtivo(const ListaTime *lista);

/* Anda um passo, circulando nas pontas */
int avancarAtivo(ListaTime *lista);
int retrocederAtivo(ListaTime *lista);

/* Próximo com HP, para a troca depois do desmaio */
int avancarParaProximoVivo(ListaTime *lista);

int primeiraPosicaoViva(const ListaTime *lista);

int tamanhoTime(const ListaTime *lista);
int timeFoiDerrotado(const ListaTime *lista);
void restaurarTime(ListaTime *lista);

/* Insertion sort por velocidade */
void ordenarTime(ListaTime *lista);

#endif
