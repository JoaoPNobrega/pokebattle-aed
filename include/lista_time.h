/*
 * lista_time.h
 * ------------
 * Time do jogador (e também o time de cada treinador) como lista
 * duplamente encadeada.
 *
 * Por que esta estrutura: o time é uma fila de banco com ordem.
 * Inserir no fim é O(1) porque guardamos o ponteiro `fim`. Remover
 * um Pokémon do meio exige os dois elos (ant e prox), senão
 * perderíamos a lista. O ponteiro `ativo` é o Pokémon que está em
 * campo: trocar, desmaiar e escolher o próximo são movimentos desse
 * cursor, não um vetor paralelo.
 *
 * A posição 0 (o nó `inicio`) é o líder: é quem entra no começo de
 * cada batalha. TROCAR e ORDENAR mudam essa ordem de propósito.
 */

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

/*
 * criarListaTime
 * Zera a lista. Não aloca o struct ListaTime em si (ele pode viver
 * dentro de outro struct, como o Treinador).
 *
 * Complexidade: O(1).
 */
void criarListaTime(ListaTime *lista);

/*
 * liberarListaTime
 * Libera todos os nós e deixa a lista vazia. Não dá free na ListaTime.
 *
 * Complexidade: O(n), n = tamanho do time.
 */
void liberarListaTime(ListaTime *lista);

/*
 * adicionarPokemon
 * Insere uma cópia no fim. Falha se a lista estiver cheia (6),
 * se faltar memória ou se o ponteiro for NULL.
 *
 * Retorno: 1 em sucesso, 0 em falha.
 * Complexidade: O(1).
 */
int adicionarPokemon(ListaTime *lista, const Pokemon *pokemon);

/*
 * removerPokemon
 * Remove o primeiro Pokémon com esse nome (sem diferenciar maiúsculas).
 * Religa ant e prox. Se o removido era o ativo, o cursor vai para o
 * vizinho.
 *
 * Retorno: 1 se removeu, 0 se não encontrou.
 * Complexidade: O(n).
 */
int removerPokemon(ListaTime *lista, const char *nome);

/*
 * removerPokemonPorPosicao
 * Remove o nó na posição especificada (0-indexada).
 * Retorno: 1 se removeu, 0 se índice inválido.
 * Complexidade: O(n).
 */
int removerPokemonPorPosicao(ListaTime *lista, int posicao);

/*
 * trocarPokemon
 * Troca os Pokémon das posições i e j (índices começando em 0).
 * Os nós continuam no lugar; o que muda é o conteúdo. O cursor ativo
 * acompanha o Pokémon, não a vaga.
 *
 * Retorno: 1 se trocou (ou se i == j), 0 se a posição não existe.
 * Complexidade: O(n) para achar os nós, O(1) para a troca.
 */
int trocarPokemon(ListaTime *lista, int i, int j);

/*
 * buscarPokemonNoTime
 * Procura por nome.
 *
 * Retorno: ponteiro para o Pokémon dentro do nó, ou NULL.
 * Complexidade: O(n).
 */
Pokemon *buscarPokemonNoTime(ListaTime *lista, const char *nome);

/*
 * pokemonNaPosicao
 * Acesso por índice, começando em 0.
 *
 * Retorno: ponteiro interno ou NULL.
 * Complexidade: O(n).
 */
const Pokemon *pokemonNaPosicao(const ListaTime *lista, int posicao);

/*
 * obterAtivo / definirAtivoPorPosicao / posicaoDoAtivo
 * Leem e movem o cursor do Pokémon em campo.
 *
 * definirAtivoPorPosicao retorna 1 se a posição existe.
 * posicaoDoAtivo retorna -1 se não houver ativo.
 * Complexidade: O(1) para obter; O(n) para definir e para achar a posição.
 */
Pokemon *obterAtivo(ListaTime *lista);
int definirAtivoPorPosicao(ListaTime *lista, int posicao);
int posicaoDoAtivo(const ListaTime *lista);

/*
 * avancarAtivo / retrocederAtivo
 * Andam um passo na lista, circulando nas pontas (do fim volta ao
 * início e vice-versa). Servem para navegar o time na montagem.
 *
 * Retorno: 1 se havia um ativo e ele se moveu (mesmo que para si, em
 *          um time de 1), 0 se a lista não tem cursor.
 * Complexidade: O(1).
 */
int avancarAtivo(ListaTime *lista);
int retrocederAtivo(ListaTime *lista);

/*
 * avancarParaProximoVivo
 * A partir do ativo, caminha com avancarAtivo até achar alguém com HP.
 * É o que a batalha usa quando o oponente desmaia: o próximo da lista
 * entra, dando a volta se precisar.
 *
 * Retorno: 1 se encontrou alguém vivo, 0 se o time inteiro desmaiou.
 * Complexidade: O(n).
 */
int avancarParaProximoVivo(ListaTime *lista);

/*
 * primeiraPosicaoViva
 * Índice do primeiro Pokémon com HP > 0, varrendo do início.
 *
 * Retorno: índice, ou -1 se ninguém puder lutar.
 * Complexidade: O(n).
 */
int primeiraPosicaoViva(const ListaTime *lista);

/*
 * tamanhoTime / timeFoiDerrotado / restaurarTime
 * tamanhoTime: O(1), lê o contador.
 * timeFoiDerrotado: O(n), verdadeiro se não houver ninguém com HP.
 * restaurarTime: O(n), enche o HP de todos (centro Pokémon entre lutas).
 */
int tamanhoTime(const ListaTime *lista);
int timeFoiDerrotado(const ListaTime *lista);
void restaurarTime(ListaTime *lista);

/*
 * ordenarTime
 * Reordena o time da maior velocidade de espécie para a menor,
 * com insertion sort. Quem fica no início entra primeiro na próxima
 * batalha. Não é ranking de placar: muda a ordem de entrada.
 *
 * Retorno: nenhum.
 * Complexidade: O(n^2), n <= 6.
 */
void ordenarTime(ListaTime *lista);

#endif
