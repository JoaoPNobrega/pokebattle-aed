/* Regras da luta, sem desenhar */

#ifndef BATALHA_H
#define BATALHA_H

#include "fila_treinadores.h"
#include "lista_turno.h"
#include "pilha_log.h"

#define MAX_TURNOS 100
#define NIVEL_DO_JOGADOR 50

typedef void (*Narrador)(void *contexto, const char *mensagem);

typedef struct {
    Narrador narrar; /* pode ser NULL */
    void *contexto;
    PilhaLog *log;   /* pode ser NULL */
} SaidaBatalha;

typedef enum {
    ACAO_GOLPE = 1,
    ACAO_TROCAR,
    ACAO_DESISTIR
} TipoAcaoJogador;

typedef struct {
    TipoAcaoJogador tipo;
    int indice; /* golpe ou posição no time */
} AcaoJogador;

/* O que a tela lê durante o turno */
typedef struct {
    ListaTime *time_jogador;
    ListaTime *time_oponente;
    const char *nome_treinador;
    int numero_turno;
    PilhaLog *log;
} VisaoBatalha;

/* A tela preenche o golpe, a troca ou a desistência */
typedef int (*PedirAcaoJogador)(void *contexto, const VisaoBatalha *visao, AcaoJogador *acao);
typedef int (*PedirTrocaJogador)(void *contexto, const VisaoBatalha *visao, int *posicao);

void registrarMensagem(SaidaBatalha *saida, const char *mensagem);

/* Dano com efetividade e STAB */
int calcularDano(const Pokemon *atacante, const Golpe *golpe, const Pokemon *defensor);

/* Oponente escolhe o golpe de maior dano */
int escolherGolpeAdversario(const Pokemon *atacante, const Pokemon *defensor);

/* Insertion sort e lista circular do turno */
int definirOrdemTurno(ListaTurno *ordem,
                      Pokemon *jogador, int golpe_jogador,
                      Pokemon *oponente, int golpe_oponente);

/* Percorre o ciclo uma vez */
void executarTurno(ListaTurno *ordem, Pokemon *jogador, Pokemon *oponente, SaidaBatalha *saida);

int verificarDesmaio(const Pokemon *pokemon);

/* Uma luta contra um treinador */
int conduzirBatalha(ListaTime *time_jogador,
                    Treinador *treinador,
                    PilhaLog *log,
                    PedirAcaoJogador pedir_acao,
                    PedirTrocaJogador pedir_troca,
                    Narrador narrar,
                    void *contexto);

/* Esvazia a fila de treinadores */
int conduzirArena(ListaTime *time_jogador,
                  FilaTreinadores *fila,
                  PilhaLog *log,
                  PedirAcaoJogador pedir_acao,
                  PedirTrocaJogador pedir_troca,
                  Narrador narrar,
                  void *contexto);

#endif
