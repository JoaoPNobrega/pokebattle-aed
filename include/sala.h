/* Lista dupla das salas */

#ifndef SALA_H
#define SALA_H

#include "lista_time.h"
#include "pilha_log.h"
#include "desafio.h"

#define NOME_SALA_MAX 48
#define TEXTO_SALA_MAX 320

typedef enum {
    HAB_NENHUMA = 0,
    HAB_ELETRICA,
    HAB_FOGO,
    HAB_AGUA,
    HAB_PSIQUICA
} Habilidade;

typedef struct NoSala {
    char nome[NOME_SALA_MAX];
    char descricao[TEXTO_SALA_MAX];
    char texto_puzzle[TEXTO_SALA_MAX];
    char texto_resolvido[TEXTO_SALA_MAX];
    Habilidade habilidade; /* HAB_NENHUMA = sem puzzle */
    int resolvido;
    int desbloqueada;
    int inicia_batalha;    /* 1 = tem luta */
    int capturavel;        /* 1 = dá para capturar */
    int e_chefe;           /* 1 = vencer encerra o caso */
    int batalha_concluida; /* 1 = luta já ganha */
    Desafio desafio;
    unsigned int luzes;
    unsigned int luzes_necessarias;
    int duelo_obrigatorio;
    int duelo_concluido;
    struct NoSala *prox;
    struct NoSala *ant;
} NoSala;

typedef struct {
    NoSala *inicio;
    NoSala *fim;
    NoSala *atual; /* sala do detetive */
    int tamanho;
} ListaSalas;

void criarListaSalas(ListaSalas *lista);
void liberarListaSalas(ListaSalas *lista);

/* Insere a sala no fim */
int adicionarSala(ListaSalas *lista, const NoSala *modelo);

NoSala *salaAtual(ListaSalas *lista);

/* Anda se o puzzle e a luta da sala já acabaram */
int avancarSala(ListaSalas *lista, PilhaLog *log);
int voltarSala(ListaSalas *lista, PilhaLog *log);

/* Resolve o puzzle se o time tem a habilidade */
int resolverPuzzleDaSala(ListaSalas *lista, const ListaTime *time, PilhaLog *log);

int tamanhoSalas(const ListaSalas *lista);

/* Tipo do Pokémon vira habilidade da sala */
Habilidade habilidadeDoPokemon(const Pokemon *pokemon);
int timeTemHabilidade(const ListaTime *time, Habilidade habilidade);
const char *nomeHabilidade(Habilidade habilidade);

#endif
