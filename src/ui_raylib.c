/*
 * ui_raylib.c
 * -----------
 * Janela do caso: menu, uma sala com puzzle, uma batalha com barras
 * de HP e a tela de vitória ou derrota. Formas e texto, sem sprite.
 * A regra fica em caso.c. Ver ui_raylib.h.
 */

#include "ui_raylib.h"

#include "caso.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "raylib.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#define LARGURA 1024
#define ALTURA 576

typedef enum {
    TELA_MENU = 0,
    TELA_AJUDA,
    TELA_POKEDEX,
    TELA_SALA,
    TELA_BATALHA,
    TELA_FIM
} Tela;

static Font fonte;
static int fonte_ok = 0;

static const Color COR_FUNDO = {16, 20, 34, 255};
static const Color COR_PAINEL = {28, 36, 58, 255};
static const Color COR_BOTAO = {46, 72, 120, 255};
static const Color COR_BOTAO_ON = {70, 110, 170, 255};
static const Color COR_DESTAQUE = {255, 196, 60, 255};
static const Color COR_TEXTO = {236, 234, 226, 255};
static const Color COR_HP = {72, 190, 96, 255};
static const Color COR_HP_FUNDO = {48, 28, 32, 255};
static const Color COR_PERIGO = {186, 72, 72, 255};

static void carregarFonte(void) {
    static const char *caminhos[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        NULL
    };
    int pontos[224];
    int n = 0;
    int c;
    int i;

    for (c = 32; c <= 255; c++) {
        pontos[n] = c;
        n++;
    }

    fonte_ok = 0;
    for (i = 0; caminhos[i] != NULL; i++) {
        if (!FileExists(caminhos[i])) {
            continue;
        }
        fonte = LoadFontEx(caminhos[i], 32, pontos, n);
        if (fonte.glyphCount > 0) {
            SetTextureFilter(fonte.texture, TEXTURE_FILTER_BILINEAR);
            fonte_ok = 1;
            return;
        }
    }
}

static void desenhar(const char *texto, float x, float y, float tamanho, Color cor) {
    if (texto == NULL) {
        return;
    }
    if (fonte_ok) {
        DrawTextEx(fonte, texto, (Vector2){x, y}, tamanho, 1.0f, cor);
    } else {
        DrawText(texto, (int)x, (int)y, (int)tamanho, cor);
    }
}

static void desenharQuebra(const char *texto, float x, float y, float tamanho,
                           float largura, Color cor) {
    const char *p;
    float yy;

    if (texto == NULL) {
        return;
    }

    p = texto;
    yy = y;
    while (*p != '\0' && yy < ALTURA - 20) {
        char buf[TEXTO_SALA_MAX];
        int i = 0;
        int ultimo = -1;
        float w = 0.0f;

        while (p[i] != '\0' && i < (int)sizeof buf - 1) {
            buf[i] = p[i];
            buf[i + 1] = '\0';
            if (fonte_ok) {
                w = MeasureTextEx(fonte, buf, tamanho, 1.0f).x;
            } else {
                w = (float)MeasureText(buf, (int)tamanho);
            }
            if (w > largura) {
                buf[i] = '\0';
                break;
            }
            if (p[i] == ' ') {
                ultimo = i;
            }
            i++;
        }

        if (p[i] != '\0' && ultimo > 0) {
            buf[ultimo] = '\0';
            p += (size_t)ultimo + 1;
        } else if (i == 0) {
            break;
        } else {
            p += (size_t)i;
        }

        desenhar(buf, x, yy, tamanho, cor);
        yy += tamanho + 6.0f;
    }
}

static int botao(const char *rotulo, Rectangle ret, Color fundo) {
    Vector2 mouse = GetMousePosition();
    int sobre = CheckCollisionPointRec(mouse, ret);
    Vector2 tam;
    float tx;
    float ty;

    DrawRectangleRec(ret, sobre ? COR_BOTAO_ON : fundo);
    DrawRectangleLinesEx(ret, 2.0f, sobre ? COR_DESTAQUE : COR_PAINEL);

    if (fonte_ok) {
        tam = MeasureTextEx(fonte, rotulo, 22.0f, 1.0f);
    } else {
        tam.x = (float)MeasureText(rotulo, 22);
        tam.y = 22.0f;
    }
    tx = ret.x + (ret.width - tam.x) / 2.0f;
    ty = ret.y + (ret.height - tam.y) / 2.0f;
    desenhar(rotulo, tx, ty, 22.0f, COR_TEXTO);
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void barraHp(int x, int y, int largura, int atual, int maximo) {
    float prop;

    if (maximo < 1) {
        maximo = 1;
    }
    if (atual < 0) {
        atual = 0;
    }
    if (atual > maximo) {
        atual = maximo;
    }
    prop = (float)atual / (float)maximo;
    DrawRectangle(x, y, largura, 22, COR_HP_FUNDO);
    DrawRectangle(x, y, (int)(largura * prop), 22, COR_HP);
}

typedef struct {
    char linhas[8][LOG_MAX];
    int n;
} PainelCombate;

static void narrarCombate(void *contexto, const char *mensagem) {
    PainelCombate *painel = (PainelCombate *)contexto;
    int i;

    if (painel == NULL || mensagem == NULL) {
        return;
    }
    if (painel->n >= 8) {
        /* Copia para um buffer temporário: as linhas se sobrepõem. */
        for (i = 1; i < 8; i++) {
            char temporario[LOG_MAX];
            copiarTexto(temporario, sizeof temporario, painel->linhas[i]);
            copiarTexto(painel->linhas[i - 1], LOG_MAX, temporario);
        }
        painel->n = 7;
    }
    snprintf(painel->linhas[painel->n], LOG_MAX, "%s", mensagem);
    painel->n++;
}

typedef struct {
    char linhas[24][80];
    int n;
    float y;
} ListaDex;

static void visitarDex(const Pokemon *pokemon, void *contexto) {
    ListaDex *lista = (ListaDex *)contexto;

    if (lista == NULL || pokemon == NULL || lista->n >= 24) {
        return;
    }
    snprintf(lista->linhas[lista->n], sizeof lista->linhas[0], "#%03d  %s",
             pokemon->numero, pokemon->nome);
    lista->n++;
}

static void desenharMenu(void) {
    desenhar("PokéBattle", 80, 70, 54, COR_DESTAQUE);
    desenhar("O Caso do Laboratório", 84, 136, 28, COR_TEXTO);
    desenhar("Um detetive. Um Pikachu. Um laboratório trancado.", 84, 180, 20, COR_TEXTO);
}

static void desenharSala(Caso *caso) {
    NoSala *sala = salaAtual(&caso->salas);
    const Pokemon *parceiro = pokemonNaPosicao(&caso->nucleo.time, 0);
    char topo[LOG_MAX];

    DrawRectangle(40, 30, 944, 150, COR_PAINEL);
    if (sala == NULL) {
        desenhar("Nenhuma sala carregada.", 60, 50, 24, COR_TEXTO);
        return;
    }

    desenhar(sala->nome, 60, 46, 32, COR_DESTAQUE);
    desenharQuebra(sala->descricao, 60, 92, 20, 880, COR_TEXTO);

    DrawRectangle(40, 200, 944, 120, COR_PAINEL);
    if (sala->resolvido) {
        desenharQuebra(sala->texto_resolvido, 60, 220, 20, 880, COR_TEXTO);
    } else {
        desenharQuebra(sala->texto_puzzle, 60, 220, 20, 880, COR_TEXTO);
    }

    if (parceiro != NULL) {
        desenhar(parceiro->nome, 60, 350, 22, COR_DESTAQUE);
        barraHp(60, 384, 280, parceiro->hp_atual, parceiro->hp_max);
    }

    if (topoDoLog(&caso->nucleo.log, topo, sizeof topo)) {
        desenharQuebra(topo, 380, 350, 18, 580, COR_TEXTO);
    }
}

static void desenharBatalha(Caso *caso, const PainelCombate *painel) {
    const Pokemon *eu = obterAtivo(&caso->nucleo.time);
    const Pokemon *opp = caso->oponente_carregado ? obterAtivo(&caso->oponente_atual.time) : NULL;
    int i;

    desenhar(caso->oponente_atual.nome, 60, 36, 28, COR_PERIGO);
    if (opp != NULL) {
        desenhar(opp->nome, 60, 78, 22, COR_TEXTO);
        barraHp(60, 112, 400, opp->hp_atual, opp->hp_max);
    }

    if (eu != NULL) {
        desenhar(eu->nome, 560, 78, 22, COR_DESTAQUE);
        barraHp(560, 112, 400, eu->hp_atual, eu->hp_max);
    }

    DrawRectangle(40, 160, 944, 200, COR_PAINEL);
    for (i = 0; i < painel->n; i++) {
        desenhar(painel->linhas[i], 56, 172 + (float)i * 22, 18, COR_TEXTO);
    }
}

int executarInterfaceRaylib(void) {
    Caso caso;
    Tela tela = TELA_MENU;
    PainelCombate painel;
    ListaDex dex;
    int resultado = 0;
    int vivo = 1;

    if (!iniciarCaso(&caso)) {
        fprintf(stderr, "Não foi possível carregar a Pokédex.\n");
        return 1;
    }

    memset(&painel, 0, sizeof painel);
    memset(&dex, 0, sizeof dex);

    InitWindow(LARGURA, ALTURA, "PokéBattle: O Caso do Laboratório");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    carregarFonte();

    while (vivo && !WindowShouldClose()) {
        NoSala *sala;
        int i;

        if (IsKeyPressed(KEY_ESCAPE) && tela != TELA_MENU) {
            tela = TELA_MENU;
        }

        BeginDrawing();
        ClearBackground(COR_FUNDO);

        if (tela == TELA_MENU) {
            desenharMenu();
            if (botao("Novo jogo", (Rectangle){84, 250, 320, 52}, COR_BOTAO) ||
                IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
                if (comecarCaso(&caso)) {
                    memset(&painel, 0, sizeof painel);
                    resultado = 0;
                    tela = TELA_SALA;
                }
            }
            if (botao("Como jogar", (Rectangle){84, 316, 320, 52}, COR_BOTAO) ||
                IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
                tela = TELA_AJUDA;
            }
            if (botao("Pokédex", (Rectangle){84, 382, 320, 52}, COR_BOTAO) ||
                IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
                dex.n = 0;
                percorrerEmOrdem(&caso.nucleo.arvore, visitarDex, &dex);
                tela = TELA_POKEDEX;
            }
            if (botao("Sair", (Rectangle){84, 448, 320, 52}, COR_PERIGO) ||
                IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
                vivo = 0;
            }
        } else if (tela == TELA_AJUDA) {
            desenhar("Como jogar", 60, 40, 36, COR_DESTAQUE);
            desenharQuebra("Você é o detetive. O Pikachu é o parceiro, não o protagonista. "
                           "Na sala, use a habilidade dele para resolver o puzzle. "
                           "Depois avance e lute. Na batalha, escolha um golpe: "
                           "quem age primeiro sai da velocidade e da prioridade.",
                           60, 100, 22, 900, COR_TEXTO);
            desenhar("Teclas: 1 novo jogo, 2 ajuda, 3 Pokédex, 4 sair, Esc volta.",
                     60, 280, 20, COR_TEXTO);
            desenhar("Na sala: E habilidade, D avançar, A voltar, B lutar.",
                     60, 314, 20, COR_TEXTO);
            desenhar("Na luta: teclas 1 a 4 escolhem o golpe.",
                     60, 348, 20, COR_TEXTO);
            if (botao("Voltar", (Rectangle){60, 470, 220, 52}, COR_BOTAO)) {
                tela = TELA_MENU;
            }
        } else if (tela == TELA_POKEDEX) {
            desenhar("Pokédex", 60, 36, 36, COR_DESTAQUE);
            for (i = 0; i < dex.n; i++) {
                int coluna = i / 10;
                int linha = i % 10;
                desenhar(dex.linhas[i], 60 + coluna * 320, 100 + linha * 36, 22, COR_TEXTO);
            }
            if (botao("Voltar", (Rectangle){780, 500, 180, 48}, COR_BOTAO)) {
                tela = TELA_MENU;
            }
        } else if (tela == TELA_SALA) {
            desenharSala(&caso);
            sala = salaAtual(&caso.salas);
            if (sala != NULL && !sala->resolvido) {
                if (botao("Usar Choque do Pikachu", (Rectangle){60, 450, 360, 52}, COR_BOTAO) ||
                    IsKeyPressed(KEY_E)) {
                    casoUsarHabilidade(&caso);
                }
            }
            if (sala != NULL && sala->prox != NULL && sala->resolvido && sala->prox->desbloqueada) {
                if (botao("Avançar", (Rectangle){440, 450, 160, 52}, COR_BOTAO) ||
                    IsKeyPressed(KEY_D)) {
                    casoAvancar(&caso);
                }
            }
            if (sala != NULL && sala->ant != NULL) {
                if (botao("Voltar", (Rectangle){620, 450, 160, 52}, COR_BOTAO) ||
                    IsKeyPressed(KEY_A)) {
                    casoVoltar(&caso);
                }
            }
            if (sala != NULL && sala->inicia_batalha && !caso.batalha_vencida) {
                if (botao("Enfrentar", (Rectangle){800, 450, 180, 52}, COR_PERIGO) ||
                    IsKeyPressed(KEY_B)) {
                    if (casoIniciarBatalha(&caso)) {
                        memset(&painel, 0, sizeof painel);
                        tela = TELA_BATALHA;
                    }
                }
            }
            if (caso.batalha_vencida && sala != NULL && sala->inicia_batalha) {
                if (botao("Concluir o caso", (Rectangle){800, 450, 180, 52}, COR_DESTAQUE)) {
                    resultado = 1;
                    tela = TELA_FIM;
                }
            }
            if (botao("Menu", (Rectangle){60, 516, 120, 40}, COR_PAINEL)) {
                tela = TELA_MENU;
            }
        } else if (tela == TELA_BATALHA) {
            const Pokemon *eu = obterAtivo(&caso.nucleo.time);
            int golpe = -1;

            desenharBatalha(&caso, &painel);
            if (eu != NULL) {
                for (i = 0; i < eu->qtd_golpes; i++) {
                    Rectangle ret = {60, 390 + (float)i * 44, 420, 40};
                    char rotulo[80];
                    snprintf(rotulo, sizeof rotulo, "%d  %s", i + 1, eu->golpes[i].nome);
                    if (botao(rotulo, ret, COR_BOTAO)) {
                        golpe = i;
                    }
                    if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i)) {
                        golpe = i;
                    }
                }
            }
            if (golpe >= 0) {
                SaidaBatalha saida;
                int codigo;

                saida.narrar = narrarCombate;
                saida.contexto = &painel;
                saida.log = &caso.nucleo.log;
                codigo = casoExecutarGolpe(&caso, golpe, &saida);
                if (codigo == 0) {
                    resultado = 1;
                    tela = TELA_FIM;
                } else if (codigo < 0 && codigo != -2) {
                    resultado = -1;
                    tela = TELA_FIM;
                }
            }
        } else if (tela == TELA_FIM) {
            if (resultado > 0) {
                desenhar("Caso encerrado", 80, 80, 48, COR_DESTAQUE);
                desenharQuebra("O capanga saiu do caminho. O laboratório ainda guarda segredos, mas esta passagem é sua.",
                               80, 170, 24, 860, COR_TEXTO);
            } else {
                desenhar("Derrota", 80, 80, 48, COR_PERIGO);
                desenhar("O Pikachu desmaiou. Dá para tentar a luta de novo.", 80, 170, 24, COR_TEXTO);
                if (botao("Tentar de novo", (Rectangle){80, 260, 280, 52}, COR_BOTAO)) {
                    if (casoIniciarBatalha(&caso)) {
                        memset(&painel, 0, sizeof painel);
                        tela = TELA_BATALHA;
                    }
                }
            }
            if (botao("Menu", (Rectangle){80, 360, 220, 52}, COR_BOTAO)) {
                tela = TELA_MENU;
            }
        }

        EndDrawing();
    }

    if (fonte_ok) {
        UnloadFont(fonte);
        fonte_ok = 0;
    }
    CloseWindow();
    encerrarCaso(&caso);
    return 0;
}
