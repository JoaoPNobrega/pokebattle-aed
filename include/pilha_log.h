/*
 * pilha_log.h
 * -----------
 * Histórico da batalha como pilha. O lance mais recente fica no topo,
 * que é como o jogador lembra o combate ("o que acabou de acontecer?").
 *
 * Mostrar em ordem cronológica usa o algoritmo clássico de duas pilhas:
 * desempilha tudo numa auxiliar (isso inverte), depois desempilha a
 * auxiliar imprimindo e devolvendo os textos à pilha original. No fim
 * o histórico está igual ao que estava, e a inversão fez parte da
 * jogada de "ver histórico", não é código morto.
 */

#ifndef PILHA_LOG_H
#define PILHA_LOG_H

#include <stddef.h>
#include <stdio.h>

#define LOG_MAX 224

typedef struct NoLog {
    char texto[LOG_MAX];
    struct NoLog *prox;
} NoLog;

typedef struct PilhaLog {
    NoLog *topo;
    int tamanho;
} PilhaLog;

/*
 * criarPilhaLog / liberarPilhaLog
 * criar: O(1).
 * liberar: O(n).
 */
void criarPilhaLog(PilhaLog *pilha);
void liberarPilhaLog(PilhaLog *pilha);

/*
 * empilharLog
 * Copia o texto para um nó novo no topo.
 *
 * Retorno: 1 se empilhou, 0 se faltou memória ou o texto é NULL.
 * Complexidade: O(k), k = tamanho do texto (a cópia).
 */
int empilharLog(PilhaLog *pilha, const char *texto);

/*
 * desempilharLog
 * Remove o topo. Se destino não for NULL e capacidade > 0, recebe o texto.
 *
 * Retorno: 1 se havia um lance, 0 se a pilha estava vazia.
 * Complexidade: O(k) por causa da cópia do texto.
 */
int desempilharLog(PilhaLog *pilha, char *destino, size_t capacidade);

/*
 * topoDoLog
 * Lê o topo sem remover.
 *
 * Retorno: 1 se havia topo, 0 se vazia.
 * Complexidade: O(k).
 */
int topoDoLog(const PilhaLog *pilha, char *destino, size_t capacidade);

/*
 * pilhaVazia / tamanhoPilha
 * Consultas O(1).
 */
int pilhaVazia(const PilhaLog *pilha);
int tamanhoPilha(const PilhaLog *pilha);

/*
 * mostrarHistorico
 * Imprime do lance mais antigo ao mais novo, usando uma pilha auxiliar,
 * e restaura a pilha original.
 *
 * Retorno: nenhum.
 * Complexidade: O(n) em tempo e O(n) de memória extra.
 */
void mostrarHistorico(PilhaLog *pilha, FILE *saida);

#endif
