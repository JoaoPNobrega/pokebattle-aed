/*
 * caso.c
 * ------
 * Monta o mistério e dá um passo de batalha sem saber desenhar.
 * Ver caso.h. As salas são a lista dupla; a luta reusa a fila,
 * o insertion sort e a lista circular que já existiam.
 */

#include "caso.h"

#include "util.h"

#include <stdio.h>
#include <string.h>

static int colocarEspecie(ListaTime *time, HashPokedex *pokedex, const char *nome, int nivel) {
    const Pokemon *modelo;
    Pokemon preparado;

    modelo = buscarPokemon(pokedex, nome);
    if (modelo == NULL) {
        return 0;
    }
    preparado = pokemonPreparar(modelo, nivel);
    return adicionarPokemon(time, &preparado);
}

static int enfileirarCapanga(FilaTreinadores *fila, HashPokedex *pokedex) {
    Treinador capanga;

    criarTreinador(&capanga, "Capanga Rocket",
                   "O chefe mandou fechar este laboratorio. Some daqui.");
    if (!colocarEspecie(&capanga.time, pokedex, "Meowth", 16)) {
        liberarListaTime(&capanga.time);
        return 0;
    }
    if (!enfileirarTreinador(fila, &capanga)) {
        liberarListaTime(&capanga.time);
        return 0;
    }
    return 1;
}

static int montarSalas(ListaSalas *salas) {
    NoSala laboratorio;
    NoSala corredor;

    memset(&laboratorio, 0, sizeof laboratorio);
    memset(&corredor, 0, sizeof corredor);

    copiarTexto(laboratorio.nome, sizeof laboratorio.nome, "Laboratorio");
    copiarTexto(laboratorio.descricao, sizeof laboratorio.descricao,
                "Tanques vazios, cabos soltos e um painel sem luz. Foi aqui que criaram o Mewtwo.");
    copiarTexto(laboratorio.texto_puzzle, sizeof laboratorio.texto_puzzle,
                "O painel esta morto. Um choque pode religar a sala.");
    copiarTexto(laboratorio.texto_resolvido, sizeof laboratorio.texto_resolvido,
                "As luzes acendem. A porta do corredor destrava.");
    laboratorio.habilidade = HAB_ELETRICA;
    laboratorio.resolvido = 0;
    laboratorio.desbloqueada = 1;
    laboratorio.inicia_batalha = 0;

    copiarTexto(corredor.nome, sizeof corredor.nome, "Corredor");
    copiarTexto(corredor.descricao, sizeof corredor.descricao,
                "Um capanga da Equipe Rocket bloqueia a passagem para o resto do predio.");
    copiarTexto(corredor.texto_puzzle, sizeof corredor.texto_puzzle,
                "Nao ha puzzle aqui. Ha uma luta.");
    copiarTexto(corredor.texto_resolvido, sizeof corredor.texto_resolvido,
                "O corredor fica livre.");
    corredor.habilidade = HAB_NENHUMA;
    corredor.resolvido = 1;
    corredor.desbloqueada = 0;
    corredor.inicia_batalha = 1;

    if (!adicionarSala(salas, &laboratorio)) {
        return 0;
    }
    if (!adicionarSala(salas, &corredor)) {
        return 0;
    }
    return 1;
}

int iniciarCaso(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }
    memset(caso, 0, sizeof *caso);
    if (!iniciarJogo(&caso->nucleo)) {
        return 0;
    }
    criarListaSalas(&caso->salas);
    return 1;
}

static void soltarOponente(Caso *caso) {
    if (caso != NULL && caso->oponente_carregado) {
        liberarListaTime(&caso->oponente_atual.time);
        caso->oponente_carregado = 0;
        caso->em_batalha = 0;
    }
}

void encerrarCaso(Caso *caso) {
    if (caso == NULL) {
        return;
    }
    soltarOponente(caso);
    liberarListaSalas(&caso->salas);
    encerrarJogo(&caso->nucleo);
}

int comecarCaso(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }

    soltarOponente(caso);
    liberarListaTime(&caso->nucleo.time);
    liberarFilaTreinadores(&caso->nucleo.fila);
    liberarPilhaLog(&caso->nucleo.log);
    liberarListaSalas(&caso->salas);
    criarListaTime(&caso->nucleo.time);
    criarFilaTreinadores(&caso->nucleo.fila);
    criarPilhaLog(&caso->nucleo.log);
    criarListaSalas(&caso->salas);
    caso->batalha_vencida = 0;

    if (!colocarEspecie(&caso->nucleo.time, &caso->nucleo.pokedex, "Pikachu", 28)) {
        return 0;
    }
    if (!montarSalas(&caso->salas)) {
        return 0;
    }
    if (!enfileirarCapanga(&caso->nucleo.fila, &caso->nucleo.pokedex)) {
        return 0;
    }

    empilharLog(&caso->nucleo.log, "Caso aberto. Voce e o detetive. O Pikachu e o seu parceiro.");
    return 1;
}

int casoUsarHabilidade(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }
    return resolverPuzzleDaSala(&caso->salas, &caso->nucleo.time, &caso->nucleo.log);
}

int casoAvancar(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }
    return avancarSala(&caso->salas, &caso->nucleo.log);
}

int casoVoltar(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }
    return voltarSala(&caso->salas, &caso->nucleo.log);
}

int casoIniciarBatalha(Caso *caso) {
    NoSala *sala;

    if (caso == NULL || caso->batalha_vencida) {
        return 0;
    }

    sala = salaAtual(&caso->salas);
    if (sala == NULL || !sala->inicia_batalha) {
        return 0;
    }

    if (!caso->oponente_carregado) {
        if (!proximoTreinador(&caso->nucleo.fila, &caso->oponente_atual)) {
            return 0;
        }
        caso->oponente_carregado = 1;
        definirAtivoPorPosicao(&caso->oponente_atual.time, 0);
        definirAtivoPorPosicao(&caso->nucleo.time, 0);
    } else {
        restaurarTime(&caso->nucleo.time);
        restaurarTime(&caso->oponente_atual.time);
    }

    caso->em_batalha = 1;
    empilharLog(&caso->nucleo.log, "A luta comecou.");
    return 1;
}

int casoExecutarGolpe(Caso *caso, int indice_golpe, SaidaBatalha *saida) {
    Pokemon *eu;
    Pokemon *oponente;
    ListaTurno ordem;
    int golpe_oponente;
    SaidaBatalha local;

    if (caso == NULL || !caso->em_batalha || !caso->oponente_carregado) {
        return -2;
    }

    eu = obterAtivo(&caso->nucleo.time);
    oponente = obterAtivo(&caso->oponente_atual.time);
    if (eu == NULL || oponente == NULL) {
        return -2;
    }
    if (indice_golpe < 0 || indice_golpe >= eu->qtd_golpes) {
        return -2;
    }

    if (saida == NULL) {
        local.narrar = NULL;
        local.contexto = NULL;
        local.log = &caso->nucleo.log;
        saida = &local;
    } else if (saida->log == NULL) {
        saida->log = &caso->nucleo.log;
    }

    golpe_oponente = escolherGolpeAdversario(oponente, eu);
    criarListaTurno(&ordem);
    definirOrdemTurno(&ordem, eu, indice_golpe, oponente, golpe_oponente);
    executarTurno(&ordem, eu, oponente, saida);
    liberarListaTurno(&ordem);

    if (timeFoiDerrotado(&caso->oponente_atual.time)) {
        caso->em_batalha = 0;
        caso->batalha_vencida = 1;
        soltarOponente(caso);
        registrarMensagem(saida, "O capanga fugiu. O corredor e seu.");
        return 0;
    }
    if (verificarDesmaio(eu)) {
        caso->em_batalha = 0;
        registrarMensagem(saida, "Seu Pokemon desmaiou.");
        return -1;
    }
    return 1;
}
