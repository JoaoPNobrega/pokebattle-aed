/*
 * simulacao.c
 * -----------
 * Partida automática. Os dois lados escolhem o golpe de maior dano.
 * Ver simulacao.h.
 */

#include "simulacao.h"

#include "batalha.h"
#include "jogo.h"

#include <stdio.h>

static void narrarSimulacao(void *contexto, const char *mensagem) {
    (void)contexto;
    printf("%s\n", mensagem);
}

static int acaoAutomatica(void *contexto, const VisaoBatalha *visao, AcaoJogador *acao) {
    const Pokemon *eu;
    const Pokemon *oponente;

    (void)contexto;
    if (visao == NULL || acao == NULL) {
        return 0;
    }

    eu = obterAtivo(visao->time_jogador);
    oponente = obterAtivo(visao->time_oponente);
    acao->tipo = ACAO_GOLPE;
    acao->indice = escolherGolpeAdversario(eu, oponente);
    if (acao->indice < 0) {
        acao->indice = 0;
    }
    return 1;
}

static int trocaAutomatica(void *contexto, const VisaoBatalha *visao, int *posicao) {
    int viva;

    (void)contexto;
    if (visao == NULL || posicao == NULL) {
        return 0;
    }

    viva = primeiraPosicaoViva(visao->time_jogador);
    if (viva < 0) {
        return 0;
    }
    *posicao = viva;
    return 1;
}

static int colocar(Jogo *jogo, const char *nome) {
    const Pokemon *modelo = buscarPokemon(&jogo->pokedex, nome);
    Pokemon preparado;

    if (modelo == NULL) {
        fprintf(stderr, "Espécie ausente na simulação: %s\n", nome);
        return 0;
    }
    preparado = pokemonPreparar(modelo, NIVEL_DO_JOGADOR);
    return adicionarPokemon(&jogo->time, &preparado);
}

int simularArenaAutomatica(void) {
    Jogo jogo;
    int resultado;

    if (!iniciarJogo(&jogo)) {
        fprintf(stderr, "Falha ao carregar a Pokédex.\n");
        return -1;
    }
    if (!reiniciarDesafio(&jogo)) {
        fprintf(stderr, "Falha ao montar a arena.\n");
        encerrarJogo(&jogo);
        return -1;
    }

    /*
     * Líder de água para o Brock, tanque no meio e Dragonite para
     * fechar o Mewtwo se o combate apertar. A ordem é a da lista.
     */
    if (!colocar(&jogo, "Blastoise") || !colocar(&jogo, "Snorlax") ||
        !colocar(&jogo, "Dragonite") || !colocar(&jogo, "Venusauro") ||
        !colocar(&jogo, "Gengar") || !colocar(&jogo, "Alakazam")) {
        encerrarJogo(&jogo);
        return -1;
    }

    printf("Simulação da arena (os dois lados usam a mesma regra de golpe).\n");
    resultado = conduzirArena(&jogo.time, &jogo.fila, &jogo.log,
                              acaoAutomatica, trocaAutomatica, narrarSimulacao, NULL);

    if (resultado == 1) {
        printf("RESULTADO: VITORIA\n");
    } else {
        printf("RESULTADO: DERROTA\n");
    }

    encerrarJogo(&jogo);
    return resultado == 1 ? 1 : 0;
}
