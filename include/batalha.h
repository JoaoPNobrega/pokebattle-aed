/*
 * batalha.h
 * ---------
 * A regra da luta, sem printf. Quem mostra o texto na tela é a
 * interface (hoje o terminal; amanhã pode ser o raylib), através do
 * ponteiro de função Narrador.
 *
 * Um turno:
 *   1. Os dois lados escolhem um golpe (o oponente escolhe sozinho).
 *   2. definirOrdemTurno monta os atores, ordena com insertion sort
 *      e encaixa o resultado numa lista circular.
 *   3. executarTurno percorre o ciclo. Cada um age uma vez. Se o
 *      primeiro derrubar o segundo, o segundo não joga.
 *   4. verificarDesmaio decide se alguém precisa entrar no lugar.
 */

#ifndef BATALHA_H
#define BATALHA_H

#include "fila_treinadores.h"
#include "lista_turno.h"
#include "pilha_log.h"

#define MAX_TURNOS 100
#define NIVEL_DO_JOGADOR 50

typedef void (*Narrador)(void *contexto, const char *mensagem);

typedef struct {
    Narrador narrar; /* pode ser NULL em um teste que só quer o log */
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
    int indice; /* golpe ou posição no time, começando em 0 */
} AcaoJogador;

/*
 * Foto do turno entregue à interface. Os ponteiros são os times
 * vivos da batalha: a interface só lê, a lógica é quem altera HP.
 */
typedef struct {
    ListaTime *time_jogador;
    ListaTime *time_oponente;
    const char *nome_treinador;
    int numero_turno;
    PilhaLog *log;
} VisaoBatalha;

/*
 * PedirAcaoJogador
 * A interface preenche a ação do jogador (golpe, troca ou desistência).
 * Retorno: 1 se preencheu, 0 se a entrada acabou (EOF).
 *
 * PedirTrocaJogador
 * Usada quando o Pokémon ativo desmaiou e outro precisa entrar.
 * Escreve em *posicao um índice válido de alguém vivo.
 * Retorno: 1 se escolheu, 0 em EOF.
 */
typedef int (*PedirAcaoJogador)(void *contexto, const VisaoBatalha *visao, AcaoJogador *acao);
typedef int (*PedirTrocaJogador)(void *contexto, const VisaoBatalha *visao, int *posicao);

/*
 * registrarMensagem
 * Empilha o texto no log (se houver) e entrega ao narrador (se houver).
 *
 * Complexidade: O(k), k = tamanho da mensagem.
 */
void registrarMensagem(SaidaBatalha *saida, const char *mensagem);

/*
 * calcularDano
 * Fórmula simplificada dos jogos clássicos, toda em inteiros:
 *   base = ((2 * nível / 5 + 2) * poder * ataque / defesa) / 50 + 2
 *   base = base * efetividade / 100
 *   se o tipo do golpe é o tipo do atacante, base = base * 3 / 2 (STAB)
 *   base = base / 2
 * O /2 final não existe no Game Boy. Ele só encurta a diferença entre
 * o dano e o HP para a luta no terminal durar mais de um golpe. A
 * proporção entre tipos, STAB e quem age primeiro não muda.
 *
 * Retorno: dano >= 0. Imunidade devolve 0. Ponteiro NULL devolve 0.
 * Complexidade: O(1).
 */
int calcularDano(const Pokemon *atacante, const Golpe *golpe, const Pokemon *defensor);

/*
 * escolherGolpeAdversario
 * A "IA" da etapa 1: prova cada golpe com calcularDano e fica com o
 * que tira mais HP. Empate fica com o primeiro golpe.
 *
 * Retorno: índice do golpe, ou -1 se não houver golpe.
 * Complexidade: O(g), g = quantidade de golpes (no máximo 4).
 */
int escolherGolpeAdversario(const Pokemon *atacante, const Pokemon *defensor);

/*
 * definirOrdemTurno
 * Monta os atores que ainda podem agir, ordena com insertion sort e
 * reescreve a lista circular. golpe negativo ou Pokémon desmaiado
 * fica de fora (é assim que a troca, em que só o oponente age, passa
 * pela mesma estrutura).
 *
 * A lista é esvaziada antes de ser reescrita. O chamador a libera
 * no fim do turno.
 *
 * Retorno: quantos atores entraram no ciclo.
 * Complexidade: O(n^2) pelo insertion sort, com n <= 2 nesta etapa.
 */
int definirOrdemTurno(ListaTurno *ordem,
                      Pokemon *jogador, int golpe_jogador,
                      Pokemon *oponente, int golpe_oponente);

/*
 * executarTurno
 * Percorre a lista circular uma vez por ator. Quem está desmaiado
 * não age. O golpe atualiza o HP dentro do time (o ponteiro da lista
 * circular aponta para o mesmo Pokémon).
 *
 * Complexidade: O(n), n = atores no ciclo.
 */
void executarTurno(ListaTurno *ordem, Pokemon *jogador, Pokemon *oponente, SaidaBatalha *saida);

/*
 * verificarDesmaio
 * Retorno: 1 se o ponteiro é NULL ou o HP chegou a 0.
 * Complexidade: O(1).
 */
int verificarDesmaio(const Pokemon *pokemon);

/*
 * conduzirBatalha
 * Uma luta inteira contra um treinador, até o time de alguém cair,
 * alguém desistir, a entrada acabar ou bater em MAX_TURNOS.
 *
 * Retorno:
 *   1  jogador venceu o treinador
 *   0  jogador perdeu
 *  -1  jogador desistiu
 *  -2  entrada acabou (EOF)
 * Complexidade: O(turnos * (n log... na prática turnos * golpes)).
 */
int conduzirBatalha(ListaTime *time_jogador,
                    Treinador *treinador,
                    PilhaLog *log,
                    PedirAcaoJogador pedir_acao,
                    PedirTrocaJogador pedir_troca,
                    Narrador narrar,
                    void *contexto);

/*
 * conduzirArena
 * Esvazia a fila. Cada treinador é uma batalha. Entre um e outro o
 * time do jogador passa no centro Pokémon (HP cheio). A ordem da fila
 * não é reorganizada: FIFO.
 *
 * Retorno: o mesmo código de conduzirBatalha. 1 só se a fila inteira
 *          (incluindo o Mewtwo) for vencida.
 */
int conduzirArena(ListaTime *time_jogador,
                  FilaTreinadores *fila,
                  PilhaLog *log,
                  PedirAcaoJogador pedir_acao,
                  PedirTrocaJogador pedir_troca,
                  Narrador narrar,
                  void *contexto);

#endif
