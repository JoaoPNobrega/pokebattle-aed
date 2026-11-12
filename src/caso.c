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
                   "Equipe Rocket. Este laboratorio e meu. Saia enquanto pode.");
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

    preencherSala(&sala, "Laboratorio",
                  "Tanques vazios e um painel sem luz. Foi aqui que criaram o Mewtwo.",
                  "O painel esta morto. Um choque pode religar a sala.",
                  "As luzes acendem. A porta do patio destrava.",
                  HAB_ELETRICA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Patio",
                  "Um Charmander selvagem circula entre as caixas quebradas.",
                  "Nao ha puzzle. Ha um Pokemon selvagem.",
                  "O patio fica quieto.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Camara fria",
                  "A porta seguinte esta selada por uma placa de gelo.",
                  "O gelo nao cede. Uma chama abriria caminho.",
                  "O gelo derrete. A porta do tanque abre.",
                  HAB_FOGO, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Tanque",
                  "Um Squirtle nada no tanque de contencao, assustado.",
                  "Nao ha puzzle. Ha um Pokemon selvagem.",
                  "O tanque esvazia o suficiente para passar.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Oficina",
                  "Um incendio pequeno bloqueia a estante de arquivos.",
                  "Agua apagaria isso. Fogo so piora.",
                  "O fogo apaga. Da para chegar na jaula.",
                  HAB_AGUA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Jaula",
                  "Um Alakazam de teste, ainda fraco, observa atras das grades.",
                  "Nao ha puzzle. Ha um Pokemon selvagem.",
                  "A jaula abre.",
                  HAB_NENHUMA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Arquivo",
                  "Uma estante pesada cobre a porta da sala do chefe.",
                  "Ninguem empurra isso com as maos. Uma mente, talvez.",
                  "A estante desliza. A sala do chefe aparece.",
                  HAB_PSIQUICA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Sala do chefe",
                  "Giovanni espera. A Equipe Rocket nao vai embora sem luta.",
                  "Nao ha puzzle. Ha o chefao.",
                  "Giovanni recua.",
                  HAB_NENHUMA, 1, 0, 1);
    if (!adicionarSala(salas, &sala)) return 0;

    return 1;
}

static int montarFila(FilaTreinadores *fila, HashPokedex *pokedex) {
    if (!enfileirarLuta(fila, pokedex, "Charmander selvagem",
                        "O Charmander rosna, mas nao foge.", "Charmander", 14)) {
        return 0;
    }
    if (!enfileirarLuta(fila, pokedex, "Squirtle selvagem",
                        "O Squirtle se esconde no casco.", "Squirtle", 14)) {
        return 0;
    }
    if (!enfileirarLuta(fila, pokedex, "Alakazam de teste",
                        "O experimento ainda nao esta estavel.", "Alakazam", 8)) {
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

    if (!colocarEspecie(&caso->nucleo.time, &caso->nucleo.pokedex, "Pikachu", 32)) {
        return 0;
    }
    if (!montarSalas(&caso->salas)) {
        return 0;
    }
    if (!montarFila(&caso->nucleo.fila, &caso->nucleo.pokedex)) {
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
        NoSala *sala = salaAtual(&caso->salas);
        char msg[LOG_MAX];

        caso->em_batalha = 0;
        if (sala != NULL) {
            sala->batalha_concluida = 1;
            if (sala->prox != NULL) {
                sala->prox->desbloqueada = 1;
            }
            if (sala->capturavel) {
                caso->captura_pendente = *oponente;
                caso->captura_pendente.hp_atual = caso->captura_pendente.hp_max;
                caso->captura_disponivel = 1;
            }
            if (sala->e_chefe) {
                caso->caso_encerrado = 1;
            }
            snprintf(msg, sizeof msg, "Voce venceu %s.", caso->oponente_atual.nome);
            registrarMensagem(saida, msg);
        }
        soltarOponente(caso);
        return 0;
    }

    oponente = obterAtivo(&caso->oponente_atual.time);
    if (verificarDesmaio(oponente)) {
        if (avancarParaProximoVivo(&caso->oponente_atual.time)) {
            char msg[LOG_MAX];
            snprintf(msg, sizeof msg, "%s enviou %s!",
                     caso->oponente_atual.nome, obterAtivo(&caso->oponente_atual.time)->nome);
            registrarMensagem(saida, msg);
        }
    }

    eu = obterAtivo(&caso->nucleo.time);
    if (verificarDesmaio(eu)) {
        if (avancarParaProximoVivo(&caso->nucleo.time)) {
            char msg[LOG_MAX];
            snprintf(msg, sizeof msg, "Voce enviou %s!", obterAtivo(&caso->nucleo.time)->nome);
            registrarMensagem(saida, msg);
        } else {
            caso->em_batalha = 0;
            registrarMensagem(saida, "Seu time desmaiou.");
            return -1;
        }
    }
    return 1;
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
