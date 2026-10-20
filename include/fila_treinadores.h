/*
 * fila_treinadores.h
 * ------------------
 * A arena é uma fila (FIFO): o primeiro treinador cadastrado é o
 * primeiro a ser enfrentado. Não dá para pular o Brock e ir direto
 * ao Mewtwo. Isso é regra do jogo, não só um jeito de guardar nomes.
 *
 * Cada treinador carrega o próprio time (lista dupla). Quando a fila
 * libera um treinador, o time dele precisa ser liberado também.
 */

#ifndef FILA_TREINADORES_H
#define FILA_TREINADORES_H

#include "lista_time.h"

#define NOME_TREINADOR_MAX 48
#define FALA_MAX 160

typedef struct {
    char nome[NOME_TREINADOR_MAX];
    char fala[FALA_MAX];
    ListaTime time;
} Treinador;

typedef struct NoFila {
    Treinador treinador;
    struct NoFila *prox;
} NoFila;

typedef struct FilaTreinadores {
    NoFila *frente; /* próximo a sair (desenfileirar) */
    NoFila *tras;   /* último a entrar (enfileirar) */
    int tamanho;
} FilaTreinadores;

/*
 * criarTreinador
 * Prepara um treinador vazio com nome e fala. O time começa vazio.
 *
 * Complexidade: O(1).
 */
void criarTreinador(Treinador *treinador, const char *nome, const char *fala);

/*
 * criarFilaTreinadores / liberarFilaTreinadores
 * criar: O(1).
 * liberar: O(n * t), n treinadores e t Pokémon em cada time.
 *          Também libera os times, senão os nós de lista vazam.
 */
void criarFilaTreinadores(FilaTreinadores *fila);
void liberarFilaTreinadores(FilaTreinadores *fila);

/*
 * enfileirarTreinador
 * Copia o struct para o fim da fila. A cópia inclui os ponteiros do
 * time: depois de enfileirar, a fila é a dona desses nós. O chamador
 * não pode dar liberarListaTime no original.
 *
 * Retorno: 1 se entrou na fila, 0 se faltou memória ou o ponteiro é NULL.
 * Complexidade: O(1).
 */
int enfileirarTreinador(FilaTreinadores *fila, const Treinador *treinador);

/*
 * desenfileirarTreinador
 * Remove da frente. Se destino não for NULL, recebe o treinador
 * (e a posse do time). Se for NULL, o time é liberado aqui.
 *
 * Retorno: 1 se havia alguém, 0 se a fila estava vazia.
 * Complexidade: O(1), fora a liberação do time quando destino é NULL.
 */
int desenfileirarTreinador(FilaTreinadores *fila, Treinador *destino);

/*
 * proximoTreinador
 * Nome de jogo para "sair da fila e enfrentar". É o que a arena chama.
 * Mesma complexidade e o mesmo retorno de desenfileirarTreinador.
 */
int proximoTreinador(FilaTreinadores *fila, Treinador *destino);

/*
 * frenteDaFila
 * Olha o próximo sem remover. O ponteiro fica inválido depois de um
 * desenfileirar, porque o nó é liberado.
 *
 * Retorno: ponteiro interno ou NULL se a fila estiver vazia.
 * Complexidade: O(1).
 */
const Treinador *frenteDaFila(const FilaTreinadores *fila);

/*
 * filaVazia / tamanhoFila
 * Consultas O(1) do contador e da frente.
 */
int filaVazia(const FilaTreinadores *fila);
int tamanhoFila(const FilaTreinadores *fila);

#endif
