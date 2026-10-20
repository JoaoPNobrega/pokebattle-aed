/*
 * util.c
 * ------
 * Implementação das ajudas de texto. Ver util.h.
 */

#include "util.h"

#include <ctype.h>

void copiarTexto(char *destino, size_t capacidade, const char *origem) {
    size_t i = 0;

    if (destino == NULL || capacidade == 0) {
        return;
    }

    if (origem != NULL) {
        while (origem[i] != '\0' && i + 1 < capacidade) {
            destino[i] = origem[i];
            i++;
        }
    }
    destino[i] = '\0';
}

int textosIguaisIgnorandoCaixa(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }

    while (*a != '\0' && *b != '\0') {
        unsigned char ca = (unsigned char)*a;
        unsigned char cb = (unsigned char)*b;
        if (tolower(ca) != tolower(cb)) {
            return 0;
        }
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

void apararEspacos(char *texto) {
    char *inicio;
    char *fim;
    char *destino;

    if (texto == NULL) {
        return;
    }

    inicio = texto;
    while (*inicio != '\0' && isspace((unsigned char)*inicio)) {
        inicio++;
    }

    if (*inicio == '\0') {
        texto[0] = '\0';
        return;
    }

    fim = inicio;
    while (*fim != '\0') {
        fim++;
    }
    fim--;
    while (fim > inicio && isspace((unsigned char)*fim)) {
        fim--;
    }

    destino = texto;
    while (inicio <= fim) {
        *destino = *inicio;
        destino++;
        inicio++;
    }
    *destino = '\0';
}

int textoEhNumero(const char *texto) {
    int i;

    if (texto == NULL || texto[0] == '\0') {
        return 0;
    }

    for (i = 0; texto[i] != '\0'; i++) {
        if (!isdigit((unsigned char)texto[i])) {
            return 0;
        }
    }
    return 1;
}
