/*
 * pokemon.c
 * ---------
 * Contas de atributo e a cópia de uma espécie para o nível da luta.
 * Ver pokemon.h.
 */

#include "pokemon.h"

#include <string.h>

int calcularHpMax(int hp_base, int nivel) {
    int hp;

    if (nivel < 1) {
        nivel = 1;
    }
    if (hp_base < 1) {
        hp_base = 1;
    }
    /* Fórmula clássica de HP, sem IV e sem EV. */
    hp = (2 * hp_base * nivel) / 100 + nivel + 10;
    return hp < 1 ? 1 : hp;
}

int calcularAtributo(int base, int nivel) {
    int valor;

    if (nivel < 1) {
        nivel = 1;
    }
    if (base < 1) {
        base = 1;
    }
    valor = (base * nivel) / 50 + 5;
    return valor < 1 ? 1 : valor;
}

int ataqueEmBatalha(const Pokemon *pokemon) {
    if (pokemon == NULL) {
        return 0;
    }
    return calcularAtributo(pokemon->ataque, pokemon->nivel);
}

int defesaEmBatalha(const Pokemon *pokemon) {
    if (pokemon == NULL) {
        return 0;
    }
    return calcularAtributo(pokemon->defesa, pokemon->nivel);
}

int velocidadeEmBatalha(const Pokemon *pokemon) {
    if (pokemon == NULL) {
        return 0;
    }
    return calcularAtributo(pokemon->velocidade, pokemon->nivel);
}

Pokemon pokemonPreparar(const Pokemon *modelo, int nivel) {
    Pokemon preparado;

    memset(&preparado, 0, sizeof preparado);
    if (modelo == NULL) {
        return preparado;
    }

    preparado = *modelo;
    if (nivel < 1) {
        nivel = 1;
    }
    preparado.nivel = nivel;
    preparado.hp_max = calcularHpMax(modelo->hp_base, nivel);
    preparado.hp_atual = preparado.hp_max;
    return preparado;
}
