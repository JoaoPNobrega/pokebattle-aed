/*
 * tipos.h
 * -------
 * Tipos elementares da 1ª geração e a tabela de efetividade.
 *
 * A tabela é uma matriz quadrada: a linha é o tipo do golpe e a
 * coluna é o tipo do defensor. Cada casa vale
 *   0   = imune
 *   50  = pouco efetivo (0,5x)
 *   100 = neutro (1x)
 *   200 = super efetivo (2x)
 *
 * Guardar isso como inteiros (porcentagem) evita erro de ponto
 * flutuante na hora de calcular o dano. Pokémon com dois tipos
 * multiplica as duas casas: Fogo contra Grama/Veneno, por exemplo,
 * é 2x * 1x.
 *
 * A matriz entra na lógica central: sem ela, calcularDano não sabe
 * se o golpe acerta forte, fraco ou nem acerta.
 */

#ifndef TIPOS_H
#define TIPOS_H

typedef enum {
    TIPO_NENHUM = -1,
    TIPO_NORMAL = 0,
    TIPO_FOGO,
    TIPO_AGUA,
    TIPO_ELETRICO,
    TIPO_GRAMA,
    TIPO_GELO,
    TIPO_LUTADOR,
    TIPO_VENENO,
    TIPO_TERRA,
    TIPO_VOADOR,
    TIPO_PSIQUICO,
    TIPO_INSETO,
    TIPO_PEDRA,
    TIPO_FANTASMA,
    TIPO_DRAGAO,
    TIPO_CONTAGEM
} Tipo;

/*
 * inicializarTipos
 * Preenche a matriz com o neutro (100) e aplica as exceções clássicas.
 * Pode ser chamada mais de uma vez: as chamadas seguintes não fazem nada.
 *
 * Retorno: nenhum.
 * Complexidade: O(t^2), t = quantidade de tipos (constante, 15).
 */
void inicializarTipos(void);

/*
 * nomeTipo
 * Nome em português para mostrar na tela.
 *
 * Retorno: ponteiro para texto fixo (não liberar).
 * Complexidade: O(1).
 */
const char *nomeTipo(Tipo tipo);

/*
 * efetividadePar
 * Lê uma casa da matriz (golpe contra um único tipo).
 *
 * Retorno: 0, 50, 100 ou 200. Tipo inválido ou TIPO_NENHUM devolve 100.
 * Complexidade: O(1).
 */
int efetividadePar(Tipo ataque, Tipo defesa);

/*
 * multiplicadorEfetividade
 * Combina os dois tipos do defensor.
 * def2 pode ser TIPO_NENHUM quando o Pokémon tem um tipo só.
 *
 * Retorno: porcentagem inteira (0, 25, 50, 100, 200, 400, ...).
 * Complexidade: O(1).
 */
int multiplicadorEfetividade(Tipo ataque, Tipo def1, Tipo def2);

#endif
