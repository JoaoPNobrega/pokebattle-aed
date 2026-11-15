/* desafio.c — Regras de circuito, calor, reservatórios, rastros e empurrar estantes.
 * Matrizes fixas representam espaço; a fila local do circuito tem no máximo nove
 * posições, sem alocação ou biblioteca de estruturas. O estado pertence à sala. */
#include "desafio.h"
#include <string.h>

void iniciarDesafio(Desafio *d, TipoDesafio tipo) {
    static const int inicio[9] = {1, 0, 2, 1, 1, 3, 2, 1, 0};
    int i;
    if (d == NULL) return;
    memset(d, 0, sizeof *d);
    d->tipo = tipo;
    for (i = 0; i < 9; i++) d->rotacao[i] = inicio[i];
    for (i = 4; i < 8; i++) d->gelo[i] = 3;
    d->operador = 13;
    d->caixas = (1UL << 14) | (1UL << 26);
    d->concluido = tipo == DES_NENHUM;
}

int mascaraCircuito(const Desafio *d, int celula) {
    static const int pecas[9] = {10, 10, 12, 6, 10, 9, 3, 10, 10};
    int mascara, i;
    if (d == NULL || celula < 0 || celula >= 9) return 0;
    mascara = pecas[celula];
    for (i = 0; i < d->rotacao[celula]; i++) mascara = ((mascara << 1) & 15) | (mascara >> 3);
    return mascara;
}

unsigned int circuitoAlcancado(const Desafio *d) {
    int fila[9], inicio = 0, fim = 0;
    static const int passos[4] = {-3, 1, 3, -1};
    static const int oposto[4] = {4, 8, 1, 2};
    unsigned int vistos = 0;
    if (d == NULL || !(mascaraCircuito(d, 0) & 8)) return 0;
    fila[fim++] = 0; vistos = 1;
    while (inicio < fim) {
        int atual = fila[inicio++], direcao;
        for (direcao = 0; direcao < 4; direcao++) {
            int vizinho = atual + passos[direcao];
            if (vizinho < 0 || vizinho >= 9 ||
                (direcao == 1 && atual % 3 == 2) || (direcao == 3 && atual % 3 == 0)) continue;
            if ((mascaraCircuito(d, atual) & (1 << direcao)) &&
                (mascaraCircuito(d, vizinho) & oposto[direcao]) && !(vistos & (1U << vizinho))) {
                vistos |= 1U << vizinho;
                fila[fim++] = vizinho;
            }
        }
    }
    return vistos;
}

int paredeEstantes(int celula) {
    return celula < 0 || celula >= 36 || celula / 6 == 0 || celula / 6 == 5 ||
           celula % 6 == 0 || celula % 6 == 5 || celula == 15 || celula == 21;
}

int distanciaRastro(const Desafio *d, int celula) {
    static const int ecos[3] = {2, 14, 17};
    int dx, dy;
    if (d == NULL || d->rastros >= 3 || celula < 0 || celula >= 20) return -1;
    dx = celula % 5 - ecos[d->rastros] % 5;
    dy = celula / 5 - ecos[d->rastros] / 5;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}

static int moverEstante(Desafio *d, AcaoDesafio acao) {
    static const int passos[] = {-6, 1, 6, -1};
    int proximo, destino;
    if (acao < ACAO_CIMA || acao > ACAO_ESQUERDA) return -1;
    proximo = d->operador + passos[acao - ACAO_CIMA];
    if (paredeEstantes(proximo)) return 0;
    destino = proximo + passos[acao - ACAO_CIMA];
    if ((d->caixas & (1UL << proximo)) &&
        (paredeEstantes(destino) || (d->caixas & (1UL << destino)))) return 0;
    /* Guarda um estado para desfazer; a máscara move a caixa sem duplicá-la. */
    d->operador_anterior = d->operador;
    d->caixas_anteriores = d->caixas;
    d->pode_desfazer = 1;
    if (d->caixas & (1UL << proximo)) {
        d->caixas &= ~(1UL << proximo);
        d->caixas |= 1UL << destino;
    }
    d->operador = proximo;
    d->concluido = d->caixas == ((1UL << 10) | (1UL << 28));
    return 1;
}

int comandarDesafio(Desafio *d, AcaoDesafio acao, int alvo) {
    int resultado = 1, i;
    if (d == NULL || d->tipo == DES_NENHUM || d->concluido) return -1;
    if (acao == ACAO_REINICIAR) { iniciarDesafio(d, d->tipo); return 1; }
    if (acao == ACAO_DESFAZER) {
        if (d->tipo != DES_ESTANTES || !d->pode_desfazer) return 0;
        d->operador = d->operador_anterior;
        d->caixas = d->caixas_anteriores;
        d->pode_desfazer = 0;
        return 1;
    }
    if (d->tipo == DES_CIRCUITO) {
        if (acao != ACAO_USAR || alvo < 0 || alvo >= 9) return -1;
        d->rotacao[alvo] = (d->rotacao[alvo] + 1) % 4;
        d->concluido = circuitoAlcancado(d) == 511U && (mascaraCircuito(d, 8) & 2);
    } else if (d->tipo == DES_GELO) {
        if (acao == ACAO_RESFRIAR) d->calor = d->calor < 35 ? 0 : d->calor - 35;
        else {
            if (acao != ACAO_USAR || alvo < 0 || alvo >= 12) return -1;
            d->calor += 22;
            if (d->calor >= 100 || alvo == 1 || alvo == 2 || alvo == 8 || alvo == 11) {
                d->calor = 75;
                for (i = 4; i < 8; i++) if (d->gelo[i] < 3) d->gelo[i]++;
                resultado = 0;
            } else if (d->gelo[alvo] > 0) d->gelo[alvo]--;
            else resultado = 0;
        }
        d->concluido = d->gelo[4] + d->gelo[5] + d->gelo[6] + d->gelo[7] == 0;
    } else if (d->tipo == DES_AGUA) {
        int transferir = d->tanque_a < 3 - d->tanque_b ? d->tanque_a : 3 - d->tanque_b;
        if (acao != ACAO_USAR || alvo < 0 || alvo > 3) return -1;
        if (alvo == 0) d->tanque_a = 5;
        if (alvo == 1) { d->tanque_a -= transferir; d->tanque_b += transferir; }
        if (alvo == 2) d->tanque_b = 0;
        if (alvo == 3) {
            d->concluido = d->tanque_a == 4 && d->tanque_b == 3;
            if (!d->concluido) { d->tanque_a = 0; d->tanque_b = 0; resultado = 0; }
        }
    } else if (d->tipo == DES_RASTRO) {
        if (acao != ACAO_USAR || alvo < 0 || alvo >= 20) return -1;
        d->examinados |= 1U << alvo;
        if (distanciaRastro(d, alvo) == 0) { d->rastros++; d->examinados = 0; }
        else resultado = 0;
        d->concluido = d->rastros == 3;
    } else if (d->tipo == DES_ESTANTES) resultado = moverEstante(d, acao);
    else return -1;
    if (resultado >= 0) d->movimentos++;
    return d->concluido ? 2 : resultado;
}
