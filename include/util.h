/* Funções de texto */

#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void copiarTexto(char *destino, size_t capacidade, const char *origem);

/* Compara sem diferenciar maiúsculas */
int textosIguaisIgnorandoCaixa(const char *a, const char *b);

void apararEspacos(char *texto);

/* "25" é número da Pokédex; "Pikachu" é nome */
int textoEhNumero(const char *texto);

#endif
