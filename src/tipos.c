/*
 * tipos.c
 * -------
 * Matriz de efetividade da 1ª geração (sem os tipos de gerações
 * posteriores e sem o bug antigo de Fantasma x Psíquico, que aqui
 * vale 2x como a tabela ensina). Ver tipos.h.
 */

#include "tipos.h"

static int tabela[TIPO_CONTAGEM][TIPO_CONTAGEM];
static int tabela_pronta = 0;

static const char *NOMES_TIPO[TIPO_CONTAGEM] = {
    "Normal",
    "Fogo",
    "Água",
    "Elétrico",
    "Grama",
    "Gelo",
    "Lutador",
    "Veneno",
    "Terra",
    "Voador",
    "Psíquico",
    "Inseto",
    "Pedra",
    "Fantasma",
    "Dragão"
};

static void definir(Tipo ataque, Tipo defesa, int valor) {
    tabela[ataque][defesa] = valor;
}

static void preencherTabela(void) {
    int i;
    int j;

    for (i = 0; i < TIPO_CONTAGEM; i++) {
        for (j = 0; j < TIPO_CONTAGEM; j++) {
            tabela[i][j] = 100;
        }
    }

    /* Normal */
    definir(TIPO_NORMAL, TIPO_PEDRA, 50);
    definir(TIPO_NORMAL, TIPO_FANTASMA, 0);

    /* Fogo */
    definir(TIPO_FOGO, TIPO_FOGO, 50);
    definir(TIPO_FOGO, TIPO_AGUA, 50);
    definir(TIPO_FOGO, TIPO_GRAMA, 200);
    definir(TIPO_FOGO, TIPO_GELO, 200);
    definir(TIPO_FOGO, TIPO_INSETO, 200);
    definir(TIPO_FOGO, TIPO_PEDRA, 50);
    definir(TIPO_FOGO, TIPO_DRAGAO, 50);

    /* Água */
    definir(TIPO_AGUA, TIPO_FOGO, 200);
    definir(TIPO_AGUA, TIPO_AGUA, 50);
    definir(TIPO_AGUA, TIPO_GRAMA, 50);
    definir(TIPO_AGUA, TIPO_TERRA, 200);
    definir(TIPO_AGUA, TIPO_PEDRA, 200);
    definir(TIPO_AGUA, TIPO_DRAGAO, 50);

    /* Elétrico */
    definir(TIPO_ELETRICO, TIPO_AGUA, 200);
    definir(TIPO_ELETRICO, TIPO_ELETRICO, 50);
    definir(TIPO_ELETRICO, TIPO_GRAMA, 50);
    definir(TIPO_ELETRICO, TIPO_TERRA, 0);
    definir(TIPO_ELETRICO, TIPO_VOADOR, 200);
    definir(TIPO_ELETRICO, TIPO_DRAGAO, 50);

    /* Grama */
    definir(TIPO_GRAMA, TIPO_FOGO, 50);
    definir(TIPO_GRAMA, TIPO_AGUA, 200);
    definir(TIPO_GRAMA, TIPO_GRAMA, 50);
    definir(TIPO_GRAMA, TIPO_VENENO, 50);
    definir(TIPO_GRAMA, TIPO_TERRA, 200);
    definir(TIPO_GRAMA, TIPO_VOADOR, 50);
    definir(TIPO_GRAMA, TIPO_INSETO, 50);
    definir(TIPO_GRAMA, TIPO_PEDRA, 200);
    definir(TIPO_GRAMA, TIPO_DRAGAO, 50);

    /* Gelo */
    definir(TIPO_GELO, TIPO_FOGO, 50);
    definir(TIPO_GELO, TIPO_AGUA, 50);
    definir(TIPO_GELO, TIPO_GRAMA, 200);
    definir(TIPO_GELO, TIPO_GELO, 50);
    definir(TIPO_GELO, TIPO_TERRA, 200);
    definir(TIPO_GELO, TIPO_VOADOR, 200);
    definir(TIPO_GELO, TIPO_DRAGAO, 200);

    /* Lutador */
    definir(TIPO_LUTADOR, TIPO_NORMAL, 200);
    definir(TIPO_LUTADOR, TIPO_GELO, 200);
    definir(TIPO_LUTADOR, TIPO_VENENO, 50);
    definir(TIPO_LUTADOR, TIPO_VOADOR, 50);
    definir(TIPO_LUTADOR, TIPO_PSIQUICO, 50);
    definir(TIPO_LUTADOR, TIPO_INSETO, 50);
    definir(TIPO_LUTADOR, TIPO_PEDRA, 200);
    definir(TIPO_LUTADOR, TIPO_FANTASMA, 0);

    /* Veneno */
    definir(TIPO_VENENO, TIPO_GRAMA, 200);
    definir(TIPO_VENENO, TIPO_VENENO, 50);
    definir(TIPO_VENENO, TIPO_TERRA, 50);
    definir(TIPO_VENENO, TIPO_PEDRA, 50);
    definir(TIPO_VENENO, TIPO_FANTASMA, 50);

    /* Terra */
    definir(TIPO_TERRA, TIPO_FOGO, 200);
    definir(TIPO_TERRA, TIPO_ELETRICO, 200);
    definir(TIPO_TERRA, TIPO_GRAMA, 50);
    definir(TIPO_TERRA, TIPO_VENENO, 200);
    definir(TIPO_TERRA, TIPO_VOADOR, 0);
    definir(TIPO_TERRA, TIPO_INSETO, 50);
    definir(TIPO_TERRA, TIPO_PEDRA, 200);

    /* Voador */
    definir(TIPO_VOADOR, TIPO_ELETRICO, 50);
    definir(TIPO_VOADOR, TIPO_GRAMA, 200);
    definir(TIPO_VOADOR, TIPO_LUTADOR, 200);
    definir(TIPO_VOADOR, TIPO_INSETO, 200);
    definir(TIPO_VOADOR, TIPO_PEDRA, 50);

    /* Psíquico */
    definir(TIPO_PSIQUICO, TIPO_LUTADOR, 200);
    definir(TIPO_PSIQUICO, TIPO_VENENO, 200);
    definir(TIPO_PSIQUICO, TIPO_PSIQUICO, 50);

    /* Inseto */
    definir(TIPO_INSETO, TIPO_FOGO, 50);
    definir(TIPO_INSETO, TIPO_GRAMA, 200);
    definir(TIPO_INSETO, TIPO_LUTADOR, 50);
    definir(TIPO_INSETO, TIPO_VENENO, 50);
    definir(TIPO_INSETO, TIPO_VOADOR, 50);
    definir(TIPO_INSETO, TIPO_PSIQUICO, 200);
    definir(TIPO_INSETO, TIPO_FANTASMA, 50);

    /* Pedra */
    definir(TIPO_PEDRA, TIPO_FOGO, 200);
    definir(TIPO_PEDRA, TIPO_GELO, 200);
    definir(TIPO_PEDRA, TIPO_LUTADOR, 50);
    definir(TIPO_PEDRA, TIPO_TERRA, 50);
    definir(TIPO_PEDRA, TIPO_VOADOR, 200);
    definir(TIPO_PEDRA, TIPO_INSETO, 200);

    /* Fantasma: o 2x contra Psíquico é o valor de tabela, não o bug da 1ª geração. */
    definir(TIPO_FANTASMA, TIPO_NORMAL, 0);
    definir(TIPO_FANTASMA, TIPO_PSIQUICO, 200);
    definir(TIPO_FANTASMA, TIPO_FANTASMA, 200);

    /* Dragão */
    definir(TIPO_DRAGAO, TIPO_DRAGAO, 200);
}

void inicializarTipos(void) {
    if (tabela_pronta) {
        return;
    }
    preencherTabela();
    tabela_pronta = 1;
}

const char *nomeTipo(Tipo tipo) {
    if (tipo == TIPO_NENHUM) {
        return "—";
    }
    if (tipo < 0 || tipo >= TIPO_CONTAGEM) {
        return "?";
    }
    return NOMES_TIPO[tipo];
}

int efetividadePar(Tipo ataque, Tipo defesa) {
    inicializarTipos();
    if (ataque < 0 || ataque >= TIPO_CONTAGEM) {
        return 100;
    }
    if (defesa < 0 || defesa >= TIPO_CONTAGEM) {
        return 100;
    }
    return tabela[ataque][defesa];
}

int multiplicadorEfetividade(Tipo ataque, Tipo def1, Tipo def2) {
    int e1 = efetividadePar(ataque, def1);
    int e2 = (def2 == TIPO_NENHUM) ? 100 : efetividadePar(ataque, def2);
    return (e1 * e2) / 100;
}
