/*
 * sala.h
 * ------
 * As salas do caso, numa lista duplamente encadeada.
 *
 * O laboratório é um corredor com portas: dá para voltar à sala
 * anterior e seguir para a próxima só quando ela foi desbloqueada.
 * A lista dupla guarda essa ordem. Não é só um depósito de nomes:
 * o cursor `atual` é a sala em que o detetive está, e avançar falha
 * se o puzzle da sala corrente ainda não foi resolvido.
 *
 * O puzzle pede uma habilidade de tipo (choque, chama, jato, mente).
 * Quem resolve é um Pokémon do time, não um item solto.
 */

#ifndef SALA_H
#define SALA_H

#include "lista_time.h"
#include "pilha_log.h"

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
    Habilidade habilidade; /* HAB_NENHUMA = sala sem puzzle */
    int resolvido;
    int desbloqueada;
    int inicia_batalha; /* 1 = tem um confronto ao entrar em ação */
    struct NoSala *prox;
    struct NoSala *ant;
} NoSala;

typedef struct {
    NoSala *inicio;
    NoSala *fim;
    NoSala *atual; /* sala em que o detetive está */
    int tamanho;
} ListaSalas;

/*
 * criarListaSalas / liberarListaSalas
 * criar: O(1), zera os ponteiros.
 * liberar: O(n), dá free em cada sala. Não dá free na ListaSalas.
 */
void criarListaSalas(ListaSalas *lista);
void liberarListaSalas(ListaSalas *lista);

/*
 * adicionarSala
 * Encaixa uma sala nova no fim. A primeira sala entra desbloqueada
 * e vira a sala atual. As seguintes nascem bloqueadas, a menos que
 * o chamador já marque desbloqueada no próprio nó depois.
 *
 * O conteúdo é copiado para o nó. habilidade, textos e a flag de
 * batalha vêm prontos nos campos do ponteiro `modelo` (pode ser um
 * nó temporário na stack; só os campos de texto e as flags são lidos).
 *
 * Retorno: 1 se entrou na lista, 0 se faltou memória ou o ponteiro é NULL.
 * Complexidade: O(1).
 */
int adicionarSala(ListaSalas *lista, const NoSala *modelo);

/*
 * salaAtual
 * Retorno: a sala do cursor, ou NULL.
 * Complexidade: O(1).
 */
NoSala *salaAtual(ListaSalas *lista);

/*
 * avancarSala / voltarSala
 * avancar só anda se a sala atual está resolvida e a próxima já foi
 * desbloqueada. voltar anda para a anterior se ela existe.
 * As duas empilham o nome da sala nova no histórico, se o log não for NULL.
 *
 * Retorno: 1 se o cursor mudou, 0 se o passo não era permitido.
 * Complexidade: O(1), mais O(k) para copiar o texto do log.
 */
int avancarSala(ListaSalas *lista, PilhaLog *log);
int voltarSala(ListaSalas *lista, PilhaLog *log);

/*
 * resolverPuzzleDaSala
 * A sala atual pede uma habilidade. Se algum Pokémon do time tiver
 * essa habilidade (pelo tipo), a sala fica resolvida e a próxima,
 * se existir, é desbloqueada.
 *
 * Retorno: 1 se resolveu agora ou se já estava resolvida, 0 se o
 *          time não tem a habilidade ou não há sala.
 * Complexidade: O(t), t = tamanho do time (a busca da habilidade).
 */
int resolverPuzzleDaSala(ListaSalas *lista, const ListaTime *time, PilhaLog *log);

/*
 * tamanhoSalas
 * Retorno: quantidade de salas, ou 0 se a lista for NULL.
 * Complexidade: O(1).
 */
int tamanhoSalas(const ListaSalas *lista);

/*
 * habilidadeDoPokemon / timeTemHabilidade / nomeHabilidade
 * A ponte entre o tipo do Pokémon e o puzzle.
 * Elétrico acende e liga máquina. Fogo derrete e acende.
 * Água apaga fogo. Psíquico move objeto.
 * timeTemHabilidade percorre o time: O(t). As outras são O(1).
 */
Habilidade habilidadeDoPokemon(const Pokemon *pokemon);
int timeTemHabilidade(const ListaTime *time, Habilidade habilidade);
const char *nomeHabilidade(Habilidade habilidade);

#endif
