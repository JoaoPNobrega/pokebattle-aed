/*
 * simulacao.c
 * -----------
 * Partida sem janela: opera os desafios espaciais, religa refletores,
 * captura parceiros, enfrenta guardas e percorre as salas até Giovanni.
 * O programa `./bin/pokebattle --simular` usa isto. Ver simulacao.h.
 */

#include "simulacao.h"

#include "caso.h"

#include <stdio.h>

int simularCaptura(Caso *caso) {
    int i;
    if (caso == NULL || !caso->captura_disponivel) return 0;
    for (i = 0; i < 5 && caso->captura_acertos < 5; i++) {
        if (casoResponderCaptura(caso, casoComandoCaptura(caso), 300) < 1) return 0;
    }
    return casoCapturar(caso);
}

int simularPuzzleAtual(Caso *caso) {
    NoSala *sala = caso == NULL ? NULL : caso->salas.atual;
    int i;
    if (sala == NULL) return 0;
    if (sala->resolvido) return 1;
    if (sala->desafio.tipo == DES_CIRCUITO) {
        for (i = 0; i < 9 && !sala->resolvido; i++) {
            int giros = 0;
            while (sala->desafio.rotacao[i] != 0 && giros++ < 4 && !sala->resolvido) {
                if (casoComandarPuzzle(caso, ACAO_USAR, i) < 0) return 0;
            }
        }
    } else if (sala->desafio.tipo == DES_GELO) {
        for (i = 4; i < 8; i++) {
            int tentativas = 0;
            while (sala->desafio.gelo[i] > 0 && tentativas++ < 8) {
                if (sala->desafio.calor >= 66) casoComandarPuzzle(caso, ACAO_RESFRIAR, 0);
                if (casoComandarPuzzle(caso, ACAO_USAR, i) < 0) return 0;
            }
        }
    } else if (sala->desafio.tipo == DES_AGUA) {
        static const int valvulas[] = {0, 1, 2, 1, 0, 1, 3};
        casoComandarPuzzle(caso, ACAO_REINICIAR, 0);
        for (i = 0; i < 7; i++) if (casoComandarPuzzle(caso, ACAO_USAR, valvulas[i]) < 0) return 0;
    } else if (sala->desafio.tipo == DES_RASTRO) {
        static const int ecos[] = {2, 14, 17};
        while (sala->desafio.rastros < 3) {
            if (casoComandarPuzzle(caso, ACAO_USAR, ecos[sala->desafio.rastros]) < 0) return 0;
        }
    } else if (sala->desafio.tipo == DES_ESTANTES) {
        static const AcaoDesafio passos[] = {ACAO_BAIXO, ACAO_DIREITA, ACAO_CIMA,
            ACAO_ESQUERDA, ACAO_CIMA, ACAO_DIREITA, ACAO_DIREITA, ACAO_ESQUERDA,
            ACAO_ESQUERDA, ACAO_BAIXO, ACAO_BAIXO, ACAO_BAIXO, ACAO_DIREITA, ACAO_DIREITA};
        casoComandarPuzzle(caso, ACAO_REINICIAR, 0);
        for (i = 0; i < 14; i++) if (casoComandarPuzzle(caso, passos[i], 0) < 0) return 0;
    }
    return sala->resolvido;
}

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

        if (sala->inicia_batalha && !sala->batalha_concluida) {
            int luta = 1;
            int golpes = 0;

            casoDescansar(&caso);
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
            if (caso.captura_disponivel && !simularCaptura(&caso)) {
                fprintf(stderr, "Não foi possível capturar em %s.\n", sala->nome);
                codigo = -1;
                break;
            }
        }

        /* A Jaula pede o parceiro capturado na própria sala antes da busca. */
        if (!simularPuzzleAtual(&caso)) {
            fprintf(stderr, "Puzzle sem solução em %s.\n", sala->nome);
            codigo = -1;
            break;
        }
        if (sala->luzes_necessarias) {
            casoAcenderPonto(&caso, 0);
            casoAcenderPonto(&caso, 1);
        }
        if (sala->duelo_obrigatorio && !sala->duelo_concluido) {
            int golpes = 0;
            casoDescansar(&caso);
            if (!casoIniciarDuelo(&caso)) { codigo = -1; break; }
            while (caso.em_batalha && golpes++ < 60) {
                int golpe = escolherGolpeAdversario(obterAtivo(&caso.nucleo.time), obterAtivo(&caso.oponente_atual.time));
                if (casoExecutarGolpe(&caso, golpe, &saida) < 0) break;
            }
            if (!sala->duelo_concluido) { codigo = -1; break; }
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
