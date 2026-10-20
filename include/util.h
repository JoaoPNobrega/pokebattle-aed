/*
 * util.h
 * ------
 * Funções pequenas de texto, sem regra de jogo.
 *
 * Os nomes dos Pokémon são ASCII de propósito: assim a comparação
 * sem diferenciar maiúsculas funciona em qualquer sistema, sem
 * depender de acentuação em UTF-8 (tolower não entende acento).
 */

#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/*
 * copiarTexto
 * Copia origem para destino, sempre fechando com '\0'.
 * Se origem for NULL, destino fica vazio.
 *
 * Parâmetros: destino, capacidade em bytes, origem.
 * Retorno: nenhum.
 * Complexidade: O(min(tamanho da origem, capacidade)).
 */
void copiarTexto(char *destino, size_t capacidade, const char *origem);

/*
 * textosIguaisIgnorandoCaixa
 * Compara duas strings ASCII sem diferenciar maiúsculas de minúsculas.
 *
 * Retorno: 1 se forem iguais, 0 caso contrário (também se alguma for NULL).
 * Complexidade: O(k), k = tamanho da menor string.
 */
int textosIguaisIgnorandoCaixa(const char *a, const char *b);

/*
 * apararEspacos
 * Remove espaços no começo e no fim, no próprio buffer.
 *
 * Retorno: nenhum.
 * Complexidade: O(k), k = tamanho do texto.
 */
void apararEspacos(char *texto);

/*
 * textoEhNumero
 * Diz se o texto tem ao menos um dígito e nada além de dígitos.
 * Usado para separar "25" (número da Pokédex) de "Pikachu" (nome).
 *
 * Retorno: 1 se for número, 0 caso contrário.
 * Complexidade: O(k).
 */
int textoEhNumero(const char *texto);

#endif
