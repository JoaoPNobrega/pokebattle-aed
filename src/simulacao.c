/*
 * simulacao.c
 * -----------
 * Partida sem janela: resolve o puzzle do laboratório com o Choque
 * do Pikachu, entra no corredor e luta até o capanga cair.
 * O programa `./bin/pokebattle --simular` usa isto. Ver simulacao.h.
 */

#include "simulacao.h"

#include "caso.h"

#include <stdio.h>

static void narrarSimulacao(void *contexto, const char *mensagem) {
    (void)contexto;
    printf("%s\n", mensagem);
}

int simularArenaAutomatica(void) {
    Caso caso;
    SaidaBatalha saida;
    int codigo = 1;
    int turnos = 0;

    if (!iniciarCaso(&caso)) {
        fprintf(stderr, "Falha ao carregar a Pokédex.\n");
        return -1;
    }
    if (!comecarCaso(&caso)) {
        fprintf(stderr, "Falha ao abrir o caso.\n");
        encerrarCaso(&caso);
        return -1;
    }

    printf("Simulação: O Caso do Laboratório\n");
    if (!casoUsarHabilidade(&caso)) {
        fprintf(stderr, "O puzzle do laboratório não foi resolvido.\n");
        encerrarCaso(&caso);
        return 0;
    }
    if (!casoAvancar(&caso)) {
        fprintf(stderr, "Não foi possível entrar no corredor.\n");
        encerrarCaso(&caso);
        return 0;
    }
    if (!casoIniciarBatalha(&caso)) {
        fprintf(stderr, "A luta não começou.\n");
        encerrarCaso(&caso);
        return 0;
    }

    saida.narrar = narrarSimulacao;
    saida.contexto = NULL;
    saida.log = &caso.nucleo.log;

    while (codigo == 1 && turnos < 40) {
        const Pokemon *eu = obterAtivo(&caso.nucleo.time);
        const Pokemon *oponente = obterAtivo(&caso.oponente_atual.time);
        int golpe = escolherGolpeAdversario(eu, oponente);

        codigo = casoExecutarGolpe(&caso, golpe, &saida);
        turnos++;
    }

    if (codigo == 0) {
        printf("RESULTADO: VITORIA\n");
    } else {
        printf("RESULTADO: DERROTA\n");
    }

    encerrarCaso(&caso);
    return codigo == 0 ? 1 : 0;
}
