/*
 * ui_raylib.c
 * -----------
 * Só desenho. A regra continua em caso.c, sala.c e batalha.c.
 *
 * A cena nasce numa textura de 384×216 e sobe para a janela em escala
 * inteira, com filtro de ponto, para o pixel não borrar. A fonte é a
 * Press Start 2P (OFL), em assets/fontes/. Os Pokémon são grades de
 * pixel feitas aqui, não sprites oficiais.
 *
 * O que a leitura visual pediu e esta tela faz: barra de HP que desce
 * e troca de cor, caixa de texto letra por letra, golpes em 2×2,
 * plataformas, entrada deslizando, laboratório escuro que acende,
 * caderno de pistas, e a luta do Giovanni em vermelho com tremor.
 */

#include "ui_raylib.h"

#include "caso.h"
#include "util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "raylib.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#define VIRT_L 384
#define VIRT_A 216
#define ESCALA 3
#define JANELA_L (VIRT_L * ESCALA)
#define JANELA_A (VIRT_A * ESCALA)
#define FONTE_PX 8
#define MAX_PART 40
#define MAX_FILA_TEXTO 8

typedef enum {
    TELA_MENU = 0,
    TELA_AJUDA,
    TELA_POKEDEX,
    TELA_SALA,
    TELA_BATALHA,
    TELA_FIM
} Tela;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float vida;
    Color cor;
} Particula;

typedef struct {
    float tempo;
    float fade;
    float tremor;
    float entrada;
    float hp_eu;
    float hp_opp;
    int hp_pronto;
    float acum_letra;
    int bytes_texto;
    char texto[TEXTO_SALA_MAX];
    char fila[MAX_FILA_TEXTO][LOG_MAX];
    int fila_n;
    int caderno;
    int ultimo_resolvido;
    int texto_completo;
    Particula partes[MAX_PART];
    int som_ok;
    Sound som_clique;
    Sound som_acerto;
} Visual;

static Font fonte;
static int fonte_ok = 0;
static RenderTexture2D alvo;
static int grafico_ok = 0;
static Visual vis;

static const Color CREME = {244, 244, 236, 255};
static const Color TINTA = {28, 32, 48, 255};
static const Color NOITE = {8, 10, 20, 255};
static const Color AZUL = {36, 72, 168, 255};
static const Color CIANO = {72, 220, 230, 255};
static const Color ROXO = {150, 90, 220, 255};
static const Color DOURADO = {255, 204, 72, 255};
static const Color VERDE = {72, 200, 88, 255};
static const Color AMARELO = {248, 208, 64, 255};
static const Color VERMELHO = {220, 64, 56, 255};

static const char *PIKACHU[16] = {
    ".....kk....kk...",
    ".....ky....yk...",
    ".....yy....yy...",
    "....yyyyyyyy....",
    "...yykkkyyykk...",
    "...yyyyyyyyyyy..",
    "...yyyrryyrryy..",
    "....yyyyyyyyyy..",
    "....yyybyybbyy..",
    ".....yyyyyyyy...",
    "......yyyyyy....",
    ".....yy.yy.yy...",
    "....yy..yy..yy..",
    "................",
    "................",
    "................"
};

static const char *CHARMANDER[16] = {
    "................",
    ".......oo.......",
    "......oooo......",
    ".....ooccooo....",
    ".....ooooooo....",
    "....oooooooo....",
    "....ooocccco....",
    ".....oooooo.....",
    "......oooo......",
    ".......oooo.....",
    "......oo.fff....",
    ".....oo..faf....",
    "....oo...ff.....",
    "................",
    "................",
    "................"
};

static const char *SQUIRTLE[16] = {
    "................",
    "......BBBB......",
    ".....BBBBBB.....",
    "....BBCCCBBB....",
    "....BBCCCCBB....",
    "...BBBCCCCBBB...",
    "...BBBCCBBBBB...",
    "....BBBCCBBB....",
    ".....BBBBBB.....",
    "......BBBB......",
    ".....BB..BB.....",
    "....BB....BB....",
    "................",
    "................",
    "................",
    "................"
};

static const char *ALAKAZAM[16] = {
    ".....yy..yy.....",
    "....yyyyyyyy....",
    "...yyykkkyyy....",
    "...yyyyyyyyyy...",
    "..yyymmmmmmyy...",
    "...yyyyyyyyyy...",
    "....yyy..yyy....",
    ".....yy..yy.....",
    "....sss..sss....",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................"
};

static const char *MEOWTH[16] = {
    "....cc....cc....",
    "...ccc....ccc...",
    "..cccccccccccc..",
    "..ccckkcccckkc..",
    "..cccccccccccc..",
    "...cccggggccc...",
    "....cccccccc....",
    ".....cccccc.....",
    "......cggc......",
    ".....cc..cc.....",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................"
};

static const char *GEODUDE[16] = {
    "................",
    ".....GGGGGG.....",
    "....GGGGGGGG....",
    "...GGkGGGGkGG...",
    "...GGGGGGGGGGG..",
    "..GGGGGGGGGGGG..",
    "..GGGhhGGhhGGG..",
    "...GGGGGGGGGG...",
    "....GGGGGGGG....",
    "...GG......GG...",
    "..GG........GG..",
    "................",
    "................",
    "................",
    "................",
    "................"
};

static Color corPixel(char c, int flash) {
    if (c == '.' || c == '\0') {
        return (Color){0, 0, 0, 0};
    }
    if (flash) {
        return (Color){255, 255, 255, 255};
    }
    switch (c) {
    case 'y': return (Color){255, 214, 64, 255};
    case 'k': return (Color){24, 24, 32, 255};
    case 'r': return (Color){220, 48, 48, 255};
    case 'b': return (Color){120, 72, 32, 255};
    case 'o': return (Color){240, 112, 40, 255};
    case 'c': return (Color){255, 236, 200, 255};
    case 'C': return (Color){255, 236, 200, 255};
    case 'f': return (Color){255, 80, 24, 255};
    case 'a': return (Color){255, 220, 80, 255};
    case 'B': return (Color){64, 140, 220, 255};
    case 'm': return (Color){160, 96, 48, 255};
    case 's': return (Color){196, 204, 214, 255};
    case 'g': return (Color){248, 208, 72, 255};
    case 'G': return (Color){120, 124, 132, 255};
    case 'h': return (Color){72, 76, 84, 255};
    case 'H': return (Color){92, 52, 28, 255};
    case 'w': return (Color){150, 156, 170, 255};
    case 'W': return (Color){210, 214, 224, 255};
    case 'n': return (Color){36, 64, 120, 255};
    case 'N': return (Color){18, 28, 52, 255};
    case 'p': return (Color){240, 200, 156, 255};
    default: return (Color){200, 200, 210, 255};
    }
}

static const char **gradeDoNome(const char *nome) {
    if (nome == NULL) {
        return PIKACHU;
    }
    if (strcmp(nome, "Pikachu") == 0) return PIKACHU;
    if (strcmp(nome, "Charmander") == 0) return CHARMANDER;
    if (strcmp(nome, "Squirtle") == 0) return SQUIRTLE;
    if (strcmp(nome, "Alakazam") == 0) return ALAKAZAM;
    if (strcmp(nome, "Meowth") == 0) return MEOWTH;
    if (strcmp(nome, "Geodude") == 0) return GEODUDE;
    return PIKACHU;
}

static const char *PIKACHU_COSTAS[16] = {
    ".....kk....kk...",
    ".....yy....yy...",
    ".....yy....yy...",
    "....yyyyyyyy....",
    "...yyyyyyyyyy...",
    "...yyybbyybbyy..",
    "...yyyyyyyyyyy..",
    "....yyybbyyy....",
    ".....yyyyyy.....",
    "......yyyy......",
    ".....yy..yy.....",
    "....yaa....y....",
    "...yaaa.........",
    "..yaa...........",
    ".ya.............",
    "................"
};

static const char *CHAR_COSTAS[16] = {
    "................",
    "......oooo......",
    ".....oooooo.....",
    "....oooooooo....",
    "....ooobbooo....",
    "....oooooooo....",
    ".....oooooo.....",
    "......oooo......",
    ".......oo.......",
    "......oo.ff.....",
    ".....oo..faf....",
    "....oo....ff....",
    "................",
    "................",
    "................",
    "................"
};

static const char *SQUIRT_COSTAS[16] = {
    "................",
    "......BBBB......",
    ".....BBBBBB.....",
    "....BBBBBBBB....",
    "...BBBbbbbBBB...",
    "...BBBbbbbBBB...",
    "....BBbbbbBB....",
    ".....BBBBBB.....",
    "......BBBB......",
    ".....BB..BB.....",
    "....BB....BB....",
    "................",
    "................",
    "................",
    "................",
    "................"
};

static const char *DETETIVE[16] = {
    "......HH......",
    ".....HHHH.....",
    ".....HppH.....",
    "......pp......",
    ".....nnnn.....",
    "....nnnnnn....",
    "...nnnnnnnn...",
    "...nn.nn.nn...",
    "....nnnnnn....",
    ".....n..n.....",
    ".....N..N.....",
    "....NN..NN....",
    "................",
    "................",
    "................",
    "................"
};

static const char *GIOVANNI[16] = {
    "......kk......",
    ".....kkkk.....",
    ".....kppk.....",
    "......pp......",
    ".....wrrw.....",
    "....wwwwww....",
    "...WwwwwwwW...",
    "...Ww.ww.wW...",
    "....wwwwww....",
    ".....w..w.....",
    ".....W..W.....",
    "....WW..WW....",
    "................",
    "................",
    "................",
    "................"
};

static int celulaCheia(const char **grade, int linha, int coluna) {
    const char *texto;

    if (linha < 0 || coluna < 0 || linha >= 16) {
        return 0;
    }
    texto = grade[linha];
    if (texto == NULL || coluna >= (int)strlen(texto)) {
        return 0;
    }
    return texto[coluna] != '.' && texto[coluna] != '\0';
}

/* escala 1 = 16 px; 3 = 48 px; 4 = 64 px, já com contorno de um pixel da arte. */
static void desenharGrade(const char **grade, int x, int y, int escala, int flash) {
    int linha;
    int coluna;
    int k;
    static const int dxs[4] = {-1, 1, 0, 0};
    static const int dys[4] = {0, 0, -1, 1};

    if (grade == NULL) {
        return;
    }
    if (escala < 1) {
        escala = 1;
    }
    for (linha = 0; linha < 16; linha++) {
        for (coluna = 0; coluna < 16; coluna++) {
            if (!celulaCheia(grade, linha, coluna)) {
                continue;
            }
            for (k = 0; k < 4; k++) {
                if (!celulaCheia(grade, linha + dys[k], coluna + dxs[k])) {
                    DrawRectangle(x + (coluna + dxs[k]) * escala,
                                  y + (linha + dys[k]) * escala,
                                  escala, escala, (Color){10, 10, 14, 255});
                }
            }
        }
    }
    for (linha = 0; linha < 16; linha++) {
        const char *texto = grade[linha];
        for (coluna = 0; texto[coluna] != '\0' && coluna < 16; coluna++) {
            Color cor = corPixel(texto[coluna], flash);
            if (cor.a == 0) {
                continue;
            }
            DrawRectangle(x + coluna * escala, y + linha * escala, escala, escala, cor);
        }
    }
}

static int bytesDoCaractere(const unsigned char *s, int i) {
    if (s[i] == 0) {
        return 0;
    }
    if ((s[i] & 0x80) == 0) {
        return 1;
    }
    if ((s[i] & 0xE0) == 0xC0 && s[i + 1] != 0) {
        return 2;
    }
    if ((s[i] & 0xF0) == 0xE0 && s[i + 1] != 0 && s[i + 2] != 0) {
        return 3;
    }
    return 1;
}

static void carregarFonte(void) {
    static const char *caminhos[] = {
        "assets/fontes/PressStart2P-Regular.ttf",
        "../assets/fontes/PressStart2P-Regular.ttf",
        "/workspace/assets/fontes/PressStart2P-Regular.ttf",
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
        fonte = LoadFontEx(caminhos[i], FONTE_PX, pontos, n);
        if (fonte.glyphCount > 0) {
            SetTextureFilter(fonte.texture, TEXTURE_FILTER_POINT);
            fonte_ok = 1;
            return;
        }
    }
}

static Wave ondaQuadrada(float frequencia, float duracao) {
    unsigned int taxa = 22050;
    unsigned int total = (unsigned int)(taxa * duracao);
    unsigned int i;
    short *amostras;
    Wave onda;

    amostras = (short *)calloc(total, sizeof(short));
    if (amostras != NULL) {
        for (i = 0; i < total; i++) {
            float t = (float)i / (float)taxa;
            float envelope = 1.0f - ((float)i / (float)total);
            int alto = ((int)(t * frequencia)) % 2;
            float valor = (alto ? 1.0f : -1.0f) * envelope * 9000.0f;
            amostras[i] = (short)valor;
        }
    }
    onda.frameCount = total;
    onda.sampleRate = taxa;
    onda.sampleSize = 16;
    onda.channels = 1;
    onda.data = amostras;
    return onda;
}

static void prepararSom(void) {
    Wave clique;
    Wave acerto;

    vis.som_ok = 0;
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        return;
    }
    clique = ondaQuadrada(880.0f, 0.04f);
    acerto = ondaQuadrada(220.0f, 0.08f);
    vis.som_clique = LoadSoundFromWave(clique);
    vis.som_acerto = LoadSoundFromWave(acerto);
    UnloadWave(clique);
    UnloadWave(acerto);
    vis.som_ok = 1;
}

static void tocar(Sound som) {
    if (vis.som_ok) {
        PlaySound(som);
    }
}

static void soltarSom(void) {
    if (!vis.som_ok) {
        return;
    }
    UnloadSound(vis.som_clique);
    UnloadSound(vis.som_acerto);
    CloseAudioDevice();
    vis.som_ok = 0;
}

static Vector2 mouseVirtual(void) {
    return (Vector2){
        (float)GetMouseX() / (float)ESCALA,
        (float)GetMouseY() / (float)ESCALA
    };
}

static void texto(const char *s, int x, int y, Color cor) {
    if (s == NULL) {
        return;
    }
    if (fonte_ok) {
        DrawTextEx(fonte, s, (Vector2){(float)x, (float)y}, (float)FONTE_PX, 1.0f, cor);
    } else {
        DrawText(s, x, y, FONTE_PX, cor);
    }
}

static int larguraTexto(const char *s) {
    if (s == NULL || s[0] == '\0') {
        return 0;
    }
    if (fonte_ok) {
        return (int)MeasureTextEx(fonte, s, (float)FONTE_PX, 1.0f).x;
    }
    return MeasureText(s, FONTE_PX);
}

static void textoQuebra(const char *origem, int x, int y, int largura, int linhas_max, Color cor) {
    const char *p = origem;
    int linha = 0;

    if (origem == NULL) {
        return;
    }
    while (*p != '\0' && linha < linhas_max && y < VIRT_A) {
        char buf[TEXTO_SALA_MAX];
        int i = 0;
        int ultimo = -1;

        while (p[i] != '\0' && i < (int)sizeof buf - 1) {
            buf[i] = p[i];
            buf[i + 1] = '\0';
            if (larguraTexto(buf) > largura) {
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
        texto(buf, x, y, cor);
        y += FONTE_PX + 3;
        linha++;
    }
}

static void painel(int x, int y, int w, int h, Color miolo) {
    DrawRectangle(x, y, w, h, (Color){8, 8, 12, 255});
    DrawRectangle(x + 1, y + 1, w - 2, h - 2, CREME);
    DrawRectangle(x + 2, y + 2, w - 4, h - 4, miolo);
}

static int botao(const char *rotulo, int x, int y, int w, int h, Color fundo) {
    Vector2 mouse = mouseVirtual();
    Rectangle ret = {(float)x, (float)y, (float)w, (float)h};
    int sobre = CheckCollisionPointRec(mouse, ret);
    int tx;

    painel(x, y, w, h, sobre ? AZUL : fundo);
    tx = x + (w - larguraTexto(rotulo)) / 2;
    if (tx < x + 3) {
        tx = x + 3;
    }
    texto(rotulo, tx, y + (h - FONTE_PX) / 2, sobre ? DOURADO : CREME);
    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static Color corDaBarra(float proporcao) {
    if (proporcao > 0.50f) {
        return VERDE;
    }
    if (proporcao > 0.20f) {
        return AMARELO;
    }
    return VERMELHO;
}

static void aproximar(float *visivel, float alvo, float dt) {
    float delta = alvo - *visivel;
    float passo = dt * 3.2f;

    if (passo > 1.0f) {
        passo = 1.0f;
    }
    if (fabsf(delta) < 0.004f) {
        *visivel = alvo;
    } else {
        *visivel += delta * passo;
    }
}

static void barraHp(int x, int y, int w, float proporcao) {
    int cheio;

    if (proporcao < 0.0f) {
        proporcao = 0.0f;
    }
    if (proporcao > 1.0f) {
        proporcao = 1.0f;
    }
    cheio = (int)((w - 4) * proporcao);
    DrawRectangle(x, y, w, 6, (Color){32, 24, 28, 255});
    if (cheio > 0) {
        DrawRectangle(x + 1, y + 1, cheio, 4, corDaBarra(proporcao));
    }
}

static void definirTexto(const char *s) {
    if (s == NULL) {
        s = "";
    }
    if (strcmp(vis.texto, s) != 0) {
        copiarTexto(vis.texto, sizeof vis.texto, s);
        vis.bytes_texto = 0;
        vis.acum_letra = 0.0f;
    }
}

static void avancarTexto(float dt) {
    int total = (int)strlen(vis.texto);

    vis.acum_letra += dt * 42.0f;
    while (vis.acum_letra >= 1.0f && vis.bytes_texto < total) {
        int n = bytesDoCaractere((const unsigned char *)vis.texto, vis.bytes_texto);
        if (n <= 0) {
            break;
        }
        vis.bytes_texto += n;
        vis.acum_letra -= 1.0f;
    }
    if (vis.bytes_texto > total) {
        vis.bytes_texto = total;
    }
}

static void desenharTextoAnimado(int x, int y, int largura, int linhas) {
    char parcial[TEXTO_SALA_MAX];
    int n = vis.bytes_texto;

    if (n >= (int)sizeof parcial) {
        n = (int)sizeof parcial - 1;
    }
    if (vis.texto_completo) {
        n = (int)strlen(vis.texto);
    }
    if (n > (int)sizeof parcial - 1) {
        n = (int)sizeof parcial - 1;
    }
    memcpy(parcial, vis.texto, (size_t)n);
    parcial[n] = '\0';
    textoQuebra(parcial, x, y, largura, linhas, TINTA);
}

static void enfileirarTexto(const char *mensagem) {
    if (mensagem == NULL || mensagem[0] == '\0') {
        return;
    }
    if (vis.fila_n >= MAX_FILA_TEXTO) {
        int i;
        for (i = 1; i < MAX_FILA_TEXTO; i++) {
            copiarTexto(vis.fila[i - 1], LOG_MAX, vis.fila[i]);
        }
        vis.fila_n = MAX_FILA_TEXTO - 1;
    }
    copiarTexto(vis.fila[vis.fila_n], LOG_MAX, mensagem);
    vis.fila_n++;
    if (vis.fila_n == 1) {
        definirTexto(vis.fila[0]);
    }
}

static void narrarCombate(void *contexto, const char *mensagem) {
    (void)contexto;
    enfileirarTexto(mensagem);
    if (mensagem != NULL && strstr(mensagem, "Causou") != NULL) {
        vis.tremor = 2.2f;
        tocar(vis.som_acerto);
    }
}

static void consumirFilaSePronta(float dt) {
    (void)dt;
    if (vis.fila_n <= 0) {
        return;
    }
    definirTexto(vis.fila[0]);
    if (vis.bytes_texto >= (int)strlen(vis.texto) && vis.fila_n > 1 && vis.acum_letra > 8.0f) {
        int i;
        for (i = 1; i < vis.fila_n; i++) {
            copiarTexto(vis.fila[i - 1], LOG_MAX, vis.fila[i]);
        }
        vis.fila_n--;
        definirTexto(vis.fila[0]);
        vis.acum_letra = 0.0f;
    }
}

static void soltarParticulas(int x, int y, Habilidade hab) {
    Color cor = CIANO;
    int i;
    int n = 0;

    if (hab == HAB_FOGO) {
        cor = (Color){255, 120, 32, 255};
    } else if (hab == HAB_AGUA) {
        cor = (Color){64, 160, 255, 255};
    } else if (hab == HAB_PSIQUICA) {
        cor = ROXO;
    } else if (hab == HAB_ELETRICA) {
        cor = DOURADO;
    }
    for (i = 0; i < MAX_PART && n < 16; i++) {
        if (vis.partes[i].vida > 0.0f) {
            continue;
        }
        vis.partes[i].x = (float)x;
        vis.partes[i].y = (float)y;
        vis.partes[i].vx = (float)((n % 5) - 2) * 18.0f;
        vis.partes[i].vy = (float)(-30 - (n % 4) * 8);
        vis.partes[i].vida = 0.7f;
        vis.partes[i].cor = cor;
        n++;
    }
    tocar(vis.som_clique);
}

static void atualizarParticulas(float dt) {
    int i;

    for (i = 0; i < MAX_PART; i++) {
        if (vis.partes[i].vida <= 0.0f) {
            continue;
        }
        vis.partes[i].vida -= dt;
        vis.partes[i].x += vis.partes[i].vx * dt;
        vis.partes[i].y += vis.partes[i].vy * dt;
        vis.partes[i].vy += 40.0f * dt;
    }
}

static void desenharParticulas(void) {
    int i;

    for (i = 0; i < MAX_PART; i++) {
        if (vis.partes[i].vida <= 0.0f) {
            continue;
        }
        DrawRectangle((int)vis.partes[i].x, (int)vis.partes[i].y, 2, 2, vis.partes[i].cor);
    }
}

static const char **gradeCostas(const char *nome) {
    if (nome != NULL && strcmp(nome, "Charmander") == 0) {
        return CHAR_COSTAS;
    }
    if (nome != NULL && strcmp(nome, "Squirtle") == 0) {
        return SQUIRT_COSTAS;
    }
    if (nome != NULL && strcmp(nome, "Alakazam") == 0) {
        return ALAKAZAM;
    }
    return PIKACHU_COSTAS;
}

/* escala 3 desenha 48 px; escala 4 desenha 64 px, antes do contorno. */
static void desenharMon(const char *nome, int x, int y, int escala, int costas, int flash) {
    int bob = (escala > 1) ? (int)sinf(vis.tempo * 4.0f) : 0;
    const char **grade = costas ? gradeCostas(nome) : gradeDoNome(nome);
    desenharGrade(grade, x, y + bob, escala, flash);
}

static void desenharPessoa(const char **grade, int x, int y, int escala) {
    int bob = (int)sinf(vis.tempo * 3.0f);
    desenharGrade(grade, x, y + bob, escala, 0);
}

static void plataforma(int x, int y) {
    DrawEllipse(x, y, 28, 8, (Color){20, 24, 32, 255});
    DrawEllipse(x, y - 1, 22, 5, (Color){48, 56, 72, 255});
}

static void vinheta(int forca) {
    Color borda = {0, 0, 0, (unsigned char)forca};
    DrawRectangle(0, 0, VIRT_L, 18, borda);
    DrawRectangle(0, VIRT_A - 24, VIRT_L, 24, borda);
    DrawRectangle(0, 0, 16, VIRT_A, borda);
    DrawRectangle(VIRT_L - 16, 0, 16, VIRT_A, borda);
}

static void piso(int y0, int y1, Color a, Color b) {
    int x;
    int y;

    for (y = y0; y < y1; y += 8) {
        for (x = 0; x < VIRT_L; x += 8) {
            DrawRectangle(x, y, 8, 8, (((x + y) / 8) % 2) == 0 ? a : b);
        }
    }
}

static void tubo(int x, int y, int h, Color liquido, int aceso) {
    Color vidro = aceso ? (Color){170, 210, 220, 255} : (Color){50, 64, 78, 255};
    Color dentro = aceso ? liquido : (Color){liquido.r / 3, liquido.g / 3, liquido.b / 3, 255};

    DrawRectangle(x, y, 12, h, (Color){16, 18, 24, 255});
    DrawRectangle(x + 1, y + 2, 10, h - 4, vidro);
    DrawRectangle(x + 3, y + 6, 6, h - 12, dentro);
    DrawRectangle(x + 2, y + 4, 2, h - 8, (Color){255, 255, 255, (unsigned char)(aceso ? 90 : 30)});
}

static void desenharCenario(const NoSala *sala) {
    int aceso;
    int pulso;
    const char *nome;
    Color parede;
    Color chao_a;
    Color chao_b;

    nome = (sala != NULL) ? sala->nome : "";
    aceso = (sala != NULL && sala->resolvido) ? 1 : 0;
    pulso = 160 + (int)(sinf(vis.tempo * 5.0f) * 40.0f);
    parede = (Color){18, 24, 40, 255};
    chao_a = (Color){32, 36, 48, 255};
    chao_b = (Color){24, 28, 38, 255};

    if (strcmp(nome, "Câmara fria") == 0) {
        parede = (Color){150, 190, 210, 255};
        chao_a = (Color){190, 220, 230, 255};
        chao_b = (Color){160, 200, 214, 255};
    } else if (strcmp(nome, "Oficina") == 0) {
        parede = (Color){48, 22, 18, 255};
        chao_a = (Color){60, 32, 24, 255};
        chao_b = (Color){42, 22, 18, 255};
    } else if (strcmp(nome, "Sala do chefe") == 0) {
        parede = (Color){36, 8, 12, 255};
        chao_a = (Color){48, 12, 16, 255};
        chao_b = (Color){28, 8, 10, 255};
    } else if (strcmp(nome, "Pátio") == 0) {
        parede = (Color){18, 40, 28, 255};
        chao_a = (Color){36, 90, 48, 255};
        chao_b = (Color){28, 70, 38, 255};
    }

    DrawRectangle(0, 0, VIRT_L, 150, parede);
    piso(96, 150, chao_a, chao_b);
    DrawRectangle(0, 92, VIRT_L, 4, (Color){12, 12, 16, 255});

    if (strcmp(nome, "Laboratório") == 0 || nome[0] == '\0') {
        tubo(12, 28, 62, (Color){80, 220, 210, 255}, aceso);
        tubo(30, 36, 54, ROXO, aceso);
        tubo(330, 24, 66, CIANO, aceso);
        tubo(348, 40, 50, (Color){80, 140, 255, 255}, aceso);
        DrawRectangle(120, 78, 140, 16, (Color){40, 44, 56, 255});
        DrawRectangle(132, 62, 28, 16, (Color){20, 24, 32, 255});
        DrawRectangle(134, 64, 24, 10, aceso ? (Color){80, 220, 120, 255} : (Color){20, 50, 30, 255});
        DrawRectangle(168, 64, 22, 14, (Color){20, 24, 32, 255});
        DrawRectangle(170, 66, 18, 8, aceso ? CIANO : (Color){30, 40, 50, 255});
        DrawLine(150, 78, 150, 96, (Color){80, 70, 40, 255});
        DrawLine(190, 78, 210, 96, (Color){80, 70, 40, 255});
        DrawRectangle(210, 48, 36, 28, aceso ? (Color){90, 230, 210, 255} : (Color){36, 48, 58, 255});
        if (!aceso && ((int)(vis.tempo * 3.0f) % 2) == 0) {
            DrawRectangleLines(208, 46, 40, 32, DOURADO);
        }
    } else if (strcmp(nome, "Câmara fria") == 0) {
        DrawRectangle(150, 40, 90, 52, (Color){200, 236, 245, 255});
        DrawRectangle(156, 46, 78, 40, (Color){150, 210, 230, 255});
        if (!aceso && ((int)(vis.tempo * 3.0f) % 2) == 0) {
            DrawRectangleLines(148, 38, 94, 56, DOURADO);
        }
    } else if (strcmp(nome, "Oficina") == 0) {
        DrawRectangle(140, 70, 100, 22, (Color){50, 36, 28, 255});
        if (!aceso) {
            DrawRectangle(168, 36, 14, 34, (Color){255, 90, 24, 255});
            DrawRectangle(182, 24, 12, 46, (Color){255, 170, 40, (unsigned char)pulso});
            DrawRectangle(156, 44, 10, 26, (Color){255, 60, 20, 255});
            DrawRectangleLines(150, 20, 52, 54, DOURADO);
        }
    } else if (strcmp(nome, "Tanque") == 0) {
        DrawRectangle(130, 36, 130, 56, (Color){20, 50, 80, 255});
        DrawRectangle(136, 42, 118, 44, (Color){40, 130, 200, 255});
        DrawRectangle(140, 46, 30, 8, (Color){180, 230, 255, 180});
    } else if (strcmp(nome, "Jaula") == 0) {
        DrawRectangle(140, 28, 100, 64, (Color){30, 28, 40, 255});
        DrawRectangle(148, 28, 3, 64, CREME);
        DrawRectangle(166, 28, 3, 64, CREME);
        DrawRectangle(184, 28, 3, 64, CREME);
        DrawRectangle(202, 28, 3, 64, CREME);
        DrawRectangle(220, 28, 3, 64, CREME);
    } else if (strcmp(nome, "Arquivo") == 0) {
        DrawRectangle(236, 30, 18, 62, (Color){110, 72, 40, 255});
        DrawRectangle(256, 30, 18, 62, (Color){90, 58, 32, 255});
        DrawRectangle(276, 30, 18, 62, (Color){120, 80, 44, 255});
        DrawRectangle(150, 36, 70, 56, aceso ? (Color){70, 50, 36, 255} : (Color){130, 90, 48, 255});
        if (!aceso && ((int)(vis.tempo * 3.0f) % 2) == 0) {
            DrawRectangleLines(148, 34, 74, 60, DOURADO);
        }
    } else if (strcmp(nome, "Sala do chefe") == 0) {
        DrawRectangle(160, 0, 70, 96, (Color){90, 12, 18, 110});
        DrawRectangle(40, 30, 50, 60, (Color){20, 8, 10, 255});
        DrawRectangle(44, 36, 18, 12, (Color){120, 20, 24, 255});
    } else {
        DrawRectangle(20, 60, 30, 16, (Color){30, 90, 40, 255});
        DrawRectangle(300, 58, 36, 18, (Color){24, 80, 36, 255});
    }
}

static void desenharEquipe(Caso *caso) {
    int n = tamanhoTime(&caso->nucleo.time);
    int ativo = posicaoDoAtivo(&caso->nucleo.time);
    int i;

    for (i = 0; i < n && i < 6; i++) {
        const Pokemon *membro = pokemonNaPosicao(&caso->nucleo.time, i);
        int x = 168 + i * 18;
        if (membro == NULL) {
            continue;
        }
        if (i == ativo) {
            DrawRectangle(x - 1, 17, 18, 18, DOURADO);
        }
        desenharGrade(gradeDoNome(membro->nome), x, 2, 1, 0);
    }
}

static void desenharFade(void) {
    unsigned char alfa;

    if (vis.fade <= 0.01f) {
        return;
    }
    alfa = (unsigned char)(vis.fade * 255.0f);
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){0, 0, 0, alfa});
}

static void desenharMenu(void) {
    int i;

    for (i = 0; i < 28; i++) {
        int x = (i * 53 + (int)(vis.tempo * 12.0f)) % VIRT_L;
        int sy = (i * 37) % 90;
        DrawRectangle(x, sy, 1, 1, (Color){160, 190, 210, 255});
    }
    if (fonte_ok) {
        DrawTextEx(fonte, "PokéBattle", (Vector2){88.0f, 8.0f}, 16.0f, 1.0f, (Color){90, 40, 20, 255});
        DrawTextEx(fonte, "PokéBattle", (Vector2){86, 6}, 16.0f, 1.0f, DOURADO);
    } else {
        texto("PokéBattle", 86, 6, DOURADO);
    }
    texto("O Caso do Laboratório", 56, 28, CREME);
    DrawRectangle(0, 48, VIRT_L, 90, (Color){12, 18, 36, 255});
    DrawRectangle(40, 70, 70, 48, (Color){20, 28, 48, 255});
    DrawRectangle(48, 78, 18, 12, CIANO);
    DrawRectangle(70, 78, 18, 12, ROXO);
    DrawCircle(300, 62, 10, (Color){230, 230, 210, 255});
    plataforma(150, 132);
    plataforma(230, 132);
    desenharPessoa(DETETIVE, 120, 58, 3);
    desenharMon("Pikachu", 200, 66, 3, 1, 0);
    texto("Um detetive. Um Pikachu.", 64, 122, CIANO);
}

static void desenharSala(Caso *caso) {
    NoSala *sala = salaAtual(&caso->salas);
    const char *fala;

    painel(2, 2, 380, 14, (Color){16, 28, 58, 255});
    if (sala != NULL) {
        texto(sala->nome, 8, 5, DOURADO);
    }
    texto("N caderno", 292, 5, CIANO);
    desenharEquipe(caso);
    desenharCenario(sala);
    if (sala != NULL && strcmp(sala->nome, "Laboratório") == 0 && !sala->resolvido) {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawCircle(70, 108, 50, (Color){55, 48, 18, 255});
        DrawCircle(70, 108, 24, (Color){90, 78, 30, 255});
        EndBlendMode();
        vinheta(120);
    }
    desenharPessoa(DETETIVE, 36, 86, 3);
    desenharMon("Pikachu", 88, 98, 2, 1, 0);
    if (sala != NULL && sala->e_chefe) {
        desenharPessoa(GIOVANNI, 250, 78, 3);
        desenharMon("Meowth", 210, 90, 2, 0, 0);
    } else if (sala != NULL && strcmp(sala->nome, "Pátio") == 0 && !sala->batalha_concluida) {
        desenharMon("Charmander", 250, 90, 3, 0, 0);
    } else if (sala != NULL && strcmp(sala->nome, "Tanque") == 0 && !sala->batalha_concluida) {
        desenharMon("Squirtle", 250, 90, 3, 0, 0);
    } else if (sala != NULL && strcmp(sala->nome, "Jaula") == 0 && !sala->batalha_concluida) {
        desenharMon("Alakazam", 250, 90, 3, 0, 0);
    }
    desenharParticulas();

    painel(2, 140, 380, 36, CREME);
    if (sala == NULL) {
        fala = "Nenhuma sala.";
    } else if (!sala->resolvido) {
        fala = sala->texto_puzzle;
    } else {
        fala = sala->texto_resolvido;
    }
    definirTexto(fala);
    desenharTextoAnimado(8, 146, 364, 2);
}

static void desenharCaderno(Caso *caso) {
    NoSala *no;
    int y = 36;
    char linha[96];
    FILE *arquivo;

    painel(28, 20, 328, 170, (Color){36, 28, 20, 255});
    texto("CADERNO", 40, 28, DOURADO);
    /* Só as salas já visitadas. As de luta nascem "resolvidas" porque
     * não têm puzzle, mas ainda não são pista enquanto o detetive não chegou. */
    for (no = caso->salas.inicio; no != NULL && y < 110; no = no->prox) {
        snprintf(linha, sizeof linha, "- %s", no->nome);
        texto(linha, 40, y, CREME);
        y += 11;
        if (no == caso->salas.atual) {
            break;
        }
    }
    arquivo = tmpfile();
    if (arquivo != NULL) {
        char buffer[128];
        mostrarHistorico(&caso->nucleo.log, arquivo);
        rewind(arquivo);
        if (fgets(buffer, (int)sizeof buffer, arquivo) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            textoQuebra(buffer, 40, 128, 300, 3, (Color){220, 210, 180, 255});
        }
        fclose(arquivo);
    }
    texto("N fecha", 40, 172, CIANO);
}

static int flashDeDano(float mostrado, float real) {
    return (mostrado - real) > 0.08f && ((int)(vis.tempo * 16.0f) % 2) == 0;
}

static void desenharBatalha(Caso *caso) {
    const Pokemon *eu = obterAtivo(&caso->nucleo.time);
    const Pokemon *opp = caso->oponente_carregado ? obterAtivo(&caso->oponente_atual.time) : NULL;
    int chefe = caso->oponente_atual.nome[0] != '\0' && strcmp(caso->oponente_atual.nome, "Giovanni") == 0;
    int deslize = (int)((1.0f - vis.entrada) * 24.0f);
    float real_eu = 1.0f;
    float real_opp = 1.0f;
    char linha[64];

    if (chefe) {
        DrawRectangle(0, 0, VIRT_L, 140, (Color){32, 6, 10, 255});
        vinheta(120);
        vis.tremor = 1.3f;
    } else {
        DrawRectangle(0, 0, VIRT_L, 140, (Color){18, 36, 64, 255});
        DrawRectangle(0, 96, VIRT_L, 44, (Color){28, 64, 48, 255});
    }

    /* Jogador embaixo à esquerda, de costas. Inimigo no alto à direita.
       As caixas ficam no lado oposto de cada sprite, sem cobri-los. */
    plataforma(48 - deslize, 118);
    plataforma(300 + deslize, 72);
    if (eu != NULL) {
        real_eu = (eu->hp_max > 0) ? (float)eu->hp_atual / (float)eu->hp_max : 0.0f;
        desenharMon(eu->nome, 20 - deslize, 52, 4, 1, flashDeDano(vis.hp_eu, real_eu));
    }
    if (chefe) {
        desenharPessoa(GIOVANNI, 268 + deslize, 0, 4);
        if (opp != NULL) {
            real_opp = (opp->hp_max > 0) ? (float)opp->hp_atual / (float)opp->hp_max : 0.0f;
            desenharMon(opp->nome, 200 + deslize, 40, 2, 0, flashDeDano(vis.hp_opp, real_opp));
        }
    } else if (opp != NULL) {
        real_opp = (opp->hp_max > 0) ? (float)opp->hp_atual / (float)opp->hp_max : 0.0f;
        desenharMon(opp->nome, 292 + deslize, 8, 4, 0, flashDeDano(vis.hp_opp, real_opp));
    }

    painel(4, 4, 132, 28, (Color){232, 236, 244, 255});
    if (opp != NULL) {
        texto(opp->nome, 8, 7, TINTA);
        texto("Nv", 8, 16, (Color){40, 90, 180, 255});
        snprintf(linha, sizeof linha, "%d", opp->nivel);
        texto(linha, 26, 16, (Color){180, 40, 40, 255});
        barraHp(48, 18, 78, vis.hp_opp);
    }

    painel(168, 86, 150, 32, (Color){232, 236, 244, 255});
    if (eu != NULL) {
        texto(eu->nome, 172, 88, TINTA);
        texto("Nv", 172, 97, (Color){40, 90, 180, 255});
        snprintf(linha, sizeof linha, "%d", eu->nivel);
        texto(linha, 190, 97, (Color){180, 40, 40, 255});
        barraHp(214, 98, 96, vis.hp_eu);
        snprintf(linha, sizeof linha, "%d/%d", eu->hp_atual, eu->hp_max);
        texto(linha, 214, 106, TINTA);
    }

    painel(2, 128, 380, 28, CREME);
    if (vis.fila_n > 0) {
        desenharTextoAnimado(8, 134, 364, 2);
    } else if (eu != NULL) {
        snprintf(linha, sizeof linha, "O que %s vai fazer?", eu->nome);
        textoQuebra(linha, 8, 136, 364, 2, TINTA);
    }

    if (eu != NULL) {
        int i;
        for (i = 0; i < 4; i++) {
            int coluna = i % 2;
            int fileira = i / 2;
            int x = 4 + coluna * 190;
            int y = 160 + fileira * 26;
            if (i < eu->qtd_golpes) {
                if (botao(eu->golpes[i].nome, x, y, 186, 24, AZUL)) {
                    /* o clique é lido de novo no laço; aqui só desenha */
                }
            } else {
                painel(x, y, 186, 24, (Color){70, 74, 84, 255});
            }
        }
    }
    (void)real_eu;
    (void)real_opp;
}

static void atualizarBarras(Caso *caso, float dt) {
    const Pokemon *eu;
    const Pokemon *opp;
    float real_eu = vis.hp_eu;
    float real_opp = vis.hp_opp;

    if (!caso->em_batalha) {
        vis.hp_pronto = 0;
        return;
    }
    eu = obterAtivo(&caso->nucleo.time);
    opp = caso->oponente_carregado ? obterAtivo(&caso->oponente_atual.time) : NULL;
    if (!vis.hp_pronto) {
        vis.hp_eu = (eu != NULL && eu->hp_max > 0) ? (float)eu->hp_atual / (float)eu->hp_max : 1.0f;
        vis.hp_opp = (opp != NULL && opp->hp_max > 0) ? (float)opp->hp_atual / (float)opp->hp_max : 1.0f;
        vis.hp_pronto = 1;
        vis.entrada = 0.0f;
    }
    if (eu != NULL && eu->hp_max > 0) {
        real_eu = (float)eu->hp_atual / (float)eu->hp_max;
    }
    if (opp != NULL && opp->hp_max > 0) {
        real_opp = (float)opp->hp_atual / (float)opp->hp_max;
    }
    if (real_eu + 0.05f < vis.hp_eu || real_opp + 0.05f < vis.hp_opp) {
        vis.tremor = 2.4f;
    }
    aproximar(&vis.hp_eu, real_eu, dt);
    aproximar(&vis.hp_opp, real_opp, dt);
    if (vis.entrada < 1.0f) {
        vis.entrada += dt * 1.8f;
        if (vis.entrada > 1.0f) {
            vis.entrada = 1.0f;
        }
    }
}

static void desenharAjuda(void) {
    painel(16, 16, 352, 150, (Color){16, 24, 48, 255});
    texto("COMO JOGAR", 28, 26, DOURADO);
    textoQuebra("Você é o detetive. O Pikachu é o parceiro. Use a habilidade do tipo para abrir a sala. Capture os selvagens: cada um destrava o próximo puzzle.",
                28, 46, 328, 6, CREME);
    texto("1 menu  2 ajuda  3 pokedex  4 sair", 28, 120, CIANO);
    texto("Na luta, 1 a 4 escolhem o golpe.", 28, 136, CIANO);
}

typedef struct {
    char linhas[24][80];
    int n;
} ListaDex;

static void desenharPokedex(ListaDex *dex) {
    int i;

    texto("POKEDEX", 8, 6, DOURADO);
    for (i = 0; i < dex->n; i++) {
        int coluna = i / 10;
        int linha = i % 10;
        texto(dex->linhas[i], 8 + coluna * 128, 24 + linha * 16, CREME);
    }
}

static void desenharFim(int vitoria) {
    if (vitoria) {
        texto("CASO ENCERRADO", 70, 48, DOURADO);
        textoQuebra("Giovanni recuou. O laboratório fica para trás.", 40, 80, 300, 3, CREME);
        desenharMon("Pikachu", 160, 100, 3, 1, 0);
    } else {
        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){24, 6, 8, 255});
        texto("DERROTA", 140, 48, VERMELHO);
        textoQuebra("O time desmaiou. Dá para tentar de novo.", 40, 80, 300, 3, CREME);
    }
}

static void apresentar(void) {
    Rectangle origem = {0.0f, 0.0f, (float)VIRT_L, -(float)VIRT_A};
    int tremer_x = (int)(sinf(vis.tempo * 46.0f) * vis.tremor);
    int tremer_y = (int)(cosf(vis.tempo * 33.0f) * vis.tremor * 0.6f);
    Rectangle destino = {(float)tremer_x, (float)tremer_y, (float)JANELA_L, (float)JANELA_A};

    BeginDrawing();
    ClearBackground((Color){0, 0, 0, 255});
    DrawTexturePro(alvo.texture, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
    if (vis.tremor > 0.0f && vis.tremor < 1.2f) {
        vis.tremor *= 0.84f;
        if (vis.tremor < 0.12f) {
            vis.tremor = 0.0f;
        }
    }
}

static int abrirGrafico(void) {
    if (grafico_ok) {
        return 1;
    }
    InitWindow(JANELA_L, JANELA_A, "PokéBattle: O Caso do Laboratório");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    alvo = LoadRenderTexture(VIRT_L, VIRT_A);
    SetTextureFilter(alvo.texture, TEXTURE_FILTER_POINT);
    carregarFonte();
    prepararSom();
    grafico_ok = 1;
    return 1;
}

static void fecharGrafico(void) {
    if (!grafico_ok) {
        return;
    }
    soltarSom();
    if (fonte_ok) {
        UnloadFont(fonte);
        fonte_ok = 0;
    }
    UnloadRenderTexture(alvo);
    CloseWindow();
    grafico_ok = 0;
}

static void comecarQuadro(void) {
    BeginTextureMode(alvo);
    ClearBackground(NOITE);
}

static void fecharQuadro(void) {
    desenharFade();
    EndTextureMode();
    apresentar();
}

static void visitarDex(const Pokemon *pokemon, void *contexto) {
    ListaDex *lista = (ListaDex *)contexto;

    if (lista == NULL || pokemon == NULL || lista->n >= 24) {
        return;
    }
    snprintf(lista->linhas[lista->n], sizeof lista->linhas[0], "#%03d %s",
             pokemon->numero, pokemon->nome);
    lista->n++;
}

static int cliqueGolpe(const Pokemon *eu) {
    int i;

    if (eu == NULL) {
        return -1;
    }
    for (i = 0; i < eu->qtd_golpes && i < 4; i++) {
        int coluna = i % 2;
        int fileira = i / 2;
        int x = 4 + coluna * 190;
        int y = 160 + fileira * 26;
        Vector2 mouse = mouseVirtual();
        Rectangle ret = {(float)x, (float)y, 186.0f, 24.0f};
        if (CheckCollisionPointRec(mouse, ret) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return i;
        }
        if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i)) {
            return i;
        }
    }
    return -1;
}

static void zerarVisual(void) {
    memset(&vis, 0, sizeof vis);
    vis.fade = 1.0f;
    vis.hp_eu = 1.0f;
    vis.hp_opp = 1.0f;
}

static void atualizarRelogio(float dt) {
    vis.tempo += dt;
    if (vis.fade > 0.0f) {
        vis.fade -= dt * 1.6f;
        if (vis.fade < 0.0f) {
            vis.fade = 0.0f;
        }
    }
    avancarTexto(dt);
    atualizarParticulas(dt);
    consumirFilaSePronta(dt);
}

int executarInterfaceRaylib(void) {
    Caso caso;
    Tela tela = TELA_MENU;
    ListaDex dex;
    int resultado = 0;
    int vivo = 1;

    if (!iniciarCaso(&caso)) {
        fprintf(stderr, "Não foi possível carregar a Pokédex.\n");
        return 1;
    }
    memset(&dex, 0, sizeof dex);
    zerarVisual();
    abrirGrafico();

    while (vivo && !WindowShouldClose()) {
        float dt = GetFrameTime();
        NoSala *sala;
        int i;

        atualizarRelogio(dt);
        atualizarBarras(&caso, dt);

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (vis.caderno) {
                vis.caderno = 0;
            } else if (tela != TELA_MENU) {
                tela = TELA_MENU;
                vis.fade = 0.7f;
            }
        }
        if (tela == TELA_SALA && IsKeyPressed(KEY_N)) {
            vis.caderno = !vis.caderno;
        }

        comecarQuadro();
        if (tela == TELA_MENU) {
            desenharMenu();
            if (botao("NOVO JOGO", 112, 146, 160, 18, AZUL) || IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
                if (comecarCaso(&caso)) {
                    memset(&vis.fila, 0, sizeof vis.fila);
                    vis.fila_n = 0;
                    vis.caderno = 0;
                    resultado = 0;
                    tela = TELA_SALA;
                    vis.fade = 0.8f;
                    tocar(vis.som_clique);
                }
            }
            if (botao("AJUDA", 28, 170, 100, 18, AZUL) || IsKeyPressed(KEY_TWO)) {
                tela = TELA_AJUDA;
                vis.fade = 0.5f;
            }
            if (botao("POKEDEX", 140, 170, 100, 18, AZUL) || IsKeyPressed(KEY_THREE)) {
                dex.n = 0;
                percorrerEmOrdem(&caso.nucleo.arvore, visitarDex, &dex);
                tela = TELA_POKEDEX;
                vis.fade = 0.5f;
            }
            if (botao("SAIR", 252, 170, 100, 18, VERMELHO) || IsKeyPressed(KEY_FOUR)) {
                vivo = 0;
            }
        } else if (tela == TELA_AJUDA) {
            desenharAjuda();
            if (botao("VOLTAR", 8, 192, 72, 16, AZUL)) {
                tela = TELA_MENU;
            }
        } else if (tela == TELA_POKEDEX) {
            desenharPokedex(&dex);
            if (botao("VOLTAR", 300, 192, 72, 16, AZUL)) {
                tela = TELA_MENU;
            }
        } else if (tela == TELA_SALA) {
            sala = salaAtual(&caso.salas);
            if (sala != NULL && vis.ultimo_resolvido == 0 && sala->resolvido && sala->habilidade != HAB_NENHUMA) {
                soltarParticulas(188, 70, sala->habilidade);
            }
            vis.ultimo_resolvido = (sala != NULL && sala->resolvido) ? 1 : 0;
            desenharSala(&caso);
            if (vis.caderno) {
                desenharCaderno(&caso);
            }
            i = 4;
            if (sala != NULL && !sala->resolvido) {
                char rotulo[32];
                snprintf(rotulo, sizeof rotulo, "%s", nomeHabilidade(sala->habilidade));
                if (botao(rotulo, i, 188, 72, 16, AZUL) || IsKeyPressed(KEY_E)) {
                    casoUsarHabilidade(&caso);
                }
                i += 76;
            }
            if (caso.captura_disponivel) {
                if (botao("CAPTURAR", i, 188, 80, 16, (Color){40, 90, 48, 255}) || IsKeyPressed(KEY_C)) {
                    if (casoCapturar(&caso)) {
                        tocar(vis.som_clique);
                    }
                }
                i += 84;
            }
            if (sala != NULL && sala->prox != NULL && sala->resolvido && sala->prox->desbloqueada &&
                (!sala->inicia_batalha || sala->batalha_concluida)) {
                if (botao("AVANCAR", i, 188, 72, 16, AZUL) || IsKeyPressed(KEY_D)) {
                    casoAvancar(&caso);
                    vis.ultimo_resolvido = 0;
                }
                i += 76;
            }
            if (sala != NULL && sala->ant != NULL) {
                if (botao("VOLTAR", i, 188, 64, 16, AZUL) || IsKeyPressed(KEY_A)) {
                    casoVoltar(&caso);
                    vis.ultimo_resolvido = 0;
                }
            }
            if (sala != NULL && sala->inicia_batalha && !sala->batalha_concluida) {
                const char *rotulo = sala->e_chefe ? "GIOVANNI" : "LUTAR";
                if (botao(rotulo, 280, 188, 96, 16, VERMELHO) || IsKeyPressed(KEY_B)) {
                    if (casoIniciarBatalha(&caso)) {
                        vis.fila_n = 0;
                        vis.hp_pronto = 0;
                        enfileirarTexto(caso.oponente_atual.fala);
                        tela = TELA_BATALHA;
                        vis.fade = 1.0f;
                        vis.entrada = 0.0f;
                    }
                }
            }
        } else if (tela == TELA_BATALHA) {
            const Pokemon *eu = obterAtivo(&caso.nucleo.time);
            int golpe;

            desenharBatalha(&caso);
            golpe = cliqueGolpe(eu);
            if (golpe >= 0) {
                SaidaBatalha saida;
                int codigo;

                vis.fila_n = 0;
                saida.narrar = narrarCombate;
                saida.contexto = NULL;
                saida.log = &caso.nucleo.log;
                codigo = casoExecutarGolpe(&caso, golpe, &saida);
                vis.hp_pronto = 1;
                if (codigo == 0) {
                    if (caso.caso_encerrado) {
                        resultado = 1;
                        tela = TELA_FIM;
                        vis.fade = 1.0f;
                    } else {
                        tela = TELA_SALA;
                        vis.fade = 0.6f;
                    }
                } else if (codigo < 0 && codigo != -2) {
                    resultado = -1;
                    tela = TELA_FIM;
                    vis.fade = 1.0f;
                }
            }
        } else if (tela == TELA_FIM) {
            desenharFim(resultado > 0);
            if (resultado <= 0 && (botao("DE NOVO", 40, 140, 80, 16, AZUL))) {
                if (casoIniciarBatalha(&caso)) {
                    vis.fila_n = 0;
                    vis.hp_pronto = 0;
                    tela = TELA_BATALHA;
                }
            }
            if (botao("MENU", 140, 140, 64, 16, AZUL)) {
                tela = TELA_MENU;
            }
        }
        fecharQuadro();
    }

    fecharGrafico();
    encerrarCaso(&caso);
    return 0;
}

static void irAteOChefe(Caso *caso) {
    int seguranca = 0;

    while (!caso->caso_encerrado && seguranca < 40) {
        NoSala *sala = salaAtual(&caso->salas);
        int luta = 1;
        int golpes = 0;

        if (sala == NULL) {
            return;
        }
        if (sala->e_chefe) {
            casoIniciarBatalha(caso);
            return;
        }
        if (!sala->resolvido) {
            casoUsarHabilidade(caso);
        }
        if (sala->inicia_batalha && !sala->batalha_concluida) {
            if (!casoIniciarBatalha(caso)) {
                return;
            }
            while (luta == 1 && golpes < 60) {
                const Pokemon *eu = obterAtivo(&caso->nucleo.time);
                const Pokemon *opp = obterAtivo(&caso->oponente_atual.time);
                luta = casoExecutarGolpe(caso, escolherGolpeAdversario(eu, opp), NULL);
                golpes++;
            }
            if (caso->captura_disponivel) {
                casoCapturar(caso);
            }
        }
        if (!casoAvancar(caso)) {
            return;
        }
        seguranca++;
    }
}

static void quadroSala(Caso *caso, int caderno, const char *arquivo) {
    vis.fade = 0.0f;
    vis.texto_completo = 1;
    vis.caderno = caderno;
    comecarQuadro();
    desenharSala(caso);
    if (caderno) {
        desenharCaderno(caso);
    }
    if (salaAtual(&caso->salas) != NULL && !salaAtual(&caso->salas)->resolvido) {
        botao("CHOQUE", 4, 192, 72, 16, AZUL);
    } else {
        botao("AVANCAR", 4, 192, 72, 16, AZUL);
    }
    botao("CADERNO", 160, 192, 72, 16, AZUL);
    fecharQuadro();
    TakeScreenshot(arquivo);
}

int capturarTelasDemonstracao(void) {
    Caso caso;
    Caso chefe;

    if (!iniciarCaso(&caso) || !comecarCaso(&caso)) {
        fprintf(stderr, "Não foi possível montar o caso para as capturas.\n");
        return 1;
    }
    zerarVisual();
    vis.fade = 0.0f;
    abrirGrafico();

    comecarQuadro();
    desenharMenu();
    botao("NOVO JOGO", 112, 146, 160, 18, AZUL);
    botao("AJUDA", 28, 170, 100, 18, AZUL);
    botao("POKEDEX", 140, 170, 100, 18, AZUL);
    botao("SAIR", 252, 170, 100, 18, VERMELHO);
    fecharQuadro();
    TakeScreenshot("captura_menu.png");

    quadroSala(&caso, 0, "captura_sala.png");
    casoUsarHabilidade(&caso);
    soltarParticulas(188, 70, HAB_ELETRICA);
    quadroSala(&caso, 0, "captura_puzzle.png");
    quadroSala(&caso, 1, "captura_caderno.png");

    casoAvancar(&caso);
    if (casoIniciarBatalha(&caso)) {
        const Pokemon *eu = obterAtivo(&caso.nucleo.time);
        vis.fade = 0.0f;
        vis.texto_completo = 1;
        vis.hp_pronto = 0;
        atualizarBarras(&caso, 0.016f);
        vis.entrada = 1.0f;
        enfileirarTexto(caso.oponente_atual.fala);
        vis.bytes_texto = (int)strlen(vis.texto);
        comecarQuadro();
        desenharBatalha(&caso);
        fecharQuadro();
        TakeScreenshot("captura_batalha.png");
        (void)eu;
    }
    encerrarCaso(&caso);

    if (iniciarCaso(&chefe) && comecarCaso(&chefe)) {
        irAteOChefe(&chefe);
        vis.fade = 0.0f;
        vis.texto_completo = 1;
        vis.hp_pronto = 0;
        vis.fila_n = 0;
        atualizarBarras(&chefe, 0.016f);
        vis.entrada = 1.0f;
        vis.tremor = 0.0f;
        enfileirarTexto(chefe.oponente_atual.fala);
        vis.bytes_texto = (int)strlen(vis.texto);
        comecarQuadro();
        desenharBatalha(&chefe);
        fecharQuadro();
        TakeScreenshot("captura_boss.png");
        encerrarCaso(&chefe);
    }

    fecharGrafico();
    return 0;
}
