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

static int enfileirarLuta(FilaTreinadores *fila, HashPokedex *pokedex,
                          const char *nome, const char *fala,
                          const char *especie, int nivel) {
    Treinador treinador;

    criarTreinador(&treinador, nome, fala);
    if (!colocarEspecie(&treinador.time, pokedex, especie, nivel)) {
        liberarListaTime(&treinador.time);
        return 0;
    }
    if (!enfileirarTreinador(fila, &treinador)) {
        liberarListaTime(&treinador.time);
        return 0;
    }
    return 1;
}

static int enfileirarChefe(FilaTreinadores *fila, HashPokedex *pokedex) {
    Treinador chefe;

    criarTreinador(&chefe, "Giovanni",
                   "Equipe Rocket. Este laboratório é meu. Saia enquanto pode.");
    if (!colocarEspecie(&chefe.time, pokedex, "Meowth", 22)) {
        liberarListaTime(&chefe.time);
        return 0;
    }
    if (!colocarEspecie(&chefe.time, pokedex, "Geodude", 20)) {
        liberarListaTime(&chefe.time);
        return 0;
    }
    if (!enfileirarTreinador(fila, &chefe)) {
        liberarListaTime(&chefe.time);
        return 0;
    }
    return 1;
}

static int preencherSala(NoSala *sala, const char *nome, const char *descricao,
                         const char *puzzle, const char *resolvido,
                         Habilidade habilidade, int batalha, int captura, int chefe) {
    memset(sala, 0, sizeof *sala);
    copiarTexto(sala->nome, sizeof sala->nome, nome);
    copiarTexto(sala->descricao, sizeof sala->descricao, descricao);
    copiarTexto(sala->texto_puzzle, sizeof sala->texto_puzzle, puzzle);
    copiarTexto(sala->texto_resolvido, sizeof sala->texto_resolvido, resolvido);
    sala->habilidade = habilidade;
    sala->resolvido = (habilidade == HAB_NENHUMA);
    sala->inicia_batalha = batalha;
    sala->capturavel = captura;
    sala->e_chefe = chefe;
    return 1;
}

static int montarSalas(ListaSalas *salas) {
    NoSala sala;

    preencherSala(&sala, "Laboratório",
                  "Tanques vazios e um painel sem luz. Foi aqui que criaram o Mewtwo.",
                  "O painel está morto. Um choque pode religar a sala.",
                  "As luzes acendem. A porta do pátio destrava.",
                  HAB_ELETRICA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Pátio",
                  "Um Charmander selvagem circula entre as caixas quebradas.",
                  "Não há puzzle. Há um Pokémon selvagem.",
                  "O pátio fica quieto.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Câmara fria",
                  "A porta seguinte está selada por uma placa de gelo.",
                  "O gelo não cede. Uma chama abriria caminho.",
                  "O gelo derrete. A porta do tanque abre.",
                  HAB_FOGO, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Tanque",
                  "Um Squirtle nada no tanque de contenção, assustado.",
                  "Não há puzzle. Há um Pokémon selvagem.",
                  "O tanque esvazia o suficiente para passar.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Oficina",
                  "Um incêndio pequeno bloqueia a estante de arquivos.",
                  "Água apagaria isso. Fogo só piora.",
                  "O fogo apaga. Dá para chegar na jaula.",
                  HAB_AGUA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Jaula",
                  "Um Alakazam de teste, ainda fraco, observa atrás das grades.",
                  "Não há puzzle. Há um Pokémon selvagem.",
                  "A jaula abre.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Arquivo",
                  "Uma estante pesada cobre a porta da sala do chefe.",
                  "Ninguém empurra isso com as mãos. Uma mente, talvez.",
                  "A estante desliza. A sala do chefe aparece.",
                  HAB_PSIQUICA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Sala do chefe",
                  "Giovanni espera. A Equipe Rocket não vai embora sem luta.",
                  "Não há puzzle. Há o chefão.",
                  "Giovanni recua.",
                  HAB_NENHUMA, 1, 0, 1);
    if (!adicionarSala(salas, &sala)) return 0;

    return 1;
}

static int montarFila(FilaTreinadores *fila, HashPokedex *pokedex) {
    if (!enfileirarLuta(fila, pokedex, "Charmander selvagem",
                        "O Charmander rosna, mas não foge.", "Charmander", 14)) {
        return 0;
    }
    if (!enfileirarLuta(fila, pokedex, "Squirtle selvagem",
                        "O Squirtle se esconde no casco.", "Squirtle", 14)) {
        return 0;
    }
    if (!enfileirarLuta(fila, pokedex, "Alakazam de teste",
                        "O experimento ainda não está estável.", "Alakazam", 8)) {
        return 0;
    }
    if (!enfileirarChefe(fila, pokedex)) {
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
    caso->caso_encerrado = 0;
    caso->captura_disponivel = 0;
    caso->pocoes = 2;
    caso->super_pocoes = 1;
    caso->pokebolas = 3;
    caso->tentativas_fuga = 0;

    if (!colocarEspecie(&caso->nucleo.time, &caso->nucleo.pokedex, "Pikachu", 32)) {
        return 0;
    }
    if (!montarSalas(&caso->salas)) {
        return 0;
    }
    if (!montarFila(&caso->nucleo.fila, &caso->nucleo.pokedex)) {
        return 0;
    }

    empilharLog(&caso->nucleo.log, "Caso aberto. Você é o detetive. O Pikachu é o seu parceiro.");
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

    if (caso == NULL || caso->caso_encerrado) {
        return 0;
    }

    sala = salaAtual(&caso->salas);
    if (sala == NULL || !sala->inicia_batalha || sala->batalha_concluida) {
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
    caso->tentativas_fuga = 0;
    empilharLog(&caso->nucleo.log, "A luta começou.");
    return 1;
}

static SaidaBatalha *saidaDaBatalha(Caso *caso, SaidaBatalha *saida, SaidaBatalha *local) {
    if (saida == NULL) {
        local->narrar = NULL;
        local->contexto = NULL;
        local->log = &caso->nucleo.log;
        return local;
    }
    if (saida->log == NULL) {
        saida->log = &caso->nucleo.log;
    }
    return saida;
}

static int vencerBatalha(Caso *caso, Pokemon *oponente, SaidaBatalha *saida) {
    NoSala *sala = salaAtual(&caso->salas);
    char msg[LOG_MAX];

    caso->em_batalha = 0;
    if (sala != NULL) {
        sala->batalha_concluida = 1;
        if (sala->prox != NULL) {
            sala->prox->desbloqueada = 1;
        }
        if (sala->capturavel && oponente != NULL) {
            caso->captura_pendente = *oponente;
            caso->captura_pendente.hp_atual = caso->captura_pendente.hp_max;
            caso->captura_disponivel = 1;
        }
        if (sala->e_chefe) {
            caso->caso_encerrado = 1;
        }
        snprintf(msg, sizeof msg, "Você venceu %s.", caso->oponente_atual.nome);
        registrarMensagem(saida, msg);
    }
    soltarOponente(caso);
    return 0;
}

static int aposTurno(Caso *caso, Pokemon *oponente, SaidaBatalha *saida) {
    Pokemon *eu;

    if (timeFoiDerrotado(&caso->oponente_atual.time)) {
        return vencerBatalha(caso, oponente, saida);
    }

    oponente = obterAtivo(&caso->oponente_atual.time);
    if (verificarDesmaio(oponente)) {
        if (avancarParaProximoVivo(&caso->oponente_atual.time)) {
            char msg[LOG_MAX];
            snprintf(msg, sizeof msg, "%s enviou %s!",
                     caso->oponente_atual.nome,
                     obterAtivo(&caso->oponente_atual.time)->nome);
            registrarMensagem(saida, msg);
        }
    }

    eu = obterAtivo(&caso->nucleo.time);
    if (verificarDesmaio(eu)) {
        if (avancarParaProximoVivo(&caso->nucleo.time)) {
            char msg[LOG_MAX];
            snprintf(msg, sizeof msg, "Você enviou %s!",
                     obterAtivo(&caso->nucleo.time)->nome);
            registrarMensagem(saida, msg);
        } else {
            caso->em_batalha = 0;
            registrarMensagem(saida, "Seu time desmaiou.");
            return -1;
        }
    }
    return 1;
}

static int turnoSoDoOponente(Caso *caso, SaidaBatalha *saida) {
    Pokemon *eu = obterAtivo(&caso->nucleo.time);
    Pokemon *oponente = obterAtivo(&caso->oponente_atual.time);
    ListaTurno ordem;
    int golpe;
    int resultado;

    if (eu == NULL || oponente == NULL) {
        return -2;
    }

    golpe = escolherGolpeAdversario(oponente, eu);
    criarListaTurno(&ordem);
    /* Golpe -1 tira o jogador da lista circular. Só o oponente entra. */
    definirOrdemTurno(&ordem, eu, -1, oponente, golpe);
    executarTurno(&ordem, eu, oponente, saida);
    liberarListaTurno(&ordem);
    resultado = aposTurno(caso, oponente, saida);
    return resultado;
}

static int curarAtivo(Caso *caso, int cura, SaidaBatalha *saida) {
    Pokemon *eu = obterAtivo(&caso->nucleo.time);
    int antes;
    char msg[LOG_MAX];

    if (eu == NULL || eu->hp_atual <= 0) {
        registrarMensagem(saida, "Não há quem curar.");
        return 0;
    }
    if (eu->hp_atual >= eu->hp_max) {
        registrarMensagem(saida, "O HP já está cheio.");
        return 0;
    }
    antes = eu->hp_atual;
    eu->hp_atual += cura;
    if (eu->hp_atual > eu->hp_max) {
        eu->hp_atual = eu->hp_max;
    }
    snprintf(msg, sizeof msg, "%s recuperou %d de HP.", eu->nome, eu->hp_atual - antes);
    registrarMensagem(saida, msg);
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

    saida = saidaDaBatalha(caso, saida, &local);
    golpe_oponente = escolherGolpeAdversario(oponente, eu);
    criarListaTurno(&ordem);
    definirOrdemTurno(&ordem, eu, indice_golpe, oponente, golpe_oponente);
    executarTurno(&ordem, eu, oponente, saida);
    liberarListaTurno(&ordem);
    return aposTurno(caso, oponente, saida);
}

const char *casoNomeItem(int item) {
    switch (item) {
    case ITEM_POCAO:
        return "Poção";
    case ITEM_SUPER_POCAO:
        return "Super Poção";
    case ITEM_POKE_BOLA:
        return "Poké Bola";
    default:
        return NULL;
    }
}

int casoQuantidadeItem(const Caso *caso, int item) {
    if (caso == NULL) {
        return 0;
    }
    switch (item) {
    case ITEM_POCAO:
        return caso->pocoes;
    case ITEM_SUPER_POCAO:
        return caso->super_pocoes;
    case ITEM_POKE_BOLA:
        return caso->pokebolas;
    default:
        return 0;
    }
}

int casoUsarItem(Caso *caso, int item, SaidaBatalha *saida) {
    NoSala *sala;
    SaidaBatalha local;
    int *quantidade;
    int cura;

    if (caso == NULL || !caso->em_batalha || !caso->oponente_carregado) {
        return -2;
    }
    saida = saidaDaBatalha(caso, saida, &local);
    sala = salaAtual(&caso->salas);

    if (item == ITEM_POCAO) {
        quantidade = &caso->pocoes;
        cura = 20;
    } else if (item == ITEM_SUPER_POCAO) {
        quantidade = &caso->super_pocoes;
        cura = 50;
    } else if (item == ITEM_POKE_BOLA) {
        Pokemon *oponente = obterAtivo(&caso->oponente_atual.time);

        if (caso->pokebolas <= 0) {
            registrarMensagem(saida, "Não tem mais Poké Bola.");
            return -2;
        }
        if (sala == NULL || !sala->capturavel || sala->e_chefe) {
            registrarMensagem(saida, "Não dá para capturar este Pokémon.");
            return -2;
        }
        caso->pokebolas--;
        if (oponente != NULL && oponente->hp_max > 0 &&
            oponente->hp_atual * 2 <= oponente->hp_max) {
            registrarMensagem(saida, "A Poké Bola prendeu o Pokémon!");
            return vencerBatalha(caso, oponente, saida);
        }
        registrarMensagem(saida, "O Pokémon escapou da Poké Bola!");
        return turnoSoDoOponente(caso, saida);
    } else {
        return -2;
    }

    if (*quantidade <= 0) {
        registrarMensagem(saida, "Não tem mais desse item.");
        return -2;
    }
    if (!curarAtivo(caso, cura, saida)) {
        return -2;
    }
    *quantidade -= 1;
    return turnoSoDoOponente(caso, saida);
}

int casoTentarFugir(Caso *caso, SaidaBatalha *saida) {
    NoSala *sala;
    Pokemon *eu;
    Pokemon *oponente;
    SaidaBatalha local;
    int vel_eu;
    int vel_op;

    if (caso == NULL || !caso->em_batalha || !caso->oponente_carregado) {
        return -2;
    }
    saida = saidaDaBatalha(caso, saida, &local);
    sala = salaAtual(&caso->salas);
    if (sala != NULL && sala->e_chefe) {
        registrarMensagem(saida, "Não dá pra fugir de um chefe!");
        return -2;
    }

    eu = obterAtivo(&caso->nucleo.time);
    oponente = obterAtivo(&caso->oponente_atual.time);
    if (eu == NULL || oponente == NULL) {
        return -2;
    }

    caso->tentativas_fuga++;
    vel_eu = velocidadeEmBatalha(eu);
    vel_op = velocidadeEmBatalha(oponente);
    if (vel_eu * caso->tentativas_fuga >= vel_op) {
        caso->em_batalha = 0;
        registrarMensagem(saida, "Você fugiu em segurança!");
        return 3;
    }

    registrarMensagem(saida, "Não conseguiu fugir!");
    return turnoSoDoOponente(caso, saida);
}

int casoCapturar(Caso *caso) {
    char msg[LOG_MAX];

    if (caso == NULL || !caso->captura_disponivel) {
        return 0;
    }
    if (!adicionarPokemon(&caso->nucleo.time, &caso->captura_pendente)) {
        return 0;
    }

    snprintf(msg, sizeof msg, "%s entrou no time.", caso->captura_pendente.nome);
    empilharLog(&caso->nucleo.log, msg);
    caso->captura_disponivel = 0;
    return 1;
}
