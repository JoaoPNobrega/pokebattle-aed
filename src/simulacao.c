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
    saida.narrar = narrarSimulacao;
    saida.contexto = NULL;
    saida.log = &caso.nucleo.log;

    while (!caso.caso_encerrado && turnos < 30) {
        NoSala *sala = salaAtual(&caso.salas);

        if (sala == NULL) {
            codigo = -1;
            break;
        }
        printf("Sala: %s\n", sala->nome);

        if (!sala->resolvido) {
            if (!casoUsarHabilidade(&caso)) {
                fprintf(stderr, "Puzzle sem solução em %s.\n", sala->nome);
                codigo = -1;
                break;
            }
        }

        if (sala->inicia_batalha && !sala->batalha_concluida) {
            int luta = 1;
            int golpes = 0;

            if (!casoIniciarBatalha(&caso)) {
                fprintf(stderr, "A luta não começou em %s.\n", sala->nome);
                codigo = -1;
                break;
            }
            while (luta == 1 && golpes < 60) {
                const Pokemon *eu = obterAtivo(&caso.nucleo.time);
                const Pokemon *oponente = obterAtivo(&caso.oponente_atual.time);
                int golpe = escolherGolpeAdversario(eu, oponente);
                luta = casoExecutarGolpe(&caso, golpe, &saida);
                golpes++;
            }
            if (luta != 0) {
                codigo = luta;
                break;
            }
            if (caso.captura_disponivel && !casoCapturar(&caso)) {
                fprintf(stderr, "Não foi possível capturar em %s.\n", sala->nome);
                codigo = -1;
                break;
            }
        }

        if (caso.caso_encerrado) {
            codigo = 0;
            break;
        }
        if (!casoAvancar(&caso)) {
            fprintf(stderr, "Travou em %s.\n", sala->nome);
            codigo = -1;
            break;
        }
        turnos++;
    }

    if (codigo == 0 && caso.caso_encerrado) {
        printf("RESULTADO: VITORIA\n");
    } else {
        printf("RESULTADO: DERROTA\n");
        codigo = -1;
    }

    encerrarCaso(&caso);
    return codigo == 0 ? 1 : 0;
}
