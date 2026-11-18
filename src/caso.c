/*
 * caso.c
 * ------
 * Monta o mistério e dá um passo de batalha sem saber desenhar.
 * Ver caso.h. As salas são a lista dupla; a luta reusa a fila,
 * o insertion sort e a lista circular que já existiam.
 * Etapas ficam no nó da sala. Treinos usam o oponente temporário,
 * preservando a fila das lutas obrigatórias e reservando vagas no time.
 */

#include "caso.h"

#include "util.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

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
    if (!colocarEspecie(&chefe.time, pokedex, "Mewtwo", 14)) {
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
    iniciarDesafio(&sala->desafio, habilidade == HAB_ELETRICA ? DES_CIRCUITO :
                   habilidade == HAB_FOGO ? DES_GELO : habilidade == HAB_AGUA ? DES_AGUA :
                   habilidade == HAB_PSIQUICA ? (strcmp(nome, "Arquivo") == 0 ? DES_ESTANTES : DES_RASTRO) : DES_NENHUM);
    sala->luzes_necessarias = habilidade != HAB_NENHUMA ? 3U : 0U;
    sala->duelo_obrigatorio = strcmp(nome, "Câmara fria") == 0 || strcmp(nome, "Oficina") == 0;
    return 1;
}

static int montarSalas(ListaSalas *salas) {
    NoSala sala;

    preencherSala(&sala, "Laboratório",
                  "Tanques vazios e um painel sem luz. Foi aqui que criaram o Mewtwo.",
                  "Gire os fios com Z para ligar a fonte aos três terminais. Depois use Pikachu nos dois refletores do mapa.",
                  "Gerador restaurado! Agora religue os dois refletores para liberar a passagem.",
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
                  "Derreta as quatro travas sem atingir os tubos. Z lança chama na célula selecionada. E resfria. Superaquecer faz o gelo voltar.",
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
                  "Abasteça o tanque A (5), transfira para B (3) e drene B. Reserve 4 em A e 3 em B antes de liberar a mangueira.",
                  "O fogo apaga. Dá para chegar na jaula.",
                  HAB_AGUA, 0, 0, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Jaula",
                  "Um Alakazam de teste, ainda fraco, observa atrás das grades.",
                  "Capture Alakazam. Examine a grade com Z: o eco indica a distância do rastro. Encontre três pistas para revelar o fundo falso.",
                  "Alakazam examina a área e ergue um fundo falso. Documento: Giovanni está além do Arquivo. Mova a estante com a mente. Contra Geodude, use água! A porta do Arquivo destrava.",
                  HAB_PSIQUICA, 1, 1, 0);
    if (!adicionarSala(salas, &sala)) return 0;

    preencherSala(&sala, "Arquivo",
                  "Uma estante pesada cobre a porta da sala do chefe.",
                  "Com Alakazam, empurre duas estantes até as marcas douradas. Não é possível puxar. U desfaz; R reinicia; setas movem.",
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
                        "O Charmander rosna, mas não foge.", "Charmander", 18)) {
        return 0;
    }
    if (!enfileirarLuta(fila, pokedex, "Vigia Rocket",
                        "Você religou os refletores? Não passa sem me vencer!", "Meowth", 24)) return 0;
    if (!enfileirarLuta(fila, pokedex, "Squirtle selvagem",
                        "O Squirtle se esconde no casco.", "Squirtle", 18)) {
        return 0;
    }
    {
        Treinador engenheiro;
        criarTreinador(&engenheiro, "Engenheira Rocket", "Minha equipe protege o acesso à contenção. Prepare-se!");
        if (!colocarEspecie(&engenheiro.time, pokedex, "Geodude", 16) ||
            !colocarEspecie(&engenheiro.time, pokedex, "Meowth", 22) ||
            !enfileirarTreinador(fila, &engenheiro)) {
            liberarListaTime(&engenheiro.time);
            return 0;
        }
    }
    if (!enfileirarLuta(fila, pokedex, "Alakazam de teste",
                        "O experimento ainda não está estável.", "Alakazam", 18)) {
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
    criarListaTime(&caso->reserva_pc);
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
    liberarListaTime(&caso->reserva_pc);
    encerrarJogo(&caso->nucleo);
}

int comecarCaso(Caso *caso) {
    if (caso == NULL) {
        return 0;
    }

    soltarOponente(caso);
    liberarListaTime(&caso->nucleo.time);
    liberarListaTime(&caso->reserva_pc);
    liberarFilaTreinadores(&caso->nucleo.fila);
    liberarPilhaLog(&caso->nucleo.log);
    liberarListaSalas(&caso->salas);
    criarListaTime(&caso->nucleo.time);
    criarListaTime(&caso->reserva_pc);
    criarFilaTreinadores(&caso->nucleo.fila);
    criarPilhaLog(&caso->nucleo.log);
    criarListaSalas(&caso->salas);
    caso->caso_encerrado = 0;
    caso->captura_disponivel = 0;
    caso->pocoes = 2;
    caso->super_pocoes = 1;
    caso->pokebolas = 3;
    caso->tentativas_fuga = 0;
    caso->treino_ativo = 0;
    caso->encontros_treino = 0;
    caso->batalha_npc = 0;
    caso->captura_acertos = 0;
    caso->captura_erros = 0;
    caso->area_secreta = 0;
    caso->selos_secretos = 0;
    caso->master_bolas = 0;
    caso->chave_secreta = 0;
    caso->altares_secretos = 0;
    memset(caso->pokedex_vistos, 0, sizeof caso->pokedex_vistos);
    caso->itens_mapa = 0;
    caso->semente_sorteio = (unsigned int)time(NULL) ^ (unsigned int)clock();
    if (caso->semente_sorteio == 0U) caso->semente_sorteio = 0x51A7U;

    if (!colocarEspecie(&caso->nucleo.time, &caso->nucleo.pokedex, "Pikachu", 26)) {
        return 0;
    }
    caso->pokedex_vistos[25] = 1;
    obterAtivo(&caso->nucleo.time)->qtd_golpes = 2;
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
    NoSala *sala;
    int estava_resolvido;
    int resultado;
    if (caso == NULL || caso->em_batalha) {
        return 0;
    }
    sala = salaAtual(&caso->salas);
    if (sala == NULL || (sala->inicia_batalha && !sala->batalha_concluida)) {
        return 0;
    }
    estava_resolvido = sala->resolvido;
    resultado = resolverPuzzleDaSala(&caso->salas, &caso->nucleo.time, &caso->nucleo.log);
    if (resultado && !estava_resolvido && strcmp(sala->nome, "Jaula") == 0) {
        empilharLog(&caso->nucleo.log, "Documento: Giovanni após o Arquivo; mova a estante com a mente. Geodude: use água.");
    }
    return resultado;
}

int casoComandarPuzzle(Caso *caso, AcaoDesafio acao, int alvo) {
    NoSala *sala;
    int resultado;
    if (caso == NULL || caso->em_batalha || caso->captura_disponivel ||
        caso->salas.atual == NULL) return -1;
    sala = caso->salas.atual;
    if ((sala->inicia_batalha && !sala->batalha_concluida) ||
        !timeTemHabilidade(&caso->nucleo.time, sala->habilidade)) return -1;
    resultado = comandarDesafio(&sala->desafio, acao, alvo);
    if (resultado == 2 && !casoUsarHabilidade(caso)) return -1;
    return resultado;
}

int casoAcenderPonto(Caso *caso, int ponto) {
    NoSala *sala;
    if (caso == NULL || caso->em_batalha || caso->captura_disponivel ||
        ponto < 0 || ponto > 1 || caso->salas.inicio == NULL ||
        !caso->salas.inicio->resolvido || !timeTemHabilidade(&caso->nucleo.time, HAB_ELETRICA)) return 0;
    sala = caso->salas.atual;
    if (sala == NULL) return 0;
    if (sala->luzes & (1U << ponto)) return 1;
    sala->luzes |= 1U << ponto;
    empilharLog(&caso->nucleo.log, ponto == 0 ? "Pikachu religou o refletor oeste." : "Pikachu religou o refletor leste.");
    return 1;
}

int casoAbrirAreaSecreta(Caso *caso) {
    NoSala *sala;
    if (caso == NULL || caso->em_batalha || caso->captura_disponivel) return 0;
    sala = salaAtual(&caso->salas);
    if (sala == NULL || strcmp(sala->nome, "Arquivo") != 0 || !sala->resolvido) return 0;
    if (!caso->chave_secreta) {
        empilharLog(&caso->nucleo.log, "A porta lateral está trancada. Encontre a Chave do Santuário no Arquivo.");
        return 0;
    }
    caso->area_secreta = 1;
    empilharLog(&caso->nucleo.log, "Passagem lateral destrancada com a Chave do Santuário!");
    return 1;
}

void casoSairAreaSecreta(Caso *caso) {
    if (caso != NULL) caso->area_secreta = 0;
}

int casoObterChaveSecreta(Caso *caso) {
    if (caso == NULL) return 0;
    if (caso->chave_secreta) return 1;
    caso->chave_secreta = 1;
    empilharLog(&caso->nucleo.log, "Você encontrou a Chave do Santuário nos arquivos!");
    return 1;
}

int casoTemChaveSecreta(const Caso *caso) {
    return caso != NULL && caso->chave_secreta;
}

int casoAtivarAltarElemental(Caso *caso, int altar) {
    Habilidade hab_necessaria;
    const Pokemon *ativo;
    if (caso == NULL || !caso->area_secreta || altar < 0 || altar > 2) return 0;
    if (caso->altares_secretos & (1 << altar)) return 0;

    if (altar == 0) hab_necessaria = HAB_FOGO;
    else if (altar == 1) hab_necessaria = HAB_PSIQUICA;
    else hab_necessaria = HAB_ELETRICA;

    ativo = obterAtivo(&caso->nucleo.time);
    if (ativo == NULL || (habilidadeDoPokemon(ativo) != hab_necessaria && !timeTemHabilidade(&caso->nucleo.time, hab_necessaria))) {
        return -1;
    }

    caso->altares_secretos |= (1 << altar);
    caso->selos_secretos++;

    if (altar == 0) empilharLog(&caso->nucleo.log, "Altar do Fogo ativado com chamas.");
    else if (altar == 1) empilharLog(&caso->nucleo.log, "Altar Psíquico ativado com telecinese.");
    else empilharLog(&caso->nucleo.log, "Altar do Trovão ativado com eletricidade.");

    if (caso->altares_secretos == 7) {
        caso->selos_secretos = 3;
        caso->master_bolas = 1;
        empilharLog(&caso->nucleo.log, "Puzzle resolvido: Master Ball encontrada no pedestal sagrado!");
        return 2;
    }
    return 1;
}

int casoAtivarSeloSecreto(Caso *caso, int selo) {
    return casoAtivarAltarElemental(caso, selo);
}

const char *casoObjetivoAtual(const Caso *caso) {
    const NoSala *sala = caso == NULL ? NULL : caso->salas.atual;
    if (sala == NULL) return "Abra um novo caso.";
    if (caso->captura_disponivel) return "Finalize a captura.";
    if (caso->area_secreta && caso->altares_secretos != 7 && caso->selos_secretos < 3) return "Santuário: ative os 3 altares elementais.";
    if (caso->area_secreta) return "Mata secreta: procure encontros raros.";
    if (sala->inicia_batalha && !sala->batalha_concluida) return sala->e_chefe ? "Enfrente Giovanni." : "Encontre o selvagem.";
    if (sala->luzes_necessarias && sala->luzes != sala->luzes_necessarias &&
        caso->salas.inicio->resolvido) return "Religue os 2 refletores.";
    if (strcmp(sala->nome, "Arquivo") == 0 && sala->resolvido && !caso->chave_secreta) return "Procure a Chave do Santuário.";
    if (!sala->resolvido) {
        switch (sala->desafio.tipo) {
        case DES_CIRCUITO: return "Reconecte o gerador.";
        case DES_GELO: return "Derreta as 4 travas.";
        case DES_AGUA: return "Regule A=4 e B=3.";
        case DES_RASTRO: return "Siga os ecos das pistas.";
        case DES_ESTANTES: return "Alinhe as duas estantes.";
        default: return "Investigue o setor.";
        }
    }
    if (sala->duelo_obrigatorio && !sala->duelo_concluido) return "Vença o guarda Rocket.";
    return "Siga à próxima porta.";
}

static void marcarOponentesVistos(Caso *caso) {
    if (caso != NULL && caso->oponente_carregado) {
        const NoTime *no;
        for (no = caso->oponente_atual.time.inicio; no != NULL; no = no->prox) {
            casoMarcarVisto(caso, no->pokemon.numero);
        }
    }
}

int casoIniciarDuelo(Caso *caso) {
    NoSala *sala = caso == NULL ? NULL : caso->salas.atual;
    if (sala == NULL || caso->em_batalha || caso->captura_disponivel || caso->treino_ativo ||
        !sala->duelo_obrigatorio || sala->duelo_concluido || !sala->resolvido ||
        (sala->luzes & sala->luzes_necessarias) != sala->luzes_necessarias) return 0;
    if (!caso->oponente_carregado) {
        if (!proximoTreinador(&caso->nucleo.fila, &caso->oponente_atual)) return 0;
        caso->oponente_carregado = 1;
    }
    marcarOponentesVistos(caso);
    caso->batalha_npc = 1;
    caso->em_batalha = 1;
    caso->tentativas_fuga = 0;
    if (obterAtivo(&caso->nucleo.time)->hp_atual <= 0) avancarParaProximoVivo(&caso->nucleo.time);
    empilharLog(&caso->nucleo.log, "O guarda Rocket bloqueou a passagem.");
    return 1;
}

const char *casoPaginaDocumento(const Caso *caso, int pagina) {
    const NoSala *sala;
    if (caso == NULL || pagina < 0 || pagina > 1) return NULL;
    for (sala = caso->salas.inicio; sala != NULL; sala = sala->prox) {
        if (strcmp(sala->nome, "Jaula") == 0 && sala->resolvido) {
            if (pagina == 0) return "RELATÓRIO 07. Mewtwo saiu antes do apagão. Os guardas protegem Giovanni. No Arquivo, empurre as estantes para as marcas douradas. Use U para desfazer e R para reiniciar se ficar preso.";
            return "Geodude ignora golpes elétricos: use água. Há dois refletores em cada setor escuro. No fogo, reserve 4 unidades no tanque A e 3 no B. Capturas exigem lançar e conter: siga as teclas destacadas.";
        }
    }
    return NULL;
}

static int temEspecie(const ListaTime *time, const char *nome) {
    const NoTime *no;
    for (no = time->inicio; no != NULL; no = no->prox) {
        if (strcmp(no->pokemon.nome, nome) == 0) return 1;
    }
    return 0;
}

/* Reserva espaço para os parceiros do caso ou envia para a reserva do PC. */
static int podeCapturarTreino(const Caso *caso, const Pokemon *pokemon) {
    if (caso == NULL || pokemon == NULL) return 0;
    if (temEspecie(&caso->nucleo.time, pokemon->nome) || temEspecie(&caso->reserva_pc, pokemon->nome)) {
        return 0;
    }
    return 1;
}

int casoIniciarTreino(Caso *caso) {
    NoSala *sala;
    int nivel;
    if (caso == NULL || caso->em_batalha || caso->oponente_carregado ||
        caso->captura_disponivel || caso->caso_encerrado ||
        timeFoiDerrotado(&caso->nucleo.time)) return 0;
    sala = caso->salas.atual;
    if (sala == NULL || (sala->inicia_batalha && !sala->batalha_concluida)) return 0;
    if (strcmp(sala->nome, "Pátio") == 0) {
        nivel = 8;
    } else if (strcmp(sala->nome, "Tanque") == 0) {
        nivel = 9;
    } else if (strcmp(sala->nome, "Oficina") == 0 || strcmp(sala->nome, "Jaula") == 0) {
        nivel = 8;
    } else if (strcmp(sala->nome, "Arquivo") == 0 && caso->area_secreta) {
        nivel = 15;
    } else return 0;
    {
        unsigned int sorte;
        int disponiveis[151];
        int quantidade = 0;
        int numero;
        int raro = 0;
        Pokemon preparado;
        const Pokemon *modelo;
        caso->semente_sorteio = caso->semente_sorteio * 1664525U + 1013904223U;
        sorte = caso->semente_sorteio >> 16;
        for (numero = 1; numero <= 151; numero++) {
            int lendario = (numero >= 144 && numero <= 146) || numero == 150 || numero == 151;
            if (!lendario && buscarPorNumero(&caso->nucleo.arvore, numero) != NULL) {
                disponiveis[quantidade++] = numero;
            }
        }
        if (quantidade == 0) return 0;
        numero = disponiveis[sorte % (unsigned int)quantidade];

        /* No Santuário Secreto, lendários aparecem com alta frequência: 50% ou 75% com altares ativos! */
        if (caso->area_secreta) {
            unsigned int chance_lendario = (caso->altares_secretos == 7) ? 75U : 50U;
            if ((sorte % 100U) < chance_lendario) {
                unsigned int rola = (sorte / 100U) % 5U;
                if (rola == 0) numero = 144;      /* Articuno */
                else if (rola == 1) numero = 145; /* Zapdos */
                else if (rola == 2) numero = 146; /* Moltres */
                else if (rola == 3) numero = 151; /* Mew */
                else numero = 150;                /* Mewtwo */
                raro = 1;
            }
        }
        criarTreinador(&caso->oponente_atual, "Pokémon dos arredores",
                       raro ? "Uma presença lendária surgiu!" :
                       "Um selvagem apareceu! Vença para ganhar 30 XP de treino.");
        modelo = buscarPorNumero(&caso->nucleo.arvore, numero);
        if (modelo == NULL) {
            liberarListaTime(&caso->oponente_atual.time);
            return 0;
        }
        preparado = pokemonPreparar(modelo, raro ? (numero == 150 ? 18 : numero == 151 ? 15 : 16) : nivel);
        if (!adicionarPokemon(&caso->oponente_atual.time, &preparado)) {
            liberarListaTime(&caso->oponente_atual.time);
            return 0;
        }
        /* Um segundo sorteio independente marca a variante rara para a apresentação. */
        caso->semente_sorteio = caso->semente_sorteio * 1664525U + 1013904223U;
        obterAtivo(&caso->oponente_atual.time)->brilhante = ((caso->semente_sorteio >> 16) % 128U == 1U);
    }
    caso->oponente_carregado = 1;
    marcarOponentesVistos(caso);
    caso->treino_ativo = 1;
    caso->em_batalha = 1;
    caso->tentativas_fuga = 0;
    caso->encontros_treino++;
    if (obterAtivo(&caso->nucleo.time)->hp_atual <= 0) avancarParaProximoVivo(&caso->nucleo.time);
    empilharLog(&caso->nucleo.log, "Encontro opcional: treino nos arredores.");
    return 1;
}

int casoDescansar(Caso *caso) {
    if (caso == NULL || caso->em_batalha || caso->oponente_carregado ||
        caso->captura_disponivel) return 0;
    restaurarTime(&caso->nucleo.time);
    empilharLog(&caso->nucleo.log, "Pausa no banco: time recuperado.");
    return 1;
}

int casoAvancar(Caso *caso) {
    if (caso == NULL || caso->em_batalha || caso->captura_disponivel) {
        return 0;
    }
    return avancarSala(&caso->salas, &caso->nucleo.log);
}

int casoVoltar(Caso *caso) {
    if (caso == NULL || caso->em_batalha || caso->captura_disponivel) {
        return 0;
    }
    return voltarSala(&caso->salas, &caso->nucleo.log);
}

int casoIniciarBatalha(Caso *caso) {
    NoSala *sala;

    if (caso == NULL || caso->caso_encerrado || caso->em_batalha || caso->treino_ativo ||
        caso->captura_disponivel) {
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
    marcarOponentesVistos(caso);
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
    caso->captura_acertos = 0;
    caso->captura_erros = 0;
    if (caso->batalha_npc) {
        NoTime *no;
        sala->duelo_concluido = 1;
        for (no = caso->nucleo.time.inicio; no != NULL; no = no->prox) {
            if (strcmp(no->pokemon.nome, "Pikachu") == 0 && no->pokemon.qtd_golpes < MAX_GOLPES) {
                no->pokemon.qtd_golpes++;
                snprintf(msg, sizeof msg, "Pikachu aprendeu %s!", no->pokemon.golpes[no->pokemon.qtd_golpes - 1].nome);
                registrarMensagem(saida, msg);
            }
        }
        caso->pocoes += 2;
        caso->pokebolas += 2;
        restaurarTime(&caso->nucleo.time);
        registrarMensagem(saida, "Guarda vencido! Time recuperado. Recebeu 2 Poções e 2 Poké Bolas.");
        soltarOponente(caso);
        caso->batalha_npc = 0;
        return 0;
    }
    if (caso->treino_ativo) {
        NoTime *no;
        registrarMensagem(saida, "Treino vencido! Cada parceiro recebeu 30 XP.");
        for (no = caso->nucleo.time.inicio; no != NULL; no = no->prox) {
            Pokemon *parceiro = &no->pokemon;
            if (parceiro->nivel >= 60) continue;
            parceiro->experiencia += 30;
            if (parceiro->experiencia >= 60) {
                int hp_antigo = parceiro->hp_max;
                parceiro->experiencia -= 60;
                parceiro->nivel++;
                parceiro->hp_max = calcularHpMax(parceiro->hp_base, parceiro->nivel);
                if (parceiro->hp_atual > 0) parceiro->hp_atual += parceiro->hp_max - hp_antigo;
                snprintf(msg, sizeof msg, "%s subiu para o nível %d!", parceiro->nome, parceiro->nivel);
                registrarMensagem(saida, msg);
            }
        }
        if (oponente != NULL && podeCapturarTreino(caso, oponente)) {
            caso->captura_pendente = *oponente;
            caso->captura_pendente.hp_atual = oponente->hp_max;
            caso->captura_disponivel = 1;
        } else {
            registrarMensagem(saida, "Selvagem liberado: espécie já no time ou vagas reservadas aos parceiros do caso.");
        }
        soltarOponente(caso);
        caso->treino_ativo = 0;
        return 0;
    }
    if (sala != NULL) {
        sala->batalha_concluida = 1;
        if (sala->resolvido && sala->prox != NULL) {
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
    case ITEM_MASTER_BOLA:
        return "Master Ball";
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
    case ITEM_MASTER_BOLA:
        return caso->master_bolas;
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
        if (sala == NULL || caso->batalha_npc || (!caso->treino_ativo && (!sala->capturavel || sala->e_chefe))) {
            registrarMensagem(saida, "Não dá para capturar este Pokémon.");
            return -2;
        }
        if (caso->treino_ativo && oponente != NULL && !podeCapturarTreino(caso, oponente)) {
            registrarMensagem(saida, "Já tem esta espécie ou precisa reservar vagas aos parceiros do caso.");
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
    } else if (item == ITEM_MASTER_BOLA) {
        Pokemon *oponente = obterAtivo(&caso->oponente_atual.time);
        char mensagem[LOG_MAX];
        if (caso->master_bolas <= 0 || !caso->treino_ativo || oponente == NULL ||
            ((oponente->numero < 144 || oponente->numero > 146) &&
             oponente->numero != 150 && oponente->numero != 151) || !podeCapturarTreino(caso, oponente)) {
            registrarMensagem(saida, "A Master Ball fica reservada para um lendário capturável.");
            return -2;
        }
        oponente->hp_atual = oponente->hp_max;
        oponente->brilhante = obterAtivo(&caso->oponente_atual.time)->brilhante;
        if (!adicionarPokemon(&caso->nucleo.time, oponente)) {
            if (!adicionarPokemon(&caso->reserva_pc, oponente)) return -2;
            snprintf(mensagem, sizeof mensagem, "Master Ball! %s capturado e enviado ao PC!", oponente->nome);
        } else {
            snprintf(mensagem, sizeof mensagem, "Master Ball! %s foi capturado!", oponente->nome);
        }
        casoMarcarVisto(caso, oponente->numero);
        caso->master_bolas--;
        caso->em_batalha = 0;
        caso->treino_ativo = 0;
        registrarMensagem(saida, mensagem);
        empilharLog(&caso->nucleo.log, "Master Ball: Pokémon lendário capturado sem falha.");
        soltarOponente(caso);
        return 0;
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
    if (caso->batalha_npc || (!caso->treino_ativo && sala != NULL && sala->e_chefe)) {
        registrarMensagem(saida, caso->batalha_npc ? "O guarda bloqueia a fuga!" : "Não dá pra fugir de um chefe!");
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
        if (caso->treino_ativo) {
            soltarOponente(caso);
            caso->treino_ativo = 0;
        }
        return 3;
    }

    registrarMensagem(saida, "Não conseguiu fugir!");
    return turnoSoDoOponente(caso, saida);
}

int casoComandoCaptura(const Caso *caso) {
    static const int sequencia[5] = {4, 0, 1, 3, 4};
    int comando;
    if (caso == NULL || !caso->captura_disponivel || caso->captura_acertos >= 5) return -1;
    comando = sequencia[caso->captura_acertos];
    return comando == 4 ? 4 : (comando + caso->captura_pendente.numero) % 4;
}

int casoPrazoCaptura(const Caso *caso) {
    if (caso == NULL || !caso->captura_disponivel) return 0;
    return 2300 - caso->captura_acertos * 180;
}

int casoResponderCaptura(Caso *caso, int comando, int tempo_ms) {
    int esperado = casoComandoCaptura(caso);
    if (esperado < 0 || tempo_ms < 0 || comando < 0 || comando > 4) return -1;
    if (comando != esperado || (caso->captura_acertos > 0 && tempo_ms > casoPrazoCaptura(caso))) {
        caso->captura_acertos = 0;
        caso->captura_erros++;
        return 0;
    }
    caso->captura_acertos++;
    return caso->captura_acertos == 5 ? 2 : 1;
}

int casoCapturar(Caso *caso) {
    char msg[LOG_MAX];

    if (caso == NULL || !caso->captura_disponivel || caso->captura_acertos != 5) {
        return 0;
    }
    if (!adicionarPokemon(&caso->nucleo.time, &caso->captura_pendente)) {
        if (!adicionarPokemon(&caso->reserva_pc, &caso->captura_pendente)) {
            return 0;
        }
        snprintf(msg, sizeof msg, "%s enviado para o PC (equipe cheia)!", caso->captura_pendente.nome);
        empilharLog(&caso->nucleo.log, msg);
        casoMarcarVisto(caso, caso->captura_pendente.numero);
        caso->captura_disponivel = 0;
        return 1;
    }

    snprintf(msg, sizeof msg, "%s entrou no time.", caso->captura_pendente.nome);
    empilharLog(&caso->nucleo.log, msg);
    casoMarcarVisto(caso, caso->captura_pendente.numero);
    caso->captura_disponivel = 0;
    return 1;
}

void casoMarcarVisto(Caso *caso, int numero) {
    if (caso != NULL && numero > 0 && numero < 152) {
        caso->pokedex_vistos[numero] = 1;
    }
}

int casoFoiVisto(const Caso *caso, int numero) {
    if (caso == NULL || numero <= 0 || numero >= 152) return 0;
    return caso->pokedex_vistos[numero] != 0;
}

int casoItemJaColetado(const Caso *caso, int id_item) {
    if (caso == NULL || id_item < 0 || id_item >= 32) return 1;
    return (caso->itens_mapa & (1U << id_item)) != 0;
}

int casoColetarItemMapa(Caso *caso, int id_item, char *mensagem, size_t tamanho) {
    Pokemon *ativo;
    if (caso == NULL || id_item < 0 || id_item >= 32) return 0;
    if (caso->itens_mapa & (1U << id_item)) return 0;

    caso->itens_mapa |= (1U << id_item);
    ativo = obterAtivo(&caso->nucleo.time);

    switch (id_item) {
        case 0: /* Laboratório: Rare Candy */
        case 2: /* Câmara fria: Rare Candy */
        case 4: /* Oficina: Rare Candy */
        case 6: /* Arquivo: Rare Candy */
        case 7: /* Santuário Secreto: Rare Candy Místico */
            if (ativo != NULL && ativo->nivel < 60) {
                int hp_ant = ativo->hp_max;
                ativo->nivel++;
                ativo->hp_max = calcularHpMax(ativo->hp_base, ativo->nivel);
                if (ativo->hp_atual > 0) ativo->hp_atual += (ativo->hp_max - hp_ant);
                ativo->experiencia = 0;
                if (id_item == 7) ativo->hp_atual = ativo->hp_max;
                if (mensagem != NULL && tamanho > 0) {
                    snprintf(mensagem, tamanho, "Você achou um RARE CANDY! %s subiu para o Nível %d!",
                             ativo->nome, ativo->nivel);
                }
            } else {
                caso->super_pocoes += 2;
                if (mensagem != NULL && tamanho > 0) {
                    snprintf(mensagem, tamanho, "Você achou 2 Super Poções no compartimento!");
                }
            }
            break;
        case 1: /* Pátio: Super Poção */
            caso->super_pocoes += 2;
            if (mensagem != NULL && tamanho > 0) {
                snprintf(mensagem, tamanho, "Você achou 2 Super Poções de primeiros socorros!");
            }
            break;
        case 3: /* Tanque: 3 Poké Bolas */
            caso->pokebolas += 3;
            if (mensagem != NULL && tamanho > 0) {
                snprintf(mensagem, tamanho, "Você achou 3 Poké Bolas no compartimento aquático!");
            }
            break;
        case 5: /* Jaula: 2 Poções e 1 Poké Bola */
            caso->pocoes += 2;
            caso->pokebolas += 1;
            if (mensagem != NULL && tamanho > 0) {
                snprintf(mensagem, tamanho, "Você achou suprimentos na jaula: 2 Poções e 1 Poké Bola!");
            }
            break;
        default:
            caso->pocoes += 1;
            if (mensagem != NULL && tamanho > 0) {
                snprintf(mensagem, tamanho, "Você achou 1 Poção útil!");
            }
            break;
    }
    return 1;
}

int casoDepositarPokemon(Caso *caso, int pos_time) {
    const Pokemon *p;
    Pokemon copia;
    if (caso == NULL || caso->em_batalha) return 0;
    if (tamanhoTime(&caso->nucleo.time) <= 1) return 0;
    p = pokemonNaPosicao(&caso->nucleo.time, pos_time);
    if (p == NULL) return 0;
    copia = *p;
    if (!adicionarPokemon(&caso->reserva_pc, &copia)) return 0;
    removerPokemonPorPosicao(&caso->nucleo.time, pos_time);
    if (obterAtivo(&caso->nucleo.time) == NULL) {
        definirAtivoPorPosicao(&caso->nucleo.time, 0);
    }
    return 1;
}

int casoRetirarPokemon(Caso *caso, int pos_reserva) {
    const Pokemon *p;
    Pokemon copia;
    if (caso == NULL || caso->em_batalha) return 0;
    if (tamanhoTime(&caso->nucleo.time) >= MAX_TIME) return 0;
    p = pokemonNaPosicao(&caso->reserva_pc, pos_reserva);
    if (p == NULL) return 0;
    copia = *p;
    if (!adicionarPokemon(&caso->nucleo.time, &copia)) return 0;
    removerPokemonPorPosicao(&caso->reserva_pc, pos_reserva);
    return 1;
}

int casoTrocarOrdemTime(Caso *caso, int i, int j) {
    if (caso == NULL) return 0;
    return trocarPokemon(&caso->nucleo.time, i, j);
}

int casoDefinirLider(Caso *caso, int pos) {
    if (caso == NULL || pos < 0 || pos >= tamanhoTime(&caso->nucleo.time)) return 0;
    if (pos != 0) {
        trocarPokemon(&caso->nucleo.time, 0, pos);
    }
    definirAtivoPorPosicao(&caso->nucleo.time, 0);
    return 1;
}

int casoTamanhoReserva(const Caso *caso) {
    if (caso == NULL) return 0;
    return tamanhoTime(&caso->reserva_pc);
}

const Pokemon *casoPokemonReserva(const Caso *caso, int pos) {
    if (caso == NULL) return NULL;
    return pokemonNaPosicao(&caso->reserva_pc, pos);
}

int casoConvidarMewtwo(Caso *caso) {
    const Pokemon *modelo;
    Pokemon mewtwo;
    char msg[LOG_MAX];

    if (caso == NULL) return 0;
    modelo = buscarPorNumero(&caso->nucleo.arvore, 150);
    if (modelo == NULL) return 0;
    mewtwo = pokemonPreparar(modelo, 50);
    if (!adicionarPokemon(&caso->nucleo.time, &mewtwo)) {
        if (!adicionarPokemon(&caso->reserva_pc, &mewtwo)) {
            return 0;
        }
        snprintf(msg, sizeof msg, "Mewtwo foi enviado para a reserva do PC!");
        empilharLog(&caso->nucleo.log, msg);
    } else {
        snprintf(msg, sizeof msg, "Mewtwo entrou na equipe do detetive!");
        empilharLog(&caso->nucleo.log, msg);
    }
    casoMarcarVisto(caso, 150);
    return 1;
}

int casoIniciarEncontroLendario(Caso *caso, int numero_lendario) {
    const Pokemon *modelo;
    Pokemon preparado;
    int nivel;
    char frase[LOG_MAX];

    if (caso == NULL || caso->em_batalha || caso->oponente_carregado ||
        caso->captura_disponivel || caso->caso_encerrado ||
        timeFoiDerrotado(&caso->nucleo.time)) {
        return 0;
    }
    modelo = buscarPorNumero(&caso->nucleo.arvore, numero_lendario);
    if (modelo == NULL) return 0;

    nivel = (numero_lendario == 150 ? 18 : numero_lendario == 151 ? 15 : 16);
    snprintf(frase, sizeof frase, "O lendário %s despertou de seu repouso sagrado!", modelo->nome);
    criarTreinador(&caso->oponente_atual, "Presença Ancestral", frase);
    preparado = pokemonPreparar(modelo, nivel);
    if (!adicionarPokemon(&caso->oponente_atual.time, &preparado)) {
        liberarListaTime(&caso->oponente_atual.time);
        return 0;
    }

    caso->semente_sorteio = caso->semente_sorteio * 1664525U + 1013904223U;
    obterAtivo(&caso->oponente_atual.time)->brilhante = ((caso->semente_sorteio >> 16) % 128U == 1U);

    caso->oponente_carregado = 1;
    marcarOponentesVistos(caso);
    caso->treino_ativo = 1;
    caso->em_batalha = 1;
    caso->tentativas_fuga = 0;
    if (obterAtivo(&caso->nucleo.time)->hp_atual <= 0) {
        avancarParaProximoVivo(&caso->nucleo.time);
    }
    empilharLog(&caso->nucleo.log, frase);
    return 1;
}
