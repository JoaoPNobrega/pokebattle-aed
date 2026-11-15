/*
 * pokemon.h
 * ---------
 * O que é um Pokémon neste jogo: identidade, atributos, golpes e HP.
 *
 * Os números de hp_base, ataque, defesa e velocidade são os atributos
 * de espécie (inspirados na 1ª geração). Na batalha eles são escalados
 * pelo nível. Há um único "ataque" e uma única "defesa": a etapa 1 não
 * separa golpe físico de especial. Para atacantes especiais famosos
 * (Alakazam, Mewtwo, Gengar) o campo ataque guarda a força ofensiva
 * que eles realmente usam, senão o Psíquico sairia fraco.
 *
 * Esta etapa não sorteia dano. A mesma situação sempre produz o mesmo
 * número, o que deixa a defesa oral e os testes reproduzíveis.
 * Treinos opcionais guardam XP no próprio Pokémon (30 por vitória,
 * 60 por nível). O HP máximo é recalculado ao subir, sem reviver desmaiados.
 */

#ifndef POKEMON_H
#define POKEMON_H

#include "tipos.h"

#define NOME_MAX 40
#define MAX_GOLPES 4

typedef struct {
    char nome[NOME_MAX];
    Tipo tipo;
    int poder;
    int prioridade; /* 0 = normal; 1 = Ataque Rápido, age antes da velocidade */
} Golpe;

typedef struct {
    int numero;
    char nome[NOME_MAX];
    Tipo tipo1;
    Tipo tipo2; /* TIPO_NENHUM quando a espécie tem um tipo só */
    int hp_base;
    int ataque;
    int defesa;
    int velocidade;
    int nivel;
    int hp_max;
    int hp_atual;
    Golpe golpes[MAX_GOLPES];
    int qtd_golpes;
    int experiencia; /* 60 XP de treino sobem um nível, até o nível 60. */
} Pokemon;

/*
 * calcularHpMax / calcularAtributo
 * Convertem o atributo de espécie para o valor usado no nível informado,
 * no mesmo formato dos jogos clássicos (sem IV e sem EV):
 *   HP       = (2 * base * nível) / 100 + nível + 10
 *   atributo = (base * nível) / 50 + 5
 *
 * Retorno: valor >= 1.
 * Complexidade: O(1).
 */
int calcularHpMax(int hp_base, int nivel);
int calcularAtributo(int base, int nivel);

/*
 * ataqueEmBatalha, defesaEmBatalha, velocidadeEmBatalha
 * Atalhos que aplicam calcularAtributo no Pokémon já preparado.
 *
 * Retorno: 0 se o ponteiro for NULL; senão o atributo escalado.
 * Complexidade: O(1).
 */
int ataqueEmBatalha(const Pokemon *pokemon);
int defesaEmBatalha(const Pokemon *pokemon);
int velocidadeEmBatalha(const Pokemon *pokemon);

/*
 * pokemonPreparar
 * Copia o modelo da Pokédex e calcula HP para o nível pedido.
 * O modelo original (dentro da hash / árvore) não é alterado.
 *
 * Retorno: Pokémon pronto para entrar no time. Se modelo for NULL,
 *          devolve uma struct zerada.
 * Complexidade: O(1) (a struct tem tamanho fixo).
 */
Pokemon pokemonPreparar(const Pokemon *modelo, int nivel);

#endif
