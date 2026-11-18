/* Regras da luta */

#include "batalha.h"

#include "ordenacao.h"
#include "util.h"

#include <stdio.h>

void registrarMensagem(SaidaBatalha *saida, const char *mensagem) {
    if (saida == NULL || mensagem == NULL) {
        return;
    }
    if (saida->log != NULL) {
        empilharLog(saida->log, mensagem);
    }
    if (saida->narrar != NULL) {
        saida->narrar(saida->contexto, mensagem);
    }
}

int calcularDano(const Pokemon *atacante, const Golpe *golpe, const Pokemon *defensor) {
    long nivel;
    long atk;
    long def;
    long base;
    long dano;
    int efetividade;
    int stab;

    if (atacante == NULL || golpe == NULL || defensor == NULL) {
        return 0;
    }
    if (golpe->poder <= 0) {
        return 0;
    }

    efetividade = multiplicadorEfetividade(golpe->tipo, defensor->tipo1, defensor->tipo2);
    if (efetividade <= 0) {
        return 0;
    }

    nivel = atacante->nivel > 0 ? atacante->nivel : 1;
    atk = ataqueEmBatalha(atacante);
    def = defesaEmBatalha(defensor);
    if (atk < 1) {
        atk = 1;
    }
    if (def < 1) {
        def = 1;
    }

    /* Divisão inteira: arredonda para baixo */
    base = (2 * nivel / 5) + 2;
    base = (base * golpe->poder * atk) / def;
    base = base / 50 + 2;

    dano = (base * efetividade) / 100;
    stab = (golpe->tipo == atacante->tipo1) ||
           (atacante->tipo2 != TIPO_NENHUM && golpe->tipo == atacante->tipo2);
    if (stab) {
        dano = (dano * 3) / 2;
    }
    /* /2 extra para a luta durar mais de um golpe */
    dano = dano / 2;
    if (dano < 1) {
        dano = 1;
    }
    return (int)dano;
}

int escolherGolpeAdversario(const Pokemon *atacante, const Pokemon *defensor) {
    int i;
    int melhor = -1;
    int melhor_dano = -1;

    if (atacante == NULL || defensor == NULL) {
        return -1;
    }

    for (i = 0; i < atacante->qtd_golpes; i++) {
        int dano = calcularDano(atacante, &atacante->golpes[i], defensor);
        if (dano > melhor_dano) {
            melhor_dano = dano;
            melhor = i;
        }
    }
    return melhor;
}

int verificarDesmaio(const Pokemon *pokemon) {
    return pokemon == NULL || pokemon->hp_atual <= 0;
}

static AtorTurno montarAtor(Pokemon *pokemon, int lado, int indice_golpe) {
    AtorTurno ator;

    ator.pokemon = pokemon;
    ator.lado = lado;
    ator.indice_golpe = indice_golpe;
    ator.prioridade = 0;
    ator.velocidade = velocidadeEmBatalha(pokemon);
    if (indice_golpe >= 0 && indice_golpe < pokemon->qtd_golpes) {
        ator.prioridade = pokemon->golpes[indice_golpe].prioridade;
    }
    return ator;
}

static int golpeValido(const Pokemon *pokemon, int indice) {
    return pokemon != NULL && indice >= 0 && indice < pokemon->qtd_golpes;
}

int definirOrdemTurno(ListaTurno *ordem,
                      Pokemon *jogador, int golpe_jogador,
                      Pokemon *oponente, int golpe_oponente) {
    AtorTurno atores[2];
    int n = 0;
    int i;

    if (ordem == NULL) {
        return 0;
    }

    /* Monta a lista circular deste turno */
    liberarListaTurno(ordem);

    if (!verificarDesmaio(jogador) && golpeValido(jogador, golpe_jogador)) {
        atores[n] = montarAtor(jogador, LADO_JOGADOR, golpe_jogador);
        n++;
    }
    if (!verificarDesmaio(oponente) && golpeValido(oponente, golpe_oponente)) {
        atores[n] = montarAtor(oponente, LADO_OPONENTE, golpe_oponente);
        n++;
    }

    /* Insertion sort estável: empate deixa quem entrou primeiro */
    ordenarTurnoPorInsercao(atores, n);

    for (i = 0; i < n; i++) {
        if (!inserirFimTurno(ordem, &atores[i])) {
            break;
        }
    }
    return tamanhoListaTurno(ordem);
}

static void anunciarOrdem(const ListaTurno *ordem, SaidaBatalha *saida) {
    char msg[LOG_MAX];
    NoTurno *no;
    int n;

    if (ordem == NULL) {
        return;
    }

    n = tamanhoListaTurno(ordem);
    no = inicioDoCiclo(ordem);
    if (n <= 0 || no == NULL || no->ator.pokemon == NULL) {
        return;
    }

    if (n == 1) {
        snprintf(msg, sizeof msg, "Ordem de ação: só %s vai agir.", no->ator.pokemon->nome);
        registrarMensagem(saida, msg);
        return;
    }

    if (no->prox != NULL && no->prox->ator.pokemon != NULL) {
        snprintf(msg, sizeof msg,
                 "Ordem de ação: %s (vel %d, prioridade %d) age antes de %s (vel %d, prioridade %d).",
                 no->ator.pokemon->nome, no->ator.velocidade, no->ator.prioridade,
                 no->prox->ator.pokemon->nome, no->prox->ator.velocidade, no->prox->ator.prioridade);
        registrarMensagem(saida, msg);
    }
}

static void aplicarGolpe(Pokemon *atacante, int indice, Pokemon *defensor, SaidaBatalha *saida) {
    const Golpe *golpe;
    int efetividade;
    int dano;
    char msg[LOG_MAX];

    if (verificarDesmaio(atacante) || verificarDesmaio(defensor)) {
        return;
    }
    if (!golpeValido(atacante, indice)) {
        return;
    }

    golpe = &atacante->golpes[indice];
    efetividade = multiplicadorEfetividade(golpe->tipo, defensor->tipo1, defensor->tipo2);
    dano = calcularDano(atacante, golpe, defensor);

    snprintf(msg, sizeof msg, "%s usou %s!", atacante->nome, golpe->nome);
    registrarMensagem(saida, msg);

    if (efetividade <= 0 || dano <= 0) {
        snprintf(msg, sizeof msg, "Não teve efeito em %s!", defensor->nome);
        registrarMensagem(saida, msg);
        return;
    }

    if (defensor->hp_atual <= dano) {
        defensor->hp_atual = 0;
    } else {
        defensor->hp_atual -= dano;
    }

    snprintf(msg, sizeof msg, "Causou %d de dano. %s ficou com %d/%d de HP.",
             dano, defensor->nome, defensor->hp_atual, defensor->hp_max);
    registrarMensagem(saida, msg);

    if (efetividade >= 400) {
        registrarMensagem(saida, "É extremamente efetivo!");
    } else if (efetividade > 100) {
        registrarMensagem(saida, "É super efetivo!");
    } else if (efetividade < 100) {
        registrarMensagem(saida, "Não foi muito efetivo...");
    }

    if (verificarDesmaio(defensor)) {
        snprintf(msg, sizeof msg, "%s desmaiou!", defensor->nome);
        registrarMensagem(saida, msg);
    }
}

void executarTurno(ListaTurno *ordem, Pokemon *jogador, Pokemon *oponente, SaidaBatalha *saida) {
    NoTurno *no;
    int i;
    int n;

    if (ordem == NULL) {
        return;
    }

    anunciarOrdem(ordem, saida);
    n = tamanhoListaTurno(ordem);
    no = inicioDoCiclo(ordem);

    for (i = 0; i < n && no != NULL; i++) {
        AtorTurno *ator = &no->ator;
        Pokemon *alvo = (ator->lado == LADO_JOGADOR) ? oponente : jogador;

        /* Desmaiado não age */
        if (!verificarDesmaio(ator->pokemon)) {
            aplicarGolpe(ator->pokemon, ator->indice_golpe, alvo, saida);
        }
        no = proximoDoCiclo(no);
    }
}

static int somaHp(const ListaTime *lista) {
    int i;
    int soma = 0;
    int n = tamanhoTime(lista);

    for (i = 0; i < n; i++) {
        const Pokemon *pokemon = pokemonNaPosicao(lista, i);
        if (pokemon != NULL && pokemon->hp_atual > 0) {
            soma += pokemon->hp_atual;
        }
    }
    return soma;
}

static void narrarTexto(SaidaBatalha *saida, const char *texto) {
    registrarMensagem(saida, texto);
}

int conduzirBatalha(ListaTime *time_jogador,
                    Treinador *treinador,
                    PilhaLog *log,
                    PedirAcaoJogador pedir_acao,
                    PedirTrocaJogador pedir_troca,
                    Narrador narrar,
                    void *contexto) {
    SaidaBatalha saida;
    ListaTime *time_oponente;
    char msg[LOG_MAX];
    int turno = 1;

    if (time_jogador == NULL || treinador == NULL) {
        return 0;
    }

    saida.narrar = narrar;
    saida.contexto = contexto;
    saida.log = log;
    time_oponente = &treinador->time;

    if (tamanhoTime(time_jogador) == 0 || tamanhoTime(time_oponente) == 0) {
        return 0;
    }

    definirAtivoPorPosicao(time_jogador, 0);
    definirAtivoPorPosicao(time_oponente, 0);

    snprintf(msg, sizeof msg, "=== %s entra na arena ===", treinador->nome);
    narrarTexto(&saida, msg);
    if (treinador->fala[0] != '\0') {
        narrarTexto(&saida, treinador->fala);
    }

    snprintf(msg, sizeof msg, "%s enviou %s!", treinador->nome, obterAtivo(time_oponente)->nome);
    narrarTexto(&saida, msg);
    snprintf(msg, sizeof msg, "Você enviou %s!", obterAtivo(time_jogador)->nome);
    narrarTexto(&saida, msg);

    while (turno <= MAX_TURNOS) {
        Pokemon *ativo_jogador;
        Pokemon *ativo_oponente;
        VisaoBatalha visao;
        AcaoJogador acao;
        ListaTurno ordem;
        int golpe_oponente;

        if (timeFoiDerrotado(time_jogador)) {
            narrarTexto(&saida, "Seu time inteiro desmaiou...");
            return 0;
        }
        if (timeFoiDerrotado(time_oponente)) {
            snprintf(msg, sizeof msg, "Você venceu %s!", treinador->nome);
            narrarTexto(&saida, msg);
            return 1;
        }

        if (verificarDesmaio(obterAtivo(time_oponente))) {
            if (!avancarParaProximoVivo(time_oponente)) {
                snprintf(msg, sizeof msg, "Você venceu %s!", treinador->nome);
                narrarTexto(&saida, msg);
                return 1;
            }
            snprintf(msg, sizeof msg, "%s enviou %s!", treinador->nome, obterAtivo(time_oponente)->nome);
            narrarTexto(&saida, msg);
        }

        if (verificarDesmaio(obterAtivo(time_jogador))) {
            int posicao = -1;

            visao.time_jogador = time_jogador;
            visao.time_oponente = time_oponente;
            visao.nome_treinador = treinador->nome;
            visao.numero_turno = turno;
            visao.log = log;

            if (pedir_troca == NULL || !pedir_troca(contexto, &visao, &posicao)) {
                return -2;
            }
            if (!definirAtivoPorPosicao(time_jogador, posicao) ||
                verificarDesmaio(obterAtivo(time_jogador))) {
                narrarTexto(&saida, "Não havia Pokémon para continuar.");
                return 0;
            }
            snprintf(msg, sizeof msg, "Você enviou %s!", obterAtivo(time_jogador)->nome);
            narrarTexto(&saida, msg);
        }

        visao.time_jogador = time_jogador;
        visao.time_oponente = time_oponente;
        visao.nome_treinador = treinador->nome;
        visao.numero_turno = turno;
        visao.log = log;

        if (pedir_acao == NULL || !pedir_acao(contexto, &visao, &acao)) {
            return -2;
        }

        if (acao.tipo == ACAO_DESISTIR) {
            narrarTexto(&saida, "Você desistiu da arena.");
            return -1;
        }

        ativo_jogador = obterAtivo(time_jogador);
        ativo_oponente = obterAtivo(time_oponente);

        if (acao.tipo == ACAO_TROCAR) {
            const Pokemon *escolhido = pokemonNaPosicao(time_jogador, acao.indice);

            if (escolhido == NULL || escolhido->hp_atual <= 0 ||
                acao.indice == posicaoDoAtivo(time_jogador)) {
                narrarTexto(&saida, "A troca não aconteceu.");
                continue;
            }

            definirAtivoPorPosicao(time_jogador, acao.indice);
            ativo_jogador = obterAtivo(time_jogador);
            snprintf(msg, sizeof msg, "Você trocou para %s!", ativo_jogador->nome);
            narrarTexto(&saida, msg);

            /* Troca gasta o turno: só o oponente age */
            golpe_oponente = escolherGolpeAdversario(ativo_oponente, ativo_jogador);
            criarListaTurno(&ordem);
            definirOrdemTurno(&ordem, NULL, -1, ativo_oponente, golpe_oponente);
            executarTurno(&ordem, ativo_jogador, ativo_oponente, &saida);
            liberarListaTurno(&ordem);
            turno++;
            continue;
        }

        golpe_oponente = escolherGolpeAdversario(ativo_oponente, ativo_jogador);
        criarListaTurno(&ordem);
        definirOrdemTurno(&ordem, ativo_jogador, acao.indice, ativo_oponente, golpe_oponente);
        executarTurno(&ordem, ativo_jogador, ativo_oponente, &saida);
        liberarListaTurno(&ordem);
        turno++;
    }

    narrarTexto(&saida, "A batalha passou do limite de turnos.");
    if (somaHp(time_jogador) >= somaHp(time_oponente)) {
        snprintf(msg, sizeof msg, "Pelo HP restante, você vence %s.", treinador->nome);
        narrarTexto(&saida, msg);
        return 1;
    }
    narrarTexto(&saida, "Pelo HP restante, você perde esta batalha.");
    return 0;
}

int conduzirArena(ListaTime *time_jogador,
                  FilaTreinadores *fila,
                  PilhaLog *log,
                  PedirAcaoJogador pedir_acao,
                  PedirTrocaJogador pedir_troca,
                  Narrador narrar,
                  void *contexto) {
    SaidaBatalha saida;

    saida.narrar = narrar;
    saida.contexto = contexto;
    saida.log = log;

    if (time_jogador == NULL || fila == NULL) {
        return 0;
    }

    while (!filaVazia(fila)) {
        const Treinador *frente;
        Treinador atual;
        char nome_frente[NOME_TREINADOR_MAX];
        char msg[LOG_MAX];
        int restantes;
        int resultado;

        frente = frenteDaFila(fila);
        if (frente == NULL) {
            break;
        }

        /* Copia o nome antes de tirar da fila */
        copiarTexto(nome_frente, sizeof nome_frente, frente->nome);
        restantes = tamanhoFila(fila);
        snprintf(msg, sizeof msg, "Próximo da fila: %s (%d desafio(s) na fila).",
                 nome_frente, restantes);
        registrarMensagem(&saida, msg);

        if (!proximoTreinador(fila, &atual)) {
            break;
        }

        resultado = conduzirBatalha(time_jogador, &atual, log, pedir_acao, pedir_troca, narrar, contexto);
        liberarListaTime(&atual.time);

        if (resultado != 1) {
            return resultado;
        }

        if (!filaVazia(fila)) {
            restaurarTime(time_jogador);
            registrarMensagem(&saida,
                              "Seu time passa no centro Pokémon e recupera todo o HP.");
        }
    }

    registrarMensagem(&saida, "A fila da arena acabou. Você atravessou todos os desafios.");
    return 1;
}
