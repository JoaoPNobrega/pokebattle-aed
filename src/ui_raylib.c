/*
 * ui_raylib.c
 * -----------
 * Só desenho e leitura de teclado. A regra continua em caso.c,
 * sala.c e batalha.c. Esta tela não abre rede: os sprites vêm de
 * assets/pokemon/ (PokéAPI, gravados por tools/baixar_assets.py)
 * e o chão vem do tileset CC0 do Kenney.
 *
 * A cena nasce em 240×160, o tamanho do GBA, e sobe para a janela
 * em escala inteira com filtro de ponto. Os sprites de batalha da
 * FireRed já nascem em 64×64, o tamanho nativo daquela tela, então
 * entram em 1:1 no buffer. A ampliação nítida é a da janela.
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

#define VIRT_L 240
#define VIRT_A 160
#define ESCALA 4
#define JANELA_L (VIRT_L * ESCALA)
#define JANELA_A (VIRT_A * ESCALA)
#define TILE 16
#define MAPA_L 20
#define MAPA_A 14
#define FONTE_PX 8
#define ESPACO_FONTE 0.0f
#define MAX_FILA 12
#define MAX_POKEMON 152

typedef enum {
    TELA_MENU = 0,
    TELA_AJUDA,
    TELA_POKEDEX,
    TELA_SALA,
    TELA_BATALHA
} Tela;

typedef enum {
    BAT_ENTRADA = 0,
    BAT_TEXTO,
    BAT_COMANDO,
    BAT_GOLPES,
    BAT_TIME,
    BAT_CAPTURA,
    BAT_FIM
} FaseBatalha;

typedef const char *Mapa[MAPA_A];

typedef struct {
    Texture2D frente;
    Texture2D costas;
    Texture2D icone;
    int tem_frente;
    int tem_costas;
    int tem_icone;
} SpriteMon;

typedef struct {
    float tempo;
    float acum;
    int bytes;
    int texto_completo;
    int fala_aberta;
    char fala[TEXTO_SALA_MAX];
    char fila[MAX_FILA][LOG_MAX];
    int fila_n;
    int fila_i;
    int resultado;
    int captura_feita;
    int fase;
    int cursor;
    int cursor_time;
    float entrada;
    float hp_eu;
    float hp_opp;
    int queda_eu;
    int queda_opp;
    float flash_eu;
    float flash_opp;
    float tremor_eu;
    float tremor_opp;
    float trans_t;
    int transicao;
    float captura_t;
    int caderno;
    int leu_puzzle;
    int pp[6][4];
    int pp_init[6];
    int vitoria;
    int ultimo_eu;
    int ultimo_opp;
    int menu_cursor;
    int pokedex_topo;
    int som_ok;
    Sound som_clique;
    Sound som_acerto;
} Visual;

typedef struct {
    int tx, ty;
    int de_x, de_y;
    int para_x, para_y;
    float anda;
    int movendo;
    int dir;
    int pika_x, pika_y;
    int pika_de_x, pika_de_y;
    float pika_anda;
    int pika_dir;
    int estava_encontro;
    char sala[NOME_SALA_MAX];
} Mundo;

typedef struct {
    const Pokemon *itens[32];
    int n;
} ColetaPk;

static Font fonte;
static Font fonte_titulo;
static int fonte_ok = 0;
static int fonte_titulo_ok = 0;
static RenderTexture2D alvo;
static int grafico_ok = 0;
static Texture2D tileset;
static int tiles_ok = 0;
static Texture2D detetive_tex;
static Texture2D giovanni_ow;
static Texture2D giovanni_batalha;
static Texture2D pokebola_tex;
static int detetive_ok = 0;
static int giovanni_ow_ok = 0;
static int giovanni_bat_ok = 0;
static int pokebola_ok = 0;
static SpriteMon sprites[MAX_POKEMON];
static char raiz_assets[256];
static Caso caso_jogo;
static int caso_ok = 0;
static Visual vis;
static Mundo mundo;
static Tela tela = TELA_MENU;

static const int NUMEROS_SPRITE[] = {
    1, 3, 4, 6, 7, 9, 18, 25, 52, 54,
    65, 68, 74, 94, 95, 130, 143, 149, 150, 151
};

static Mapa MAPA_LAB = {
    "####################",
    "#RSCC........BBBB.C#",
    "#M................M#",
    "#M.P..............M#",
    "#..................#",
    "#..MM........MM....#",
    "#..MM..,,,,..MM....#",
    "#......,,,,........#",
    "#..................#",
    "#M................M#",
    "#V................V#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_PATIO = {
    "####################",
    "#BB..............BB#",
    "#B................B#",
    "#..................#",
    "#..................#",
    "#.........E........#",
    "#..................#",
    "#..MM........MM....#",
    "#..MM........MM....#",
    "#..................#",
    "#V................V#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_FRIA = {
    "####################",
    "#M................M#",
    "#,,,,,,,,,,,,,,,,,,#",
    "#,................,#",
    "#,................,#",
    "#,................,#",
    "#,................,#",
    "#,................,#",
    "#,,,,,,,,,,,,,,,,,,#",
    "#.......IIII.......#",
    "#........PP........#",
    "#........I.........#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_TANQUE = {
    "####################",
    "#M................M#",
    "#..~~~~~~~~~~~~~~..#",
    "#..~~~~~~~~~~~~~~..#",
    "#..~~~~~~~~~~~~~~..#",
    "#..~~~~~~~~~~~~~~..#",
    "#........E.........#",
    "#..................#",
    "#..................#",
    "#M................M#",
    "#..................#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_OFICINA = {
    "####################",
    "#C................C#",
    "#..................#",
    "#M................M#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#.......FFFF.......#",
    "#........FF........#",
    "#........F.........#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_JAULA = {
    "####################",
    "#HHHHHHHHHHHHHHHHHH#",
    "#H................H#",
    "#H................H#",
    "#H.......E........H#",
    "#H................H#",
    "#HHHHHHH..HHHHHHHHH#",
    "#..................#",
    "#..................#",
    "#M................M#",
    "#..................#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_ARQUIVO = {
    "####################",
    "#BBBBBBBBBBBBBBBBBB#",
    "#B................B#",
    "#B................B#",
    "#B................B#",
    "#B................B#",
    "#B................B#",
    "#..................#",
    "#..................#",
    "#M................M#",
    "#........BB........#",
    "#........P.........#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_CHEFE = {
    "####################",
    "#M................M#",
    "#........G.........#",
    "#..................#",
    "#..................#",
    "#..C............C..#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#M................M#",
    "#..................#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static void texto(const char *conteudo, float x, float y, float tamanho, Color cor);
static int avancarPonto(const char *texto, int indice);
static const char **mapaDa(const char *nome);
static char tileEm(const char **mapa, int x, int y);
static int solido(char simbolo, const NoSala *sala);
static void entrarNaSala(void);
static void abrirFala(const char *texto);
static void prepararBatalha(void);
static void desenharTudo(void);
static void apresentar(const char *png);
static int abrirGrafico(void);
static void fecharGrafico(void);

static void zerarVisual(void) {
    Sound clique;
    Sound acerto;
    int som;
    int topo;

    clique = vis.som_clique;
    acerto = vis.som_acerto;
    som = vis.som_ok;
    topo = vis.pokedex_topo;
    memset(&vis, 0, sizeof vis);
    vis.som_clique = clique;
    vis.som_acerto = acerto;
    vis.som_ok = som;
    vis.pokedex_topo = topo;
    vis.entrada = 1.0f;
    vis.hp_eu = 1.0f;
    vis.hp_opp = 1.0f;
    vis.resultado = 1;
    memset(&mundo, 0, sizeof mundo);
    mundo.anda = 1.0f;
    mundo.pika_anda = 1.0f;
}

static void caminhoAsset(char *destino, size_t capacidade, const char *relativo) {
    snprintf(destino, capacidade, "%s/%s", raiz_assets, relativo);
}

static int acharRaiz(void) {
    const char *candidatos[] = {"assets", "../assets", "/workspace/assets"};
    int i;

    for (i = 0; i < 3; i++) {
        char prova[512];
        snprintf(prova, sizeof prova, "%s/fontes/PressStart2P-Regular.ttf", candidatos[i]);
        if (FileExists(prova)) {
            copiarTexto(raiz_assets, sizeof raiz_assets, candidatos[i]);
            return 1;
        }
    }
    copiarTexto(raiz_assets, sizeof raiz_assets, "assets");
    return 0;
}

static Texture2D carregarPng(const char *relativo, int *ok) {
    char caminho[512];
    Texture2D tex;

    memset(&tex, 0, sizeof tex);
    *ok = 0;
    caminhoAsset(caminho, sizeof caminho, relativo);
    if (!FileExists(caminho)) {
        return tex;
    }
    tex = LoadTexture(caminho);
    if (tex.id == 0) {
        return tex;
    }
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    *ok = 1;
    return tex;
}

static void carregarFonteEm(const char *relativo, int tamanho, Font *fonte_dst, int *ok) {
    char caminho[512];
    int codigos[224];
    int i;

    *ok = 0;
    for (i = 0; i < 224; i++) {
        codigos[i] = 32 + i;
    }
    caminhoAsset(caminho, sizeof caminho, relativo);
    if (!FileExists(caminho)) {
        return;
    }
    *fonte_dst = LoadFontEx(caminho, tamanho, codigos, 224);
    if (fonte_dst->texture.id == 0) {
        return;
    }
    SetTextureFilter(fonte_dst->texture, TEXTURE_FILTER_POINT);
    *ok = 1;
}

static void carregarSpritesPokemon(void) {
    int i;

    memset(sprites, 0, sizeof sprites);
    for (i = 0; i < (int)(sizeof NUMEROS_SPRITE / sizeof NUMEROS_SPRITE[0]); i++) {
        int n = NUMEROS_SPRITE[i];
        char frente[64];
        char costas[64];
        char icone[64];

        if (n <= 0 || n >= MAX_POKEMON) {
            continue;
        }
        snprintf(frente, sizeof frente, "pokemon/%d_front.png", n);
        snprintf(costas, sizeof costas, "pokemon/%d_back.png", n);
        snprintf(icone, sizeof icone, "pokemon/%d_icon.png", n);
        sprites[n].frente = carregarPng(frente, &sprites[n].tem_frente);
        sprites[n].costas = carregarPng(costas, &sprites[n].tem_costas);
        sprites[n].icone = carregarPng(icone, &sprites[n].tem_icone);
    }
}

static Sound criarTom(float frequencia, float duracao) {
    unsigned int taxa = 22050;
    unsigned int quadros = (unsigned int)(taxa * duracao);
    short *amostras;
    unsigned int i;
    Wave onda;
    Sound som;

    memset(&som, 0, sizeof som);
    amostras = (short *)calloc(quadros, sizeof(short));
    if (amostras == NULL) {
        return som;
    }
    for (i = 0; i < quadros; i++) {
        float t = (float)i / (float)taxa;
        float env = 1.0f - (t / duracao);
        float s = sinf(t * frequencia * 6.2831853f) > 0.0f ? 1.0f : -1.0f;
        amostras[i] = (short)(s * env * 9000.0f);
    }
    memset(&onda, 0, sizeof onda);
    onda.frameCount = quadros;
    onda.sampleRate = taxa;
    onda.sampleSize = 16;
    onda.channels = 1;
    onda.data = amostras;
    som = LoadSoundFromWave(onda);
    UnloadWave(onda);
    return som;
}

static void carregarAssets(void) {
    acharRaiz();
    carregarFonteEm("fontes/PressStart2P-Regular.ttf", FONTE_PX, &fonte, &fonte_ok);
    carregarFonteEm("fontes/PressStart2P-Regular.ttf", 16, &fonte_titulo, &fonte_titulo_ok);
    tileset = carregarPng("tiles/kenney_indoor.png", &tiles_ok);
    detetive_tex = carregarPng("personagens/detetive.png", &detetive_ok);
    giovanni_ow = carregarPng("personagens/giovanni_overworld.png", &giovanni_ow_ok);
    giovanni_batalha = carregarPng("personagens/giovanni.png", &giovanni_bat_ok);
    pokebola_tex = carregarPng("personagens/pokebola.png", &pokebola_ok);
    carregarSpritesPokemon();
    InitAudioDevice();
    if (IsAudioDeviceReady()) {
        vis.som_clique = criarTom(660.0f, 0.04f);
        vis.som_acerto = criarTom(180.0f, 0.08f);
        vis.som_ok = 1;
    }
}

static void liberarTextura(Texture2D *tex, int *ok) {
    if (*ok) {
        UnloadTexture(*tex);
        *ok = 0;
    }
}

static void liberarAssets(void) {
    int i;

    for (i = 0; i < MAX_POKEMON; i++) {
        liberarTextura(&sprites[i].frente, &sprites[i].tem_frente);
        liberarTextura(&sprites[i].costas, &sprites[i].tem_costas);
        liberarTextura(&sprites[i].icone, &sprites[i].tem_icone);
    }
    liberarTextura(&tileset, &tiles_ok);
    liberarTextura(&detetive_tex, &detetive_ok);
    liberarTextura(&giovanni_ow, &giovanni_ow_ok);
    liberarTextura(&giovanni_batalha, &giovanni_bat_ok);
    liberarTextura(&pokebola_tex, &pokebola_ok);
    if (fonte_ok) {
        UnloadFont(fonte);
        fonte_ok = 0;
    }
    if (fonte_titulo_ok) {
        UnloadFont(fonte_titulo);
        fonte_titulo_ok = 0;
    }
    if (vis.som_ok) {
        UnloadSound(vis.som_clique);
        UnloadSound(vis.som_acerto);
        vis.som_ok = 0;
    }
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
}

static int abrirGrafico(void) {
    if (grafico_ok) {
        return 1;
    }
    InitWindow(JANELA_L, JANELA_A, "PokéBattle: O Caso do Laboratório");
    if (!IsWindowReady()) {
        return 0;
    }
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    alvo = LoadRenderTexture(VIRT_L, VIRT_A);
    SetTextureFilter(alvo.texture, TEXTURE_FILTER_POINT);
    carregarAssets();
    grafico_ok = 1;
    return 1;
}

static void fecharGrafico(void) {
    if (!grafico_ok) {
        return;
    }
    liberarAssets();
    UnloadRenderTexture(alvo);
    CloseWindow();
    grafico_ok = 0;
}

static void clique(void) {
    if (vis.som_ok) {
        PlaySound(vis.som_clique);
    }
}

static float larguraTexto(const char *texto, float tamanho) {
    if (texto == NULL) {
        return 0.0f;
    }
    if (tamanho > 8.0f && fonte_titulo_ok) {
        return MeasureTextEx(fonte_titulo, texto, tamanho, ESPACO_FONTE).x;
    }
    if (fonte_ok) {
        return MeasureTextEx(fonte, texto, tamanho, ESPACO_FONTE).x;
    }
    return (float)strlen(texto) * tamanho;
}

static void textoAte(const char *origem, float x, float y, float maximo, Color cor) {
    char buf[64];
    int cortes = 0;

    copiarTexto(buf, sizeof buf, origem == NULL ? "" : origem);
    while (buf[0] != '\0' && larguraTexto(buf, FONTE_PX) > maximo && cortes < 40) {
        int fim = 0;
        int anterior = 0;
        while (buf[fim] != '\0') {
            anterior = fim;
            fim = avancarPonto(buf, fim);
        }
        buf[anterior] = '\0';
        cortes++;
    }
    texto(buf, x, y, FONTE_PX, cor);
}

static void texto(const char *conteudo, float x, float y, float tamanho, Color cor) {
    if (conteudo == NULL) {
        return;
    }
    if (tamanho > 8.0f && fonte_titulo_ok) {
        DrawTextEx(fonte_titulo, conteudo, (Vector2){x, y}, tamanho, ESPACO_FONTE, cor);
        return;
    }
    if (fonte_ok) {
        DrawTextEx(fonte, conteudo, (Vector2){x, y}, tamanho, ESPACO_FONTE, cor);
        return;
    }
    DrawText(conteudo, (int)x, (int)y, (int)tamanho, cor);
}

static void caixa(int x, int y, int w, int h, Color miolo) {
    DrawRectangle(x, y, w, h, BLACK);
    if (w > 4 && h > 4) {
        DrawRectangle(x + 2, y + 2, w - 4, h - 4, miolo);
        DrawRectangle(x + 2, y + h - 4, w - 4, 2, (Color){180, 180, 170, 255});
    }
}

static Color corBarra(float razao) {
    if (razao > 0.5f) {
        return (Color){64, 200, 88, 255};
    }
    if (razao > 0.2f) {
        return (Color){240, 208, 64, 255};
    }
    return (Color){220, 56, 48, 255};
}

static Color corTipo(Tipo tipo) {
    switch (tipo) {
    case TIPO_FOGO: return (Color){224, 96, 48, 255};
    case TIPO_AGUA: return (Color){64, 128, 224, 255};
    case TIPO_ELETRICO: return (Color){240, 200, 48, 255};
    case TIPO_GRAMA: return (Color){88, 184, 72, 255};
    case TIPO_GELO: return (Color){128, 216, 216, 255};
    case TIPO_LUTADOR: return (Color){176, 64, 48, 255};
    case TIPO_VENENO: return (Color){160, 72, 168, 255};
    case TIPO_TERRA: return (Color){196, 168, 80, 255};
    case TIPO_VOADOR: return (Color){144, 168, 232, 255};
    case TIPO_PSIQUICO: return (Color){216, 80, 136, 255};
    case TIPO_INSETO: return (Color){160, 176, 48, 255};
    case TIPO_PEDRA: return (Color){176, 160, 80, 255};
    case TIPO_FANTASMA: return (Color){104, 80, 152, 255};
    case TIPO_DRAGAO: return (Color){96, 80, 200, 255};
    case TIPO_NORMAL:
    case TIPO_NENHUM:
    default:
        return (Color){160, 160, 144, 255};
    }
}

static int avancarPonto(const char *texto, int indice) {
    unsigned char c;

    if (texto[indice] == '\0') {
        return indice;
    }
    c = (unsigned char)texto[indice];
    if ((c & 0x80) == 0) {
        return indice + 1;
    }
    if ((c & 0xE0) == 0xC0 && texto[indice + 1] != '\0') {
        return indice + 2;
    }
    if ((c & 0xF0) == 0xE0 && texto[indice + 1] != '\0' && texto[indice + 2] != '\0') {
        return indice + 3;
    }
    if (texto[indice + 1] != '\0' && texto[indice + 2] != '\0' && texto[indice + 3] != '\0') {
        return indice + 4;
    }
    return indice + 1;
}

static void prefixo(const char *origem, int bytes, char *destino, size_t capacidade) {
    int i = 0;

    if (capacidade == 0) {
        return;
    }
    while (origem[i] != '\0' && i < bytes && (size_t)i + 1 < capacidade) {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

static void quebrar(const char *origem, char linhas[][48], int *quantidade, int maximo, float largura) {
    char palavra[48];
    int i = 0;
    int n = 0;
    char atual[48];

    atual[0] = '\0';
    *quantidade = 0;
    if (origem == NULL) {
        return;
    }
    while (origem[i] != '\0' && n < maximo) {
        int p = 0;
        int prox;

        while (origem[i] == ' ') {
            i++;
        }
        if (origem[i] == '\0') {
            break;
        }
        while (origem[i] != '\0' && origem[i] != ' ' && p + 1 < (int)sizeof palavra) {
            prox = avancarPonto(origem, i);
            while (i < prox && p + 1 < (int)sizeof palavra) {
                palavra[p++] = origem[i++];
            }
        }
        palavra[p] = '\0';
        if (atual[0] == '\0') {
            copiarTexto(atual, sizeof atual, palavra);
        } else {
            char tentativa[160];
            size_t ia = 0;
            size_t ip = 0;

            while (atual[ia] != '\0' && ia < 47) {
                tentativa[ia] = atual[ia];
                ia++;
            }
            tentativa[ia] = ' ';
            while (palavra[ip] != '\0' && ia + 1 + ip < 120) {
                tentativa[ia + 1 + ip] = palavra[ip];
                ip++;
            }
            tentativa[ia + 1 + ip] = '\0';
            if (larguraTexto(tentativa, FONTE_PX) <= largura) {
                copiarTexto(atual, sizeof atual, tentativa);
            } else {
                copiarTexto(linhas[n], 48, atual);
                n++;
                copiarTexto(atual, sizeof atual, palavra);
            }
        }
    }
    if (atual[0] != '\0' && n < maximo) {
        copiarTexto(linhas[n], 48, atual);
        n++;
    }
    *quantidade = n;
}

static const char *textoBatalha(void) {
    if (vis.fila_n <= 0 || vis.fila_i < 0 || vis.fila_i >= vis.fila_n) {
        return "";
    }
    return vis.fila[vis.fila_i];
}

static void revelarTexto(float dt, const char *conteudo) {
    int total;

    if (conteudo == NULL) {
        conteudo = "";
    }
    total = (int)strlen(conteudo);
    if (vis.texto_completo || vis.bytes >= total) {
        vis.bytes = total;
        vis.texto_completo = 1;
        return;
    }
    vis.acum += dt;
    while (vis.acum > 0.028f && vis.bytes < total) {
        vis.acum -= 0.028f;
        vis.bytes = avancarPonto(conteudo, vis.bytes);
    }
    if (vis.bytes >= total) {
        vis.texto_completo = 1;
    }
}

static void completarTexto(const char *conteudo) {
    vis.bytes = conteudo == NULL ? 0 : (int)strlen(conteudo);
    vis.texto_completo = 1;
}

static void desenharMensagem(const char *conteudo, int x, int y, int w, int h) {
    char visivel[TEXTO_SALA_MAX];
    char linhas[6][48];
    int n = 0;
    int i;

    caixa(x, y, w, h, (Color){248, 248, 240, 255});
    prefixo(conteudo == NULL ? "" : conteudo, vis.bytes, visivel, sizeof visivel);
    quebrar(visivel, linhas, &n, 4, (float)w - 12.0f);
    for (i = 0; i < n; i++) {
        texto(linhas[i], (float)x + 6.0f, (float)y + 6.0f + (float)i * 10.0f, FONTE_PX, BLACK);
    }
    if (vis.texto_completo && ((int)(vis.tempo * 3.0f) % 2) == 0) {
        int ax = x + w - 14;
        int ay = y + h - 12;
        DrawRectangle(ax, ay, 6, 2, BLACK);
        DrawRectangle(ax + 2, ay + 2, 2, 2, BLACK);
    }
}

static void filaLimpar(void) {
    vis.fila_n = 0;
    vis.fila_i = 0;
    vis.bytes = 0;
    vis.texto_completo = 0;
    vis.acum = 0.0f;
}

static void filaPor(const char *mensagem) {
    if (mensagem == NULL || mensagem[0] == '\0' || vis.fila_n >= MAX_FILA) {
        return;
    }
    copiarTexto(vis.fila[vis.fila_n], LOG_MAX, mensagem);
    vis.fila_n++;
}

static void narrarUi(void *contexto, const char *mensagem) {
    (void)contexto;
    filaPor(mensagem);
}

static void abrirFala(const char *mensagem) {
    copiarTexto(vis.fala, sizeof vis.fala, mensagem == NULL ? "" : mensagem);
    vis.fala_aberta = 1;
    vis.bytes = 0;
    vis.texto_completo = 0;
    vis.acum = 0.0f;
}

static const char **mapaDa(const char *nome) {
    if (nome == NULL) {
        return MAPA_LAB;
    }
    if (strcmp(nome, "Pátio") == 0) return MAPA_PATIO;
    if (strcmp(nome, "Câmara fria") == 0) return MAPA_FRIA;
    if (strcmp(nome, "Tanque") == 0) return MAPA_TANQUE;
    if (strcmp(nome, "Oficina") == 0) return MAPA_OFICINA;
    if (strcmp(nome, "Jaula") == 0) return MAPA_JAULA;
    if (strcmp(nome, "Arquivo") == 0) return MAPA_ARQUIVO;
    if (strcmp(nome, "Sala do chefe") == 0) return MAPA_CHEFE;
    return MAPA_LAB;
}

static char tileEm(const char **mapa, int x, int y) {
    if (mapa == NULL || y < 0 || y >= MAPA_A || x < 0 || x >= MAPA_L) {
        return '#';
    }
    if (mapa[y] == NULL || (int)strlen(mapa[y]) <= x) {
        return '#';
    }
    return mapa[y][x];
}

static int solido(char simbolo, const NoSala *sala) {
    if (simbolo == '#' || simbolo == 'M' || simbolo == 'C' || simbolo == 'B' ||
        simbolo == 'R' || simbolo == 'S' || simbolo == 'V' || simbolo == 'P' ||
        simbolo == 'H' || simbolo == '~' || simbolo == 'D') {
        return 1;
    }
    if ((simbolo == 'I' || simbolo == 'F') && (sala == NULL || !sala->resolvido)) {
        return 1;
    }
    return 0;
}

static int portaAberta(const NoSala *sala) {
    if (sala == NULL || sala->prox == NULL) {
        return 0;
    }
    if (!sala->resolvido || !sala->prox->desbloqueada) {
        return 0;
    }
    if (sala->inicia_batalha && !sala->batalha_concluida) {
        return 0;
    }
    return 1;
}

static int numeroEncontro(const char *nome) {
    if (nome == NULL) {
        return 0;
    }
    if (strcmp(nome, "Pátio") == 0) return 4;
    if (strcmp(nome, "Tanque") == 0) return 7;
    if (strcmp(nome, "Jaula") == 0) return 65;
    return 0;
}

static void blitTile(int coluna, int linha, int x, int y) {
    Rectangle origem;
    Rectangle destino;

    if (!tiles_ok) {
        DrawRectangle(x, y, TILE, TILE, (Color){96, 72, 48, 255});
        return;
    }
    origem = (Rectangle){(float)coluna * 17.0f, (float)linha * 17.0f, 16.0f, 16.0f};
    destino = (Rectangle){(float)x, (float)y, 16.0f, 16.0f};
    DrawTexturePro(tileset, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void entrarNaSala(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char **mapa;
    int y;
    int px;
    int py;

    if (sala == NULL) {
        return;
    }
    mapa = mapaDa(sala->nome);
    copiarTexto(mundo.sala, sizeof mundo.sala, sala->nome);
    mundo.tx = 10;
    mundo.ty = 4;
    if (solido(tileEm(mapa, mundo.tx, mundo.ty), sala)) {
        for (y = 1; y < MAPA_A - 1; y++) {
            if (!solido(tileEm(mapa, 10, y), sala)) {
                mundo.ty = y;
                break;
            }
        }
    }
    mundo.de_x = mundo.tx;
    mundo.de_y = mundo.ty;
    mundo.para_x = mundo.tx;
    mundo.para_y = mundo.ty;
    mundo.anda = 1.0f;
    mundo.movendo = 0;
    mundo.dir = 0;
    px = mundo.tx;
    py = mundo.ty - 2;
    if (solido(tileEm(mapa, px, py), sala)) {
        py = mundo.ty - 1;
    }
    if (solido(tileEm(mapa, px, py), sala)) {
        px = mundo.tx;
        py = mundo.ty + 1;
    }
    if (solido(tileEm(mapa, px, py), sala)) {
        px = mundo.tx;
        py = mundo.ty;
    }
    mundo.pika_x = px;
    mundo.pika_y = py;
    mundo.pika_de_x = px;
    mundo.pika_de_y = py;
    mundo.pika_anda = 1.0f;
    mundo.pika_dir = 0;
    mundo.estava_encontro = (tileEm(mapa, mundo.tx, mundo.ty) == 'E' ||
                             tileEm(mapa, mundo.tx, mundo.ty) == 'G');
    vis.leu_puzzle = 0;
    vis.fala_aberta = 0;
    vis.caderno = 0;
}

static void tentarPorta(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);

    if (sala == NULL) {
        return;
    }
    if (!casoAvancar(&caso_jogo)) {
        if (sala->inicia_batalha && !sala->batalha_concluida) {
            abrirFala("Ainda há um Pokémon no caminho.");
        } else {
            abrirFala("A porta está trancada.");
        }
        return;
    }
    clique();
    entrarNaSala();
}

static void comecarTransicao(void) {
    vis.transicao = 1;
    vis.trans_t = 0.0f;
}

static void prepararBatalha(void) {
    const Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
    const Pokemon *op = obterAtivo(&caso_jogo.oponente_atual.time);

    filaLimpar();
    vis.fase = BAT_ENTRADA;
    vis.entrada = 0.0f;
    vis.cursor = 0;
    vis.captura_t = 0.0f;
    vis.captura_feita = 0;
    vis.resultado = 1;
    vis.vitoria = 0;
    vis.flash_eu = 0.0f;
    vis.flash_opp = 0.0f;
    vis.tremor_eu = 0.0f;
    vis.tremor_opp = 0.0f;
    vis.queda_eu = 0;
    vis.queda_opp = 0;
    if (eu != NULL && eu->hp_max > 0) {
        vis.hp_eu = (float)eu->hp_atual / (float)eu->hp_max;
        vis.ultimo_eu = eu->numero;
    }
    if (op != NULL && op->hp_max > 0) {
        vis.hp_opp = (float)op->hp_atual / (float)op->hp_max;
        vis.ultimo_opp = op->numero;
    }
    if (caso_jogo.oponente_atual.fala[0] != '\0') {
        filaPor(caso_jogo.oponente_atual.fala);
    } else if (op != NULL) {
        char linha[LOG_MAX];
        snprintf(linha, sizeof linha, "Um %s selvagem apareceu!", op->nome);
        filaPor(linha);
    } else {
        filaPor("A luta começou.");
    }
}

static void tentarBatalha(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);

    if (sala == NULL) {
        return;
    }
    if (sala->batalha_concluida) {
        abrirFala(sala->texto_resolvido);
        return;
    }
    if (!casoIniciarBatalha(&caso_jogo)) {
        abrirFala("Não há luta aqui.");
        return;
    }
    clique();
    comecarTransicao();
}

static void tentarPuzzle(char simbolo) {
    NoSala *sala = salaAtual(&caso_jogo.salas);

    if (sala == NULL) {
        return;
    }
    if (simbolo != 'P' && simbolo != 'I' && simbolo != 'F') {
        return;
    }
    if (sala->resolvido) {
        abrirFala(sala->texto_resolvido);
        return;
    }
    if (!vis.leu_puzzle) {
        vis.leu_puzzle = 1;
        abrirFala(sala->texto_puzzle);
        return;
    }
    if (casoUsarHabilidade(&caso_jogo)) {
        clique();
        abrirFala(sala->texto_resolvido);
        return;
    }
    abrirFala("Nenhum Pokémon do time resolve isto.");
}

static int acaoPressionada(void) {
    return IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) ||
           IsKeyPressed(KEY_SPACE);
}

static int voltarPressionado(void) {
    return IsKeyPressed(KEY_X) || IsKeyPressed(KEY_BACKSPACE);
}

static int direcaoDe(int dx, int dy) {
    if (dx > 0) return 2;
    if (dx < 0) return 1;
    if (dy < 0) return 3;
    return 0;
}

static void comecarPasso(int dx, int dy) {
    mundo.dir = direcaoDe(dx, dy);
    mundo.de_x = mundo.tx;
    mundo.de_y = mundo.ty;
    mundo.para_x = mundo.tx + dx;
    mundo.para_y = mundo.ty + dy;
    mundo.anda = 0.0f;
    mundo.movendo = 1;
    mundo.pika_de_x = mundo.pika_x;
    mundo.pika_de_y = mundo.pika_y;
    mundo.pika_x = mundo.tx;
    mundo.pika_y = mundo.ty;
    mundo.pika_anda = 0.0f;
    mundo.pika_dir = mundo.dir;
}

static void lerMovimento(float dt) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    int direita;
    int esquerda;
    int baixo;
    int cima;
    int dx = 0;
    int dy = 0;
    int nx;
    int ny;
    char simbolo;

    if (mundo.movendo) {
        mundo.anda += dt / 0.16f;
        mundo.pika_anda += dt / 0.16f;
        if (mundo.pika_anda > 1.0f) {
            mundo.pika_anda = 1.0f;
        }
        if (mundo.anda >= 1.0f) {
            mundo.anda = 1.0f;
            mundo.tx = mundo.para_x;
            mundo.ty = mundo.para_y;
            mundo.movendo = 0;
        }
        return;
    }

    direita = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    esquerda = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    baixo = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    cima = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
    if (direita && !esquerda) dx = 1;
    else if (esquerda && !direita) dx = -1;
    else if (baixo && !cima) dy = 1;
    else if (cima && !baixo) dy = -1;
    if (dx == 0 && dy == 0) {
        return;
    }
    mundo.dir = direcaoDe(dx, dy);
    nx = mundo.tx + dx;
    ny = mundo.ty + dy;
    simbolo = tileEm(mapa, nx, ny);
    if (simbolo == 'D') {
        tentarPorta();
        return;
    }
    if (solido(simbolo, sala)) {
        return;
    }
    comecarPasso(dx, dy);
}

static void verificarEncontro(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    char simbolo = tileEm(mapa, mundo.tx, mundo.ty);
    int encontro = (simbolo == 'E' || simbolo == 'G');

    if (!mundo.movendo && encontro && !mundo.estava_encontro) {
        tentarBatalha();
    }
    if (!mundo.movendo) {
        mundo.estava_encontro = encontro;
    }
}

static void interagir(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    int fx = mundo.tx;
    int fy = mundo.ty;
    char a_frente;
    char aqui;

    if (mundo.dir == 0) fy++;
    else if (mundo.dir == 3) fy--;
    else if (mundo.dir == 1) fx--;
    else fx++;
    a_frente = tileEm(mapa, fx, fy);
    aqui = tileEm(mapa, mundo.tx, mundo.ty);
    if (a_frente == 'D') {
        tentarPorta();
        return;
    }
    if (a_frente == 'E' || a_frente == 'G' || aqui == 'E' || aqui == 'G') {
        tentarBatalha();
        return;
    }
    if (a_frente == 'P' || a_frente == 'I' || a_frente == 'F') {
        tentarPuzzle(a_frente);
    }
}

static int copiarHistorico(char linhas[][LOG_MAX], int maximo) {
    char tmp[16][LOG_MAX];
    int n = 0;
    int i;
    int mostrar;
    int inicio;

    while (n < 16 && desempilharLog(&caso_jogo.nucleo.log, tmp[n], LOG_MAX)) {
        n++;
    }
    for (i = n - 1; i >= 0; i--) {
        empilharLog(&caso_jogo.nucleo.log, tmp[i]);
    }
    mostrar = n < maximo ? n : maximo;
    inicio = n - mostrar;
    for (i = 0; i < mostrar; i++) {
        int crono = inicio + i;
        copiarTexto(linhas[i], LOG_MAX, tmp[n - 1 - crono]);
    }
    return mostrar;
}

static void atualizarMundo(float dt) {
    NoSala *sala;

    if (vis.transicao != 0) {
        return;
    }
    sala = salaAtual(&caso_jogo.salas);
    if (sala != NULL && strcmp(sala->nome, mundo.sala) != 0) {
        entrarNaSala();
    }
    if (vis.fala_aberta) {
        revelarTexto(dt, vis.fala);
        if (acaoPressionada() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (!vis.texto_completo) {
                completarTexto(vis.fala);
            } else {
                vis.fala_aberta = 0;
            }
        }
        return;
    }
    if (vis.caderno) {
        if (acaoPressionada() || IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
            vis.caderno = 0;
        }
        return;
    }
    if (IsKeyPressed(KEY_N)) {
        vis.caderno = 1;
        return;
    }
    if (acaoPressionada()) {
        interagir();
        return;
    }
    lerMovimento(dt);
    verificarEncontro();
}

static void desenharSpriteMon(int numero, int costas, Rectangle destino, float flash) {
    SpriteMon *sp;
    Texture2D tex;
    int ok;
    Rectangle origem;
    Color tinta = WHITE;

    if (numero <= 0 || numero >= MAX_POKEMON) {
        DrawRectangleRec(destino, (Color){240, 200, 64, 255});
        return;
    }
    sp = &sprites[numero];
    if (costas && sp->tem_costas) {
        tex = sp->costas;
        ok = 1;
    } else if (sp->tem_frente) {
        tex = sp->frente;
        ok = 1;
    } else {
        ok = 0;
        memset(&tex, 0, sizeof tex);
    }
    if (!ok) {
        DrawRectangleRec(destino, corTipo(TIPO_NORMAL));
        return;
    }
    origem = (Rectangle){0.0f, 0.0f, (float)tex.width, (float)tex.height};
    if (flash > 0.15f) {
        tinta = (Color){255, 255, 255, 255};
        BeginBlendMode(BLEND_ADDITIVE);
        DrawTexturePro(tex, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, tinta);
        EndBlendMode();
    }
    DrawTexturePro(tex, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void desenharFolha(Texture2D folha, int ok, int dir, int quadro, float x, float y) {
    Rectangle origem;
    Rectangle destino;

    if (!ok) {
        DrawRectangle((int)x, (int)y, 16, 24, (Color){48, 72, 148, 255});
        return;
    }
    if (dir < 0) dir = 0;
    if (dir > 3) dir = 0;
    if (quadro < 0) quadro = 0;
    if (quadro > 2) quadro = 2;
    origem = (Rectangle){(float)quadro * 16.0f, (float)dir * 32.0f, 16.0f, 32.0f};
    destino = (Rectangle){x, y, 16.0f, 32.0f};
    DrawTexturePro(folha, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void desenharMapa(const NoSala *sala, int camx, int camy) {
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    const char *nome = sala == NULL ? "" : sala->nome;
    int x0 = camx / TILE;
    int y0 = camy / TILE;
    int x1 = (camx + VIRT_L) / TILE + 2;
    int y1 = (camy + VIRT_A) / TILE + 2;
    int x;
    int y;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > MAPA_L) x1 = MAPA_L;
    if (y1 > MAPA_A) y1 = MAPA_A;

    for (y = y0; y < y1; y++) {
        for (x = x0; x < x1; x++) {
            char simbolo = tileEm(mapa, x, y);
            int px = x * TILE - camx;
            int py = y * TILE - camy;
            int aberto = portaAberta(sala);

            if (simbolo == ',') {
                blitTile(24, 6, px, py);
            } else if (simbolo != '#') {
                blitTile(24, 0, px, py);
            }
            if (simbolo == '#') {
                blitTile(0, 13, px, py);
            } else if (simbolo == 'M') {
                blitTile(2, 13, px, py);
            } else if (simbolo == 'C' || simbolo == 'R') {
                blitTile(22, 8, px, py);
            } else if (simbolo == 'B' || (simbolo == 'P' && strcmp(nome, "Arquivo") == 0)) {
                blitTile(5, 13, px, py);
            } else if (simbolo == 'S' || simbolo == 'P') {
                blitTile(16, 14, px, py);
            } else if (simbolo == 'V') {
                blitTile(16, 0, px, py);
            } else if (simbolo == 'D') {
                blitTile(aberto ? 13 : 5, aberto ? 9 : 9, px, py);
            } else if (simbolo == 'H') {
                DrawRectangle(px + 4, py, 3, TILE, (Color){48, 52, 64, 255});
                DrawRectangle(px + 10, py, 3, TILE, (Color){48, 52, 64, 255});
            } else if (simbolo == '~') {
                DrawRectangle(px, py, TILE, TILE, (Color){36, 110, 180, 220});
                DrawRectangle(px, py + 4, TILE, 2, (Color){180, 220, 255, 140});
            } else if (simbolo == 'I' && (sala == NULL || !sala->resolvido)) {
                DrawRectangle(px + 1, py + 1, 14, 14, (Color){186, 230, 240, 230});
                DrawRectangleLines(px + 1, py + 1, 14, 14, (Color){80, 140, 170, 255});
            } else if (simbolo == 'F' && (sala == NULL || !sala->resolvido)) {
                int ch = ((int)(vis.tempo * 8.0f) + x + y) % 2;
                DrawRectangle(px + 3, py + 4 + ch, 10, 10, (Color){220, 72, 24, 255});
                DrawRectangle(px + 5, py + 2 + ch, 6, 6, (Color){255, 196, 48, 255});
            }
        }
    }
}

static void desenharMundo(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    float jx;
    float jy;
    float t = mundo.movendo ? mundo.anda : 1.0f;
    float tp = mundo.pika_anda;
    float pix;
    float piy;
    float px;
    float py;
    int camx;
    int camy;
    int maxx = MAPA_L * TILE - VIRT_L;
    int maxy = MAPA_A * TILE - VIRT_A;
    int quadro = 0;
    int x;
    int y;
    int numero;

    if (tp > 1.0f) tp = 1.0f;
    jx = ((float)mundo.de_x + (float)(mundo.para_x - mundo.de_x) * t) * TILE;
    jy = ((float)mundo.de_y + (float)(mundo.para_y - mundo.de_y) * t) * TILE;
    pix = ((float)mundo.pika_de_x + (float)(mundo.pika_x - mundo.pika_de_x) * tp) * TILE;
    piy = ((float)mundo.pika_de_y + (float)(mundo.pika_y - mundo.pika_de_y) * tp) * TILE;
    camx = (int)jx - VIRT_L / 2 + TILE / 2;
    camy = (int)jy - VIRT_A / 2 + TILE;
    if (maxx < 0) maxx = 0;
    if (maxy < 0) maxy = 0;
    if (camx < 0) camx = 0;
    if (camy < 0) camy = 0;
    if (camx > maxx) camx = maxx;
    if (camy > maxy) camy = maxy;

    desenharMapa(sala, camx, camy);
    if (sala != NULL && strcmp(sala->nome, "Laboratório") == 0 && !sala->resolvido) {
        DrawRectangle(0, 0, VIRT_L, 28, (Color){0, 0, 16, 150});
        DrawRectangle(0, 0, 18, VIRT_A, (Color){0, 0, 16, 120});
        DrawRectangle(VIRT_L - 18, 0, 18, VIRT_A, (Color){0, 0, 16, 120});
    }
    if (sala != NULL && strcmp(sala->nome, "Câmara fria") == 0) {
        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){140, 190, 220, 40});
    }
    if (sala != NULL && strcmp(sala->nome, "Sala do chefe") == 0) {
        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){80, 0, 0, 28});
    }

    if (mundo.movendo) {
        quadro = mundo.anda < 0.5f ? 1 : 2;
    }
    px = jx - (float)camx;
    py = jy - (float)camy - 16.0f;
    {
        float fx = pix - (float)camx - 8.0f;
        float fy = piy - (float)camy - 16.0f;
        if (piy <= jy) {
            DrawEllipse((int)fx + 16, (int)fy + 30, 8.0f, 3.0f, (Color){0, 0, 0, 70});
            desenharSpriteMon(25, 0, (Rectangle){fx, fy, 32.0f, 32.0f}, 0.0f);
        }
        DrawEllipse((int)px + 8, (int)py + 30, 7.0f, 3.0f, (Color){0, 0, 0, 70});
        desenharFolha(detetive_tex, detetive_ok, mundo.dir, quadro, px, py);
        if (piy > jy) {
            DrawEllipse((int)fx + 16, (int)fy + 30, 8.0f, 3.0f, (Color){0, 0, 0, 70});
            desenharSpriteMon(25, 0, (Rectangle){fx, fy, 32.0f, 32.0f}, 0.0f);
        }
    }

    numero = numeroEncontro(sala == NULL ? "" : sala->nome);
    for (y = 0; y < MAPA_A; y++) {
        for (x = 0; x < MAPA_L; x++) {
            char simbolo = tileEm(mapa, x, y);
            float ex = (float)(x * TILE - camx);
            float ey = (float)(y * TILE - camy) - 16.0f;

            if (simbolo == 'E' && numero != 0 && sala != NULL && !sala->batalha_concluida) {
                DrawEllipse((int)ex + 8, (int)ey + 30, 8.0f, 3.0f, (Color){0, 0, 0, 70});
                desenharSpriteMon(numero, 0, (Rectangle){ex - 8.0f, ey, 32.0f, 32.0f}, 0.0f);
            }
            if (simbolo == 'G' && (sala == NULL || !sala->batalha_concluida)) {
                desenharFolha(giovanni_ow, giovanni_ow_ok, 0, 0, ex, ey);
            }
        }
    }

    if (sala != NULL && !vis.fala_aberta) {
        float lw = larguraTexto(sala->nome, FONTE_PX);
        caixa(2, 2, (int)lw + 12, 16, (Color){16, 20, 32, 230});
        texto(sala->nome, 8.0f, 6.0f, FONTE_PX, (Color){248, 248, 236, 255});
    }
    if (vis.fala_aberta) {
        desenharMensagem(vis.fala, 0, 112, VIRT_L, 48);
    }
    if (vis.caderno) {
        char linhas[8][LOG_MAX];
        int n;
        int i;
        NoSala *cursor;

        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){0, 0, 0, 90});
        caixa(16, 12, 208, 136, (Color){236, 224, 184, 255});
        texto("CADERNO", 24.0f, 18.0f, FONTE_PX, (Color){80, 48, 24, 255});
        n = copiarHistorico(linhas, 6);
        for (i = 0; i < n; i++) {
            char curto[22];
            prefixo(linhas[i], 21, curto, sizeof curto);
            texto(curto, 24.0f, 34.0f + (float)i * 10.0f, FONTE_PX, (Color){40, 32, 24, 255});
        }
        cursor = caso_jogo.salas.inicio;
        i = 0;
        while (cursor != NULL && i < 4) {
            texto(cursor->nome, 24.0f, 100.0f + (float)i * 10.0f, FONTE_PX,
                  cursor->resolvido ? (Color){32, 96, 48, 255} : (Color){120, 48, 32, 255});
            if (cursor == caso_jogo.salas.atual) {
                break;
            }
            cursor = cursor->prox;
            i++;
        }
    }
}

static int ppBase(int indice) {
    const int bases[] = {35, 25, 15, 10};
    if (indice < 0) indice = 0;
    if (indice > 3) indice = 3;
    return bases[indice];
}

static void garantirPp(const Pokemon *pokemon) {
    int slot;
    int i;

    if (pokemon == NULL) {
        return;
    }
    slot = posicaoDoAtivo(&caso_jogo.nucleo.time);
    if (slot < 0 || slot >= 6 || vis.pp_init[slot]) {
        return;
    }
    for (i = 0; i < 4; i++) {
        vis.pp[slot][i] = i < pokemon->qtd_golpes ? ppBase(i) : 0;
    }
    vis.pp_init[slot] = 1;
}

static void animarBarra(float *mostrado, float alvo, float dt, int *queda, float *flash, float *tremor) {
    if (alvo > *mostrado + 0.004f) {
        *mostrado = alvo;
        *queda = 0;
        return;
    }
    if (*mostrado > alvo + 0.004f) {
        if (!*queda) {
            *flash = 0.22f;
            *tremor = 0.32f;
            *queda = 1;
            if (vis.som_ok) {
                PlaySound(vis.som_acerto);
            }
        }
        *mostrado -= dt * 0.7f;
        if (*mostrado < alvo) {
            *mostrado = alvo;
        }
    } else {
        *queda = 0;
    }
}

static void desenharBarraHp(int x, int y, int w, float razao) {
    int cheia = (int)((float)(w - 2) * razao);

    if (cheia < 0) cheia = 0;
    if (cheia > w - 2) cheia = w - 2;
    DrawRectangle(x, y, w, 6, BLACK);
    DrawRectangle(x + 1, y + 1, w - 2, 4, (Color){48, 48, 48, 255});
    if (cheia > 0) {
        DrawRectangle(x + 1, y + 1, cheia, 4, corBarra(razao));
    }
}

static void desenharCaixaHp(const Pokemon *pokemon, float razao, int x, int y, int w, int mostrar_numeros) {
    char nivel[16];
    char pontos[24];
    int atual;

    if (pokemon == NULL) {
        return;
    }
    caixa(x, y, w, mostrar_numeros ? 40 : 32, (Color){248, 248, 240, 255});
    snprintf(nivel, sizeof nivel, "Nv%d", pokemon->nivel);
    textoAte(pokemon->nome, (float)x + 6.0f, (float)y + 4.0f,
             (float)w - larguraTexto(nivel, FONTE_PX) - 16.0f, BLACK);
    texto(nivel, (float)x + (float)w - larguraTexto(nivel, FONTE_PX) - 6.0f, (float)y + 4.0f, FONTE_PX, BLACK);
    texto("HP", (float)x + 6.0f, (float)y + 16.0f, FONTE_PX, (Color){216, 168, 48, 255});
    desenharBarraHp(x + 24, y + 18, w - 32, razao);
    if (mostrar_numeros && pokemon->hp_max > 0) {
        atual = (int)(razao * (float)pokemon->hp_max + 0.5f);
        if (atual < 0) atual = 0;
        if (atual > pokemon->hp_max) atual = pokemon->hp_max;
        snprintf(pontos, sizeof pontos, "%d/%d", atual, pokemon->hp_max);
        texto(pontos, (float)x + (float)w - larguraTexto(pontos, FONTE_PX) - 6.0f,
              (float)y + 26.0f, FONTE_PX, BLACK);
    }
}

static void sairParaMundo(void) {
    tela = TELA_SALA;
    vis.transicao = 0;
    vis.trans_t = 0.0f;
    vis.fase = BAT_ENTRADA;
    vis.resultado = 1;
    vis.captura_feita = 0;
    mundo.estava_encontro = 1;
    mundo.movendo = 0;
    mundo.anda = 1.0f;
}

static void avancarFilaBatalha(void) {
    if (!vis.texto_completo) {
        completarTexto(textoBatalha());
        return;
    }
    if (vis.fila_i + 1 < vis.fila_n) {
        vis.fila_i++;
        vis.bytes = 0;
        vis.texto_completo = 0;
        vis.acum = 0.0f;
        return;
    }
    if (vis.resultado == 0 && caso_jogo.captura_disponivel && !vis.captura_feita) {
        vis.fase = BAT_CAPTURA;
        vis.captura_t = 0.0f;
        return;
    }
    if (vis.resultado == 0 && caso_jogo.caso_encerrado) {
        vis.fase = BAT_FIM;
        vis.vitoria = 1;
        abrirFala("Giovanni recua. O caso está encerrado.");
        vis.texto_completo = 0;
        vis.bytes = 0;
        return;
    }
    if (vis.resultado == 0) {
        sairParaMundo();
        return;
    }
    if (vis.resultado < 0 && !caso_jogo.em_batalha) {
        vis.fase = BAT_FIM;
        vis.vitoria = 0;
        abrirFala("Seu time desmaiou.");
        vis.texto_completo = 0;
        vis.bytes = 0;
        return;
    }
    vis.fase = BAT_COMANDO;
}

static void usarGolpe(int indice) {
    Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
    SaidaBatalha saida;
    int slot;
    int resultado;

    if (eu == NULL || indice < 0 || indice >= eu->qtd_golpes) {
        return;
    }
    garantirPp(eu);
    slot = posicaoDoAtivo(&caso_jogo.nucleo.time);
    if (slot >= 0 && slot < 6 && vis.pp[slot][indice] <= 0) {
        filaLimpar();
        filaPor("Não há PP para esse golpe!");
        vis.fase = BAT_TEXTO;
        vis.resultado = 1;
        return;
    }
    filaLimpar();
    saida.narrar = narrarUi;
    saida.contexto = NULL;
    saida.log = &caso_jogo.nucleo.log;
    resultado = casoExecutarGolpe(&caso_jogo, indice, &saida);
    if (resultado == -2) {
        filaPor("O golpe falhou.");
        resultado = 1;
    }
    if (slot >= 0 && slot < 6 && vis.pp[slot][indice] > 0) {
        vis.pp[slot][indice]--;
    }
    vis.resultado = resultado;
    vis.fase = BAT_TEXTO;
    if (vis.fila_n == 0) {
        filaPor("...");
    }
    clique();
}

static void atualizarBatalha(float dt) {
    const Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
    const Pokemon *op = NULL;
    float alvo_eu = vis.hp_eu;
    float alvo_opp = 0.0f;

    if (caso_jogo.oponente_carregado) {
        op = obterAtivo(&caso_jogo.oponente_atual.time);
    }
    if (eu != NULL && eu->hp_max > 0) {
        alvo_eu = (float)eu->hp_atual / (float)eu->hp_max;
        if (eu->numero != vis.ultimo_eu) {
            vis.ultimo_eu = eu->numero;
            vis.entrada = 0.35f;
        }
    }
    if (op != NULL && op->hp_max > 0) {
        alvo_opp = (float)op->hp_atual / (float)op->hp_max;
        if (op->numero != vis.ultimo_opp) {
            vis.ultimo_opp = op->numero;
            vis.entrada = 0.35f;
        }
    }
    animarBarra(&vis.hp_eu, alvo_eu, dt, &vis.queda_eu, &vis.flash_eu, &vis.tremor_eu);
    animarBarra(&vis.hp_opp, alvo_opp, dt, &vis.queda_opp, &vis.flash_opp, &vis.tremor_opp);
    if (vis.flash_eu > 0.0f) vis.flash_eu -= dt;
    if (vis.flash_opp > 0.0f) vis.flash_opp -= dt;
    if (vis.tremor_eu > 0.0f) vis.tremor_eu -= dt;
    if (vis.tremor_opp > 0.0f) vis.tremor_opp -= dt;
    if (vis.flash_eu < 0.0f) vis.flash_eu = 0.0f;
    if (vis.flash_opp < 0.0f) vis.flash_opp = 0.0f;
    if (vis.entrada < 1.0f) {
        vis.entrada += dt / 0.4f;
        if (vis.entrada > 1.0f) vis.entrada = 1.0f;
    }

    if (vis.fase == BAT_ENTRADA) {
        if (vis.entrada >= 1.0f || acaoPressionada()) {
            vis.entrada = 1.0f;
            vis.fase = BAT_TEXTO;
        }
        return;
    }
    if (vis.fase == BAT_CAPTURA) {
        vis.captura_t += dt;
        if (vis.captura_t > 1.7f && !vis.captura_feita) {
            char msg[LOG_MAX];
            int ok;
            snprintf(msg, sizeof msg, "%s foi capturado!", caso_jogo.captura_pendente.nome);
            ok = casoCapturar(&caso_jogo);
            vis.captura_feita = 1;
            filaLimpar();
            filaPor(ok ? msg : "O time está cheio.");
            vis.resultado = 0;
            vis.fase = BAT_TEXTO;
        }
        return;
    }
    if (vis.fase == BAT_FIM) {
        revelarTexto(dt, vis.fala);
        if (acaoPressionada()) {
            if (!vis.texto_completo) {
                completarTexto(vis.fala);
            } else {
                tela = TELA_MENU;
            }
        }
        return;
    }
    if (vis.fase == BAT_TEXTO) {
        revelarTexto(dt, textoBatalha());
        if (acaoPressionada() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            avancarFilaBatalha();
        }
        return;
    }
    if (vis.fase == BAT_COMANDO) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) vis.cursor = (vis.cursor + 1) % 4;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) vis.cursor = (vis.cursor + 3) % 4;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) vis.cursor = (vis.cursor + 2) % 4;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) vis.cursor = (vis.cursor + 2) % 4;
        if (acaoPressionada()) {
            if (vis.cursor == 0) {
                vis.fase = BAT_GOLPES;
                vis.cursor = 0;
            } else if (vis.cursor == 1) {
                filaLimpar();
                filaPor("A bolsa está vazia.");
                vis.fase = BAT_TEXTO;
                vis.resultado = 1;
            } else if (vis.cursor == 2) {
                vis.fase = BAT_TIME;
                vis.cursor_time = posicaoDoAtivo(&caso_jogo.nucleo.time);
                if (vis.cursor_time < 0) vis.cursor_time = 0;
            } else {
                filaLimpar();
                filaPor("Não dá para fugir!");
                vis.fase = BAT_TEXTO;
                vis.resultado = 1;
            }
            clique();
        }
        return;
    }
    if (vis.fase == BAT_GOLPES) {
        if (eu != NULL) {
            garantirPp(eu);
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) vis.cursor = (vis.cursor % 2 == 0) ? vis.cursor + 1 : vis.cursor - 1;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) vis.cursor = (vis.cursor % 2 == 1) ? vis.cursor - 1 : vis.cursor + 1;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            vis.cursor = (vis.cursor + 2) % 4;
        }
        if (voltarPressionado()) {
            vis.fase = BAT_COMANDO;
            vis.cursor = 0;
            return;
        }
        if (acaoPressionada()) {
            usarGolpe(vis.cursor);
        }
        return;
    }
    if (vis.fase == BAT_TIME) {
        int n = tamanhoTime(&caso_jogo.nucleo.time);
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            if (n > 0) vis.cursor_time = (vis.cursor_time + 1) % n;
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            if (n > 0) vis.cursor_time = (vis.cursor_time + n - 1) % n;
        }
        if (voltarPressionado()) {
            vis.fase = BAT_COMANDO;
            return;
        }
        if (acaoPressionada() && n > 0) {
            const Pokemon *escolhido = pokemonNaPosicao(&caso_jogo.nucleo.time, vis.cursor_time);
            if (escolhido == NULL || escolhido->hp_atual <= 0) {
                filaLimpar();
                filaPor("Este Pokémon desmaiou.");
                vis.fase = BAT_TEXTO;
                vis.resultado = 1;
            } else if (definirAtivoPorPosicao(&caso_jogo.nucleo.time, vis.cursor_time)) {
                char msg[LOG_MAX];
                snprintf(msg, sizeof msg, "Você enviou %s!", escolhido->nome);
                filaLimpar();
                filaPor(msg);
                vis.fase = BAT_TEXTO;
                vis.resultado = 1;
                vis.entrada = 0.4f;
                clique();
            }
        }
    }
}

static void desenharPlataforma(int cx, int cy, float rx, float ry, Color clara, Color escura) {
    DrawEllipse(cx, cy, rx + 4.0f, ry + 3.0f, escura);
    DrawEllipse(cx, cy, rx, ry, clara);
}

static void desenharBatalha(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
    const Pokemon *op = NULL;
    int chefe = sala != NULL && sala->e_chefe;
    int numero_opp = 0;
    float entrada = vis.entrada;
    float deslize = (1.0f - entrada) * 56.0f;
    float sx_eu = 0.0f;
    float sx_opp = 0.0f;
    Rectangle jogador;
    Rectangle inimigo;
    Color ceu;
    Color chao;
    Color plat_c;
    Color plat_e;
    const char *comandos[] = {"LUTAR", "BOLSA", "POKéMON", "FUGIR"};
    int i;

    if (caso_jogo.oponente_carregado) {
        op = obterAtivo(&caso_jogo.oponente_atual.time);
        if (op != NULL) numero_opp = op->numero;
    } else if (caso_jogo.captura_disponivel || vis.fase == BAT_CAPTURA) {
        numero_opp = caso_jogo.captura_pendente.numero;
    }
    if (vis.tremor_eu > 0.0f) sx_eu = sinf(vis.tremor_eu * 90.0f) * 3.0f;
    if (vis.tremor_opp > 0.0f) sx_opp = sinf(vis.tremor_opp * 90.0f) * 3.0f;

    if (chefe) {
        ceu = (Color){120, 36, 40, 255};
        chao = (Color){72, 24, 28, 255};
        plat_c = (Color){150, 64, 64, 255};
        plat_e = (Color){60, 16, 20, 255};
    } else {
        ceu = (Color){176, 216, 168, 255};
        chao = (Color){88, 168, 80, 255};
        plat_c = (Color){128, 196, 96, 255};
        plat_e = (Color){40, 104, 48, 255};
    }
    DrawRectangle(0, 0, VIRT_L, 52, ceu);
    DrawRectangle(0, 52, VIRT_L, 60, chao);
    DrawRectangle(0, 100, VIRT_L, 12, chefe ? (Color){48, 16, 20, 255} : (Color){56, 128, 56, 255});
    desenharPlataforma(168, 58, 42.0f, 12.0f, plat_c, plat_e);
    desenharPlataforma(58, 100, 48.0f, 13.0f, plat_c, plat_e);

    jogador = (Rectangle){6.0f - deslize + sx_eu, 40.0f, 64.0f, 64.0f};
    if (eu != NULL) {
        desenharSpriteMon(eu->numero, 1, jogador, vis.flash_eu);
    }
    if (chefe && giovanni_bat_ok && vis.fase != BAT_CAPTURA) {
        Rectangle g = {176.0f + deslize, 0.0f, 64.0f, 64.0f};
        DrawTexturePro(giovanni_batalha,
                       (Rectangle){0, 0, (float)giovanni_batalha.width, (float)giovanni_batalha.height},
                       g, (Vector2){0, 0}, 0.0f, WHITE);
    }
    inimigo = (Rectangle){148.0f + deslize + sx_opp, 2.0f, 64.0f, 64.0f};
    if (chefe) {
        inimigo.x = 112.0f + deslize + sx_opp;
        inimigo.y = 6.0f;
    }
    if (vis.fase == BAT_CAPTURA && vis.captura_t > 0.55f) {
        numero_opp = 0;
    } else if (vis.fase == BAT_CAPTURA && vis.captura_t > 0.4f) {
        float k = 1.0f - (vis.captura_t - 0.4f) / 0.15f;
        if (k < 0.0f) k = 0.0f;
        inimigo.width *= k;
        inimigo.height *= k;
        inimigo.x += (64.0f - inimigo.width) * 0.5f;
        inimigo.y += 64.0f - inimigo.height;
    }
    if (numero_opp > 0) {
        desenharSpriteMon(numero_opp, 0, inimigo, vis.flash_opp);
    }

    if (op != NULL) {
        desenharCaixaHp(op, vis.hp_opp, 4, 4, 136, 0);
    } else if (vis.fase == BAT_CAPTURA) {
        desenharCaixaHp(&caso_jogo.captura_pendente, vis.hp_opp, 4, 4, 136, 0);
    }
    desenharCaixaHp(eu, vis.hp_eu, 112, 64, 122, 1);

    if (vis.fase == BAT_CAPTURA) {
        float u = vis.captura_t / 0.5f;
        float bx;
        float by;
        float balanco = 0.0f;
        if (u > 1.0f) u = 1.0f;
        bx = 28.0f + (176.0f - 28.0f) * u;
        by = 90.0f + (46.0f - 90.0f) * u - sinf(u * 3.1415926f) * 30.0f;
        if (vis.captura_t > 0.55f) {
            balanco = sinf((vis.captura_t - 0.55f) * 18.0f) * 3.0f;
            bx = 172.0f + balanco;
            by = 40.0f;
        }
        if (pokebola_ok) {
            DrawTexturePro(pokebola_tex,
                           (Rectangle){0, 0, (float)pokebola_tex.width, (float)pokebola_tex.height},
                           (Rectangle){bx, by, 16, 16}, (Vector2){0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangle((int)bx, (int)by, 12, 12, RED);
        }
        caixa(0, 112, VIRT_L, 48, (Color){248, 248, 240, 255});
        texto("A Poké Bola foi lançada!", 8.0f, 128.0f, FONTE_PX, BLACK);
    } else     if (vis.fase == BAT_GOLPES && eu != NULL) {
        int slot = posicaoDoAtivo(&caso_jogo.nucleo.time);
        garantirPp(eu);
        for (i = 0; i < 4; i++) {
            int cx = (i % 2) * 120;
            int cy = 96 + (i / 2) * 32;
            int selecionado = (vis.cursor == i);
            Color fundo = selecionado ? (Color){32, 32, 40, 255} : (Color){248, 248, 240, 255};
            Color tinta = selecionado ? (Color){248, 248, 240, 255} : BLACK;
            caixa(cx, cy, 120, 32, fundo);
            if (i < eu->qtd_golpes) {
                char pp[16];
                int atual = (slot >= 0 && slot < 6) ? vis.pp[slot][i] : ppBase(i);
                textoAte(eu->golpes[i].nome, (float)cx + 4.0f, (float)cy + 3.0f, 110.0f, tinta);
                DrawRectangle(cx + 4, cy + 18, 6, 6, corTipo(eu->golpes[i].tipo));
                textoAte(nomeTipo(eu->golpes[i].tipo), (float)cx + 12.0f, (float)cy + 16.0f, 64.0f, tinta);
                snprintf(pp, sizeof pp, "%d/%d", atual, ppBase(i));
                texto(pp, (float)cx + 120.0f - larguraTexto(pp, FONTE_PX) - 4.0f, (float)cy + 16.0f, FONTE_PX, tinta);
            } else {
                texto("-", (float)cx + 8.0f, (float)cy + 8.0f, FONTE_PX, tinta);
            }
        }
    } else if (vis.fase == BAT_COMANDO) {
        char pergunta[80];
        char linhas_p[3][48];
        int n_p = 0;
        int k;
        if (eu != NULL) {
            snprintf(pergunta, sizeof pergunta, "O que %s fará?", eu->nome);
        } else {
            copiarTexto(pergunta, sizeof pergunta, "O que fará?");
        }
        caixa(0, 112, 120, 48, (Color){248, 248, 240, 255});
        quebrar(pergunta, linhas_p, &n_p, 3, 108.0f);
        for (k = 0; k < n_p; k++) {
            texto(linhas_p[k], 6.0f, 118.0f + (float)k * 10.0f, FONTE_PX, BLACK);
        }
        for (i = 0; i < 4; i++) {
            int coluna = i % 2;
            int cx = 120 + coluna * 64;
            int cy = 112 + (i / 2) * 24;
            int largura = coluna == 0 ? 64 : 56;
            int selecionado = vis.cursor == i;
            caixa(cx, cy, largura, 24, selecionado ? (Color){32, 32, 40, 255} : (Color){248, 248, 240, 255});
            textoAte(comandos[i], (float)cx + 4.0f, (float)cy + 8.0f, (float)largura - 8.0f,
                     selecionado ? (Color){248, 248, 240, 255} : BLACK);
        }
    } else if (vis.fase == BAT_FIM) {
        desenharMensagem(vis.fala, 0, 112, VIRT_L, 48);
    } else {
        desenharMensagem(textoBatalha(), 0, 112, VIRT_L, 48);
    }

    if (vis.fase == BAT_TIME) {
        int n = tamanhoTime(&caso_jogo.nucleo.time);
        caixa(28, 16, 184, 128, (Color){248, 248, 240, 255});
        texto("POKéMON", 36.0f, 22.0f, FONTE_PX, BLACK);
        for (i = 0; i < n && i < 4; i++) {
            const Pokemon *membro = pokemonNaPosicao(&caso_jogo.nucleo.time, i);
            float razao = 0.0f;
            Color tinta = (vis.cursor_time == i) ? (Color){180, 32, 32, 255} : BLACK;
            if (membro == NULL) continue;
            if (membro->hp_max > 0) razao = (float)membro->hp_atual / (float)membro->hp_max;
            texto(membro->nome, 36.0f, 40.0f + (float)i * 24.0f, FONTE_PX, tinta);
            desenharBarraHp(120, 42 + i * 24, 80, razao);
        }
    }
}

static Rectangle botaoMenu(int indice) {
    return (Rectangle){118.0f, 46.0f + (float)indice * 22.0f, 110.0f, 18.0f};
}

static void desenharMenu(void) {
    const char *rotulos[] = {"NOVO JOGO", "COMO JOGAR", "POKEDEX", "SAIR"};
    int i;

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){16, 28, 72, 255});
    DrawRectangle(0, 96, VIRT_L, 64, (Color){12, 48, 36, 255});
    desenharPlataforma(48, 108, 36.0f, 10.0f, (Color){72, 140, 64, 255}, (Color){24, 64, 32, 255});
    desenharSpriteMon(25, 0, (Rectangle){16, 40, 64, 64}, 0.0f);
    {
        const char *titulo = "PokéBattle";
        const char *subtitulo = "O Caso do Laboratório";
        float lt = larguraTexto(titulo, 16.0f);
        float ls = larguraTexto(subtitulo, FONTE_PX);
        if (lt > 224.0f) lt = 224.0f;
        if (ls > 224.0f) ls = 224.0f;
        texto(titulo, (240.0f - lt) * 0.5f, 6.0f, 16.0f, (Color){255, 214, 64, 255});
        texto(subtitulo, (240.0f - ls) * 0.5f, 26.0f, FONTE_PX, (Color){220, 220, 210, 255});
    }
    for (i = 0; i < 4; i++) {
        Rectangle b = botaoMenu(i);
        int selecionado = vis.menu_cursor == i;
        caixa((int)b.x, (int)b.y, (int)b.width, (int)b.height,
              selecionado ? (Color){255, 214, 64, 255} : (Color){248, 248, 240, 255});
        texto(rotulos[i], b.x + 6.0f, b.y + 5.0f, FONTE_PX, BLACK);
    }
}

static void desenharAjuda(void) {
    caixa(8, 8, 224, 144, (Color){248, 248, 240, 255});
    texto("COMO JOGAR", 16.0f, 16.0f, FONTE_PX, BLACK);
    texto("Setas ou WASD andam", 16.0f, 36.0f, FONTE_PX, BLACK);
    texto("uma célula por vez.", 16.0f, 48.0f, FONTE_PX, BLACK);
    texto("Z interage. N caderno.", 16.0f, 64.0f, FONTE_PX, BLACK);
    texto("Na luta: LUTAR, BOLSA,", 16.0f, 84.0f, FONTE_PX, BLACK);
    texto("POKéMON e FUGIR.", 16.0f, 96.0f, FONTE_PX, BLACK);
    texto("X volta. Esc menu.", 16.0f, 116.0f, FONTE_PX, BLACK);
    texto("Z para fechar", 16.0f, 136.0f, FONTE_PX, (Color){80, 80, 80, 255});
}

static void visitarPk(const Pokemon *pokemon, void *contexto) {
    ColetaPk *coleta = contexto;
    if (pokemon != NULL && coleta->n < 32) {
        coleta->itens[coleta->n++] = pokemon;
    }
}

static void desenharPokedex(void) {
    ColetaPk coleta;
    int i;
    int fim;

    memset(&coleta, 0, sizeof coleta);
    percorrerEmOrdem(&caso_jogo.nucleo.arvore, visitarPk, &coleta);
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){20, 28, 48, 255});
    caixa(8, 8, 224, 144, (Color){248, 248, 240, 255});
    texto("POKEDEX", 16.0f, 14.0f, FONTE_PX, BLACK);
    if (vis.pokedex_topo < 0) vis.pokedex_topo = 0;
    if (vis.pokedex_topo > coleta.n - 1 && coleta.n > 0) vis.pokedex_topo = coleta.n - 1;
    fim = vis.pokedex_topo + 5;
    if (fim > coleta.n) fim = coleta.n;
    for (i = vis.pokedex_topo; i < fim; i++) {
        const Pokemon *pokemon = coleta.itens[i];
        char linha[64];
        int y = 32 + (i - vis.pokedex_topo) * 22;
        if (pokemon == NULL) continue;
        if (sprites[pokemon->numero].tem_icone) {
            DrawTexturePro(sprites[pokemon->numero].icone,
                           (Rectangle){0, 0, (float)sprites[pokemon->numero].icone.width,
                                       (float)sprites[pokemon->numero].icone.height},
                           (Rectangle){16, (float)y - 2, 20, 16}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        snprintf(linha, sizeof linha, "%03d %s", pokemon->numero, pokemon->nome);
        texto(linha, 40.0f, (float)y, FONTE_PX, BLACK);
    }
}

static void desenharTudo(void) {
    BeginTextureMode(alvo);
    ClearBackground(BLACK);
    if (tela == TELA_MENU) desenharMenu();
    else if (tela == TELA_AJUDA) desenharAjuda();
    else if (tela == TELA_POKEDEX) desenharPokedex();
    else if (tela == TELA_SALA) desenharMundo();
    else desenharBatalha();
    if (vis.transicao != 0) {
        int altura = (int)(vis.trans_t * (float)(VIRT_A / 2));
        if (vis.transicao == 1 && vis.trans_t < 0.18f) {
            unsigned char alfa = (unsigned char)(255.0f * (1.0f - vis.trans_t / 0.18f));
            DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){255, 255, 255, alfa});
        }
        DrawRectangle(0, 0, VIRT_L, altura, BLACK);
        DrawRectangle(0, VIRT_A - altura, VIRT_L, altura, BLACK);
    }
    EndTextureMode();
}

static void apresentar(const char *png) {
    Rectangle origem = {0.0f, 0.0f, (float)VIRT_L, -(float)VIRT_A};
    Rectangle destino = {0.0f, 0.0f, (float)JANELA_L, (float)JANELA_A};

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(alvo.texture, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
    if (png != NULL) {
        TakeScreenshot(png);
    }
}

static void atualizarTransicao(float dt) {
    if (vis.transicao == 1) {
        vis.trans_t += dt / 0.42f;
        if (vis.trans_t >= 1.0f) {
            vis.trans_t = 1.0f;
            vis.transicao = 2;
            tela = TELA_BATALHA;
            prepararBatalha();
        }
    } else if (vis.transicao == 2) {
        vis.trans_t -= dt / 0.32f;
        if (vis.trans_t <= 0.0f) {
            vis.trans_t = 0.0f;
            vis.transicao = 0;
        }
    }
}

static int pontoNoRetangulo(Rectangle r, Vector2 p) {
    return p.x >= r.x && p.y >= r.y && p.x < r.x + r.width && p.y < r.y + r.height;
}

static void atualizarMenu(void) {
    Vector2 mouse = {(float)GetMouseX() / (float)ESCALA, (float)GetMouseY() / (float)ESCALA};
    int i;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) vis.menu_cursor = (vis.menu_cursor + 1) % 4;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) vis.menu_cursor = (vis.menu_cursor + 3) % 4;
    for (i = 0; i < 4; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && pontoNoRetangulo(botaoMenu(i), mouse)) {
            vis.menu_cursor = i;
            break;
        }
    }
    if (IsKeyPressed(KEY_ONE)) vis.menu_cursor = 0;
    if (IsKeyPressed(KEY_TWO)) vis.menu_cursor = 1;
    if (IsKeyPressed(KEY_THREE)) vis.menu_cursor = 2;
    if (IsKeyPressed(KEY_FOUR)) vis.menu_cursor = 3;

    if (acaoPressionada() || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && i < 4 && pontoNoRetangulo(botaoMenu(vis.menu_cursor), mouse))) {
        clique();
        if (vis.menu_cursor == 0) {
            if (comecarCaso(&caso_jogo)) {
                zerarVisual();
                entrarNaSala();
                tela = TELA_SALA;
            }
        } else if (vis.menu_cursor == 1) {
            tela = TELA_AJUDA;
        } else if (vis.menu_cursor == 2) {
            tela = TELA_POKEDEX;
        } else {
            tela = TELA_MENU;
            /* Sair é tratado no laço, olhando o cursor. */
            vis.vitoria = -1;
        }
    }
}

static void atualizar(float dt) {
    if (dt > 0.05f) dt = 0.05f;
    vis.tempo += dt;
    atualizarTransicao(dt);
    if (vis.transicao == 1) {
        return;
    }
    if (tela == TELA_MENU) {
        atualizarMenu();
    } else if (tela == TELA_AJUDA || tela == TELA_POKEDEX) {
        if (tela == TELA_POKEDEX) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) vis.pokedex_topo++;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) vis.pokedex_topo--;
        }
        if (acaoPressionada() || IsKeyPressed(KEY_ESCAPE) || voltarPressionado()) {
            tela = TELA_MENU;
        }
    } else if (tela == TELA_SALA) {
        if (IsKeyPressed(KEY_ESCAPE) && !vis.fala_aberta && !vis.caderno) {
            tela = TELA_MENU;
            return;
        }
        atualizarMundo(dt);
    } else if (tela == TELA_BATALHA) {
        atualizarBatalha(dt);
    }
}

int executarInterfaceRaylib(void) {
    if (!abrirGrafico()) {
        fprintf(stderr, "Não foi possível abrir a janela.\n");
        return 1;
    }
    if (!iniciarCaso(&caso_jogo)) {
        fecharGrafico();
        return 1;
    }
    caso_ok = 1;
    zerarVisual();
    tela = TELA_MENU;

    while (!WindowShouldClose() && vis.vitoria != -1) {
        atualizar(GetFrameTime());
        desenharTudo();
        apresentar(NULL);
    }

    encerrarCaso(&caso_jogo);
    caso_ok = 0;
    fecharGrafico();
    return 0;
}

static int lutarAteVencer(void) {
    int luta = 1;
    int golpes = 0;

    if (!caso_jogo.em_batalha && !casoIniciarBatalha(&caso_jogo)) {
        return 0;
    }
    while (luta == 1 && golpes < 60) {
        const Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
        const Pokemon *op = obterAtivo(&caso_jogo.oponente_atual.time);
        int golpe;

        if (eu == NULL || op == NULL) {
            return 0;
        }
        golpe = escolherGolpeAdversario(eu, op);
        luta = casoExecutarGolpe(&caso_jogo, golpe, NULL);
        golpes++;
    }
    if (luta != 0) {
        return 0;
    }
    return 1;
}

static int irAteOChefe(void) {
    int passos = 0;

    while (passos < 16) {
        NoSala *sala = salaAtual(&caso_jogo.salas);

        if (sala == NULL) {
            return 0;
        }
        if (strcmp(sala->nome, "Sala do chefe") == 0) {
            return casoIniciarBatalha(&caso_jogo);
        }
        if (!sala->resolvido && !casoUsarHabilidade(&caso_jogo)) {
            return 0;
        }
        if (sala->inicia_batalha && !sala->batalha_concluida) {
            if (!lutarAteVencer()) {
                return 0;
            }
            if (caso_jogo.captura_disponivel && !casoCapturar(&caso_jogo)) {
                return 0;
            }
        }
        if (!casoAvancar(&caso_jogo)) {
            return 0;
        }
        passos++;
    }
    return 0;
}

int capturarTelasDemonstracao(void) {
    NoSala *sala;

    if (!abrirGrafico()) {
        return 1;
    }
    if (!iniciarCaso(&caso_jogo)) {
        fecharGrafico();
        return 1;
    }
    caso_ok = 1;
    zerarVisual();
    tela = TELA_MENU;
    desenharTudo();
    apresentar("captura_menu.png");

    if (!comecarCaso(&caso_jogo)) {
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    zerarVisual();
    entrarNaSala();
    tela = TELA_SALA;
    desenharTudo();
    apresentar("captura_sala.png");

    sala = salaAtual(&caso_jogo.salas);
    if (sala != NULL) {
        const char **mapa = mapaDa(sala->nome);
        int x;
        int y;
        for (y = 0; y < MAPA_A; y++) {
            for (x = 0; x < MAPA_L; x++) {
                if (tileEm(mapa, x, y) == 'P' && y + 2 < MAPA_A - 1) {
                    mundo.tx = x;
                    mundo.ty = y + 2;
                    mundo.de_x = mundo.tx;
                    mundo.de_y = mundo.ty;
                    mundo.para_x = mundo.tx;
                    mundo.para_y = mundo.ty;
                    mundo.dir = 3;
                    mundo.pika_x = mundo.tx + 1;
                    mundo.pika_y = mundo.ty;
                    mundo.pika_de_x = mundo.pika_x;
                    mundo.pika_de_y = mundo.pika_y;
                }
            }
        }
        abrirFala(sala->texto_puzzle);
    }
    vis.texto_completo = 1;
    vis.bytes = 400;
    desenharTudo();
    apresentar("captura_puzzle.png");
    vis.fala_aberta = 0;

    if (!casoUsarHabilidade(&caso_jogo) || !casoAvancar(&caso_jogo)) {
        fprintf(stderr, "Não chegou ao pátio para a batalha.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    entrarNaSala();
    if (!casoIniciarBatalha(&caso_jogo)) {
        fprintf(stderr, "A batalha do pátio não começou.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    tela = TELA_BATALHA;
    prepararBatalha();
    vis.entrada = 1.0f;
    vis.fase = BAT_COMANDO;
    vis.transicao = 0;
    desenharTudo();
    apresentar("captura_batalha.png");

    if (!lutarAteVencer()) {
        fprintf(stderr, "A batalha de demonstração não terminou em vitória.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    vis.fase = BAT_CAPTURA;
    vis.captura_t = 0.25f;
    vis.entrada = 1.0f;
    vis.hp_opp = 0.0f;
    vis.transicao = 0;
    desenharTudo();
    apresentar("captura_captura.png");

    if (caso_jogo.captura_disponivel) {
        casoCapturar(&caso_jogo);
    }
    if (!irAteOChefe()) {
        fprintf(stderr, "Não chegou na luta do Giovanni.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    tela = TELA_BATALHA;
    prepararBatalha();
    vis.entrada = 1.0f;
    vis.fase = BAT_TEXTO;
    vis.texto_completo = 1;
    vis.bytes = 400;
    vis.transicao = 0;
    desenharTudo();
    apresentar("captura_boss.png");

    encerrarCaso(&caso_jogo);
    caso_ok = 0;
    fecharGrafico();
    return 0;
}
