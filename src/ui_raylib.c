/* Janela do jogo */

#include "ui_raylib.h"

#include "caso.h"
#include "historia.h"
#include "simulacao.h"
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
    TELA_TITULO = 0,
    TELA_MENU,
    TELA_INTRO,
    TELA_AJUSTES,
    TELA_PAUSA,
    TELA_AJUDA,
    TELA_POKEDEX,
    TELA_SALA,
    TELA_BATALHA,
    TELA_CREDITOS,
    TELA_GAMEOVER,
    TELA_MEWTWO
} Tela;

typedef enum {
    MENU_INICIAR,
    MENU_AJUDA,
    MENU_POKEDEX,
    MENU_AJUSTES,
    MENU_CREDITOS,
    MENU_SAIR,
    MENU_TOTAL
} OpcaoMenu;

typedef enum {
    INTRO_ALERTA,
    INTRO_RADIO,
    INTRO_TANQUE,
    INTRO_ENTRADA,
    INTRO_TOTAL
} CenaIntro;

static const float duracao_intro[INTRO_TOTAL] = {5.0f, 6.0f, 4.5f, 5.0f};

static const Color fundo_caso = {10, 17, 31, 255};
static const Color painel_caso = {19, 32, 52, 255};
static const Color borda_caso = {61, 82, 110, 255};
static const Color texto_caso = {234, 239, 255, 255};
static const Color texto_secundario = {159, 181, 207, 255};
static const Color destaque_caso = {255, 215, 104, 255};
static const Color psiquico_caso = {173, 121, 241, 255};

typedef enum {
    BAT_ENTRADA = 0,
    BAT_ANALISE,
    BAT_TEXTO,
    BAT_COMANDO,
    BAT_GOLPES,
    BAT_TIME,
    BAT_BOLSA,
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
    int lanterna_ligada; /* Só o desenho; a luz vem do puzzle */
    int documento_aberto;
    int documento_pagina;
    int puzzle_aberto;
    int cursor_puzzle;
    float retorno_puzzle;
    char aviso_puzzle[64];
    int time_aberto;
    float retorno_captura;
    float captura_animacao;
    int batalha_treinador;
    char paginas[6][TEXTO_SALA_MAX];
    int pagina_n;
    int pagina_i;
    int intro_feita;
    CenaIntro cena_intro;
    float tempo_intro;
    float tempo_menu;
    float entrada_setor;
    float abertura_caderno;
    int dica_inicio;
    int pausa_cursor;
    int ajustes_cursor;
    int confirmar_menu;
    int salas_vistas[8];
    int depois_fala;
    int cursor_bolsa;
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
    int minimapa_aberto;
    int pokedex_aberta;
    int pokedex_sel;
    int pokedex_veio_de_onde;
    int pc_aberto;
    int pc_aba;
    int pc_sel_time;
    int pc_sel_reserva;
    int pc_modo_troca;
    int pc_troca_origem_aba;
    int pc_troca_origem_pos;
    char pc_aviso[64];
    int mewtwo_ativo;
    int mewtwo_etapa;
    int mewtwo_opcao;
    int mewtwo_convencido;
    float mewtwo_t;
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
    int passos_area;
    int ultimo_passo_x, ultimo_passo_y;
    int secreta;
    char sala[NOME_SALA_MAX];
} Mundo;

typedef struct {
    const Pokemon *itens[151];
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
static Texture2D frlg_tex;
static Texture2D pikachu_ow;
static Texture2D costas_tex;
static Texture2D teclas_tex;
static Texture2D guardas_tex;
static int teclas_ok = 0;
static int guardas_ok = 0;
static int detetive_ok = 0;
static int giovanni_ow_ok = 0;
static int giovanni_bat_ok = 0;
static int pokebola_ok = 0;
static int frlg_ok = 0;
static int pikachu_ow_ok = 0;
static int costas_ok = 0;
static SpriteMon sprites[MAX_POKEMON];
static char raiz_assets[256];
static Caso caso_jogo;
static int caso_ok = 0;
static Visual vis;
static Mundo mundo;
static Tela tela = TELA_TITULO;
static Tela tela_pausada;
static Tela retorno_ajustes;
static int volume = 80;
static int movimento_reduzido;
static Sound som_radio;
static Sound som_porta;

static void desenharPokedex(void);
static void desenharMinimapaModal(void);
static void desenharPcEquipe(void);
static void atualizarPcEquipe(void);
static Color corDoTipo(Tipo tipo);
static void desenharBarraHp(int x, int y, int w, float razao);
static void visitarPk(const Pokemon *pokemon, void *contexto);

static Mapa MAPA_LAB = {
    "####################",
    "#RSCC........BBBB.C#",
    "#M................M#",
    "#M.P..............M#",
    "#..................#",
    "#..MM........MM....#",
    "#..MM..,,,,..MM....#",
    "#......,,,,........#",
    "#................*.#",
    "#M................M#",
    "#V................V#",
    "#..................#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_PATIO = {
    "####################",
    "#BB.........C..*.BB#",
    "#B.gggg......rrr..B#",
    "#..gggg......rrr...#",
    "#..gggg............#",
    "#.........E........#",
    "#..s...............#",
    "#..MM........MM....#",
    "#..MM........MM....#",
    "#..................#",
    "#V..cc......cc....V#",
    "#...bb......bb.....#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_FRIA = {
    "####################",
    "#M................M#",
    "#,,,,,,,,,,,,,,,,,,#",
    "#,l.MM......MM...o,#",
    "#,..MM............,#",
    "#,.......cc...*...,#",
    "#,.b..............,#",
    "#,................,#",
    "#,,,,,,,,,,,,,,,,,,#",
    "#.......IIII.......#",
    "#........PP........#",
    "#........I..n......#",
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
    "#..gggg.......ggg..#",
    "#..gggg.......ggg..#",
    "#MC...............M#",
    "#...cc.......cc..*.#",
    "#...bb......bb.....#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_OFICINA = {
    "####################",
    "#C................C#",
    "#.lMM........MM..o.#",
    "#M................M#",
    "#..rrr........rrr..#",
    "#..rrr........rrr..#",
    "#......cc..........#",
    "#..bb.........*....#",
    "#.......FFFF.......#",
    "#........FF........#",
    "#........F.........#",
    "#...........n......#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_JAULA = {
    "####################",
    "#HHHHHHHHHHHHHHHHHH#",
    "#H..MM........MM..H#",
    "#H.l............o.H#",
    "#H.......E........H#",
    "#H..............*.H#",
    "#HHHHHHH..HHHHHHHHH#",
    "#..rrr........rrr..#",
    "#..rrr........rrr..#",
    "#M.......P........M#",
    "#........c.........#",
    "#..bb..........bb..#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_ARQUIVO = {
    "####################",
    "#BBBBBBBBBBBBBBBBBB#",
    "#Bk.BB.......BB..CB#",
    "#B..BB.......BB...B#",
    "#B.l............o.B#",
    "#B................S#",
    "#B.*..............B#",
    "#..MM........MM....#",
    "#..cc........cc....#",
    "#M................M#",
    "#..b.....BB........#",
    "#........P.........#",
    "#........D.........#",
    "####################"
};

static Mapa MAPA_SECRETA = {
    "####################",
    "#..F....m......M...#",
    "#..#....#.#....#...#",
    "#..#.....p.....#...#",
    "#..###.......###...#",
    "#....gggg..gggg....#",
    "#....gggg..gggg....#",
    "#..###.......###...#",
    "#..T...C........*..#",
    "#..................#",
    "#..MM..........MM..#",
    "#..................#",
    "#.........D........#",
    "####################"
};

static int idItemDaSala(const NoSala *sala) {
    if (mundo.secreta) return 7;
    if (sala == NULL) return 0;
    if (strcmp(sala->nome, "Laboratório") == 0) return 0;
    if (strcmp(sala->nome, "Pátio") == 0) return 1;
    if (strcmp(sala->nome, "Câmara fria") == 0) return 2;
    if (strcmp(sala->nome, "Tanque") == 0) return 3;
    if (strcmp(sala->nome, "Oficina") == 0) return 4;
    if (strcmp(sala->nome, "Jaula") == 0) return 5;
    if (strcmp(sala->nome, "Arquivo") == 0) return 6;
    return -1;
}

static Mapa MAPA_CHEFE = {
    "####################",
    "#M................M#",
    "#........G.........#",
    "#...BB.......BB....#",
    "#..................#",
    "#..C............C..#",
    "#...cc.......cc....#",
    "#...MM.......MM....#",
    "#..................#",
    "#M................M#",
    "#..................#",
    "#..................#",
    "#........D.........#",
    "####################"
};

/* Mapa do laboratório */
static const unsigned short LAB_META[14][13] = {
    {669, 109, 109, 110, 105, 105, 653, 653, 152, 107, 108, 107, 108},
    {685, 678, 117, 118, 133, 134, 661, 661, 160, 115, 116, 115, 116},
    {693, 694, 645, 646, 645, 646, 641, 641, 641, 643, 644, 643, 644},
    {671, 675, 676, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649},
    {671, 683, 684, 649, 649, 649, 649, 649, 680, 681, 682, 649, 649},
    {648, 691, 692, 649, 649, 649, 649, 649, 688, 689, 690, 649, 649},
    {648, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649},
    {698, 651, 652, 651, 652, 649, 649, 649, 651, 652, 651, 652, 652},
    {115, 115, 116, 115, 116, 649, 649, 649, 115, 116, 115, 116, 116},
    {647, 643, 644, 643, 644, 649, 649, 649, 643, 644, 643, 644, 644},
    {648, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649},
    {659, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 649, 660},
    {667, 649, 649, 649, 649, 656, 657, 658, 649, 649, 649, 649, 668},
    {8, 8, 8, 8, 8, 664, 665, 666, 8, 8, 8, 8, 8}
};

static const char *LAB_COL[14] = {
    "#############",
    "#############",
    ".............",
    "#...c........",
    "#PP.....###..",
    ".PP..........",
    ".......c.....",
    ".............",
    "#####...#####",
    ".............",
    ".l.........o.",
    ".............",
    "#...........#",
    "#####DDD#####"
};

static void texto(const char *conteudo, float x, float y, float tamanho, Color cor);
static int avancarPonto(const char *texto, int indice);
static const char **mapaDa(const char *nome);
static char tileEm(const char **mapa, int x, int y);
static int solido(char simbolo, const NoSala *sala);
static void entrarNaSala(void);
static void abrirFala(const char *texto);
static void tentarBatalha(void);
static void prepararBatalha(void);
static void desenharTudo(void);
static void apresentar(const char *png);
static int abrirGrafico(int tela_cheia);
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
    int n;

    memset(sprites, 0, sizeof sprites);
    for (n = 1; n < MAX_POKEMON; n++) {
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
    pikachu_ow = carregarPng("personagens/pikachu_overworld.png", &pikachu_ow_ok);
    costas_tex = carregarPng("personagens/detetive_costas.png", &costas_ok);
    frlg_tex = carregarPng("tiles/frlg_lab.png", &frlg_ok);
    teclas_tex = carregarPng("ui/teclas.png", &teclas_ok);
    guardas_tex = carregarPng("ui/guardas.png", &guardas_ok);
    carregarSpritesPokemon();
    InitAudioDevice();
    if (IsAudioDeviceReady()) {
        vis.som_clique = criarTom(660.0f, 0.04f);
        vis.som_acerto = criarTom(180.0f, 0.08f);
        som_radio = criarTom(290.0f, 0.15f);
        som_porta = criarTom(87.0f, 0.26f);
        SetMasterVolume(volume / 100.0f);
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
    liberarTextura(&pikachu_ow, &pikachu_ow_ok);
    liberarTextura(&costas_tex, &costas_ok);
    liberarTextura(&frlg_tex, &frlg_ok);
    liberarTextura(&teclas_tex, &teclas_ok);
    liberarTextura(&guardas_tex, &guardas_ok);
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
        UnloadSound(som_radio);
        UnloadSound(som_porta);
        vis.som_ok = 0;
    }
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
}

static int abrirGrafico(int tela_cheia) {
    if (grafico_ok) {
        return 1;
    }
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(JANELA_L, JANELA_A, "PokéBattle: O Caso do Laboratório");
    if (!IsWindowReady()) {
        return 0;
    }
    SetExitKey(KEY_NULL);
    SetWindowMinSize(VIRT_L, VIRT_A);
    if (tela_cheia) {
        ToggleBorderlessWindowed();
    }
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
    if (maximo <= 0.0f) return;
    copiarTexto(buf, sizeof buf, origem == NULL ? "" : origem);
    while (buf[0] != '\0' && larguraTexto(buf, FONTE_PX) > maximo) {
        int fim = 0;
        int anterior = 0;
        while (buf[fim] != '\0') {
            anterior = fim;
            fim = avancarPonto(buf, fim);
        }
        buf[anterior] = '\0';
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

static void nomeNaFicha(const char *nome, float x, float y, float maximo, Color cor) {
    int tamanho;
    char buf[64];

    if (nome == NULL) {
        nome = "";
    }
    for (tamanho = FONTE_PX; tamanho >= 6; tamanho--) {
        if (larguraTexto(nome, (float)tamanho) <= maximo) {
            texto(nome, x, y, (float)tamanho, cor);
            return;
        }
    }
    copiarTexto(buf, sizeof buf, nome);
    while (buf[0] != '\0' && larguraTexto(buf, 6.0f) > maximo) {
        int fim = 0;
        int anterior = 0;
        while (buf[fim] != '\0') {
            anterior = fim;
            fim = avancarPonto(buf, fim);
        }
        if (anterior <= 0) {
            break;
        }
        buf[anterior] = '\0';
    }
    texto(buf, x, y, 6.0f, cor);
}

static void caixa(int x, int y, int w, int h, Color miolo) {
    DrawRectangle(x, y, w, h, BLACK);
    if (w > 4 && h > 4) {
        DrawRectangle(x + 2, y + 2, w - 4, h - 4, miolo);
        if (h >= 20) DrawRectangle(x + 2, y + h - 4, w - 4, 2, (Color){180, 180, 170, 255});
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
    quebrar(visivel, linhas, &n, 4, (float)w - 20.0f);
    for (i = 0; i < n; i++) {
        texto(linhas[i], (float)x + 6.0f, (float)y + 6.0f + (float)i * 10.0f, FONTE_PX, BLACK);
    }
    if (vis.texto_completo && (movimento_reduzido || ((int)(vis.tempo * 3.0f) % 2) == 0)) {
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

static void mostrarFala(const char *mensagem) {
    copiarTexto(vis.fala, sizeof vis.fala, mensagem == NULL ? "" : mensagem);
    vis.fala_aberta = 1;
    vis.bytes = 0;
    vis.texto_completo = 0;
    vis.acum = 0.0f;
}

static void abrirFala(const char *mensagem) {
    char linhas[24][48];
    int quantidade;

    vis.pagina_n = 0;
    vis.pagina_i = 0;
    quebrar(mensagem, linhas, &quantidade, 24, VIRT_L - 20);
    if (quantidade <= 4) {
        mostrarFala(mensagem);
        return;
    }
    for (int i = 0; i < quantidade; i += 4) {
        char *pagina = vis.paginas[vis.pagina_n++];
        int usadas = 0;
        pagina[0] = '\0';
        for (int j = i; j < quantidade && j < i + 4; j++) {
            usadas += snprintf(pagina + usadas, TEXTO_SALA_MAX - usadas, "%s%s",
                               j == i ? "" : " ", linhas[j]);
        }
    }
    mostrarFala(vis.paginas[0]);
}

static void abrirPaginas(const char *(*obter)(int)) {
    vis.pagina_n = 0;
    vis.pagina_i = 0;
    if (obter == NULL) {
        return;
    }
    while (vis.pagina_n < 6) {
        const char *texto = obter(vis.pagina_n);
        if (texto == NULL) {
            break;
        }
        copiarTexto(vis.paginas[vis.pagina_n], TEXTO_SALA_MAX, texto);
        vis.pagina_n++;
    }
    if (vis.pagina_n > 0) {
        mostrarFala(vis.paginas[0]);
    }
}

static int indiceDaSala(const NoSala *sala) {
    const NoSala *cursor;
    int i = 0;

    if (sala == NULL) {
        return -1;
    }
    for (cursor = caso_jogo.salas.inicio; cursor != NULL && i < 8; cursor = cursor->prox, i++) {
        if (cursor == sala) {
            return i;
        }
    }
    return -1;
}

static void narrarSalaAtual(void) {
    NoSala *sala = salaAtual(&caso_jogo.salas);
    const char *fala;
    int indice;

    if (sala == NULL || !vis.intro_feita || vis.fala_aberta) {
        return;
    }
    indice = indiceDaSala(sala);
    if (indice < 0 || vis.salas_vistas[indice]) {
        return;
    }
    fala = historiaAoEntrar(sala->nome);
    if (fala == NULL) {
        return;
    }
    vis.salas_vistas[indice] = 1;
    abrirFala(fala);
}

static void aoFecharFala(void) {
    int depois = vis.depois_fala;

    vis.fala_aberta = 0;
    vis.pagina_n = 0;
    vis.depois_fala = 0;
    if (depois == 1) {
        tentarBatalha();
        return;
    }
    if (depois == 2) {
        tela = TELA_CREDITOS;
        vis.menu_cursor = 0;
        return;
    }
    if (depois == 3) {
        tela = TELA_GAMEOVER;
        vis.menu_cursor = 0;
        return;
    }
    if (depois == 4) {
        tela = TELA_MEWTWO;
        vis.mewtwo_ativo = 1;
        vis.mewtwo_etapa = 0;
        vis.mewtwo_opcao = 0;
        vis.mewtwo_convencido = 0;
        vis.mewtwo_t = 0.0f;
        return;
    }
    narrarSalaAtual();
}

static void confirmarFala(void) {
    if (!vis.texto_completo) {
        completarTexto(vis.fala);
        return;
    }
    if (vis.pagina_i + 1 < vis.pagina_n) {
        vis.pagina_i++;
        mostrarFala(vis.paginas[vis.pagina_i]);
        return;
    }
    aoFecharFala();
}

static const char **mapaDa(const char *nome) {
    if (mundo.secreta) return MAPA_SECRETA;
    if (nome == NULL) {
        return MAPA_LAB;
    }
    if (strcmp(nome, "Laboratório") == 0) return LAB_COL;
    if (strcmp(nome, "Pátio") == 0) return MAPA_PATIO;
    if (strcmp(nome, "Câmara fria") == 0) return MAPA_FRIA;
    if (strcmp(nome, "Tanque") == 0) return MAPA_TANQUE;
    if (strcmp(nome, "Oficina") == 0) return MAPA_OFICINA;
    if (strcmp(nome, "Jaula") == 0) return MAPA_JAULA;
    if (strcmp(nome, "Arquivo") == 0) return MAPA_ARQUIVO;
    if (strcmp(nome, "Sala do chefe") == 0) return MAPA_CHEFE;
    return MAPA_LAB;
}

static int larguraMapa(const char **mapa) {
    if (mapa == LAB_COL) {
        return 13;
    }
    return MAPA_L;
}

static int alturaMapa(const char **mapa) {
    if (mapa == LAB_COL) {
        return 14;
    }
    return MAPA_A;
}

static char tileEm(const char **mapa, int x, int y) {
    if (mapa == NULL || y < 0 || y >= alturaMapa(mapa) || x < 0 || x >= larguraMapa(mapa)) {
        return '#';
    }
    if (mapa[y] == NULL || (int)strlen(mapa[y]) <= x) {
        return '#';
    }
    return mapa[y][x];
}

static int solido(char simbolo, const NoSala *sala) {
    if (simbolo == '*') {
        int id = idItemDaSala(sala);
        return !casoItemJaColetado(&caso_jogo, id);
    }
    if (simbolo == '#' || simbolo == 'M' || simbolo == 'C' || simbolo == 'B' ||
        simbolo == 'R' || simbolo == 'S' || simbolo == 'V' || simbolo == 'P' ||
        simbolo == 'H' || simbolo == '~' || simbolo == 'D' ||
        simbolo == 'c' || simbolo == 'b' || simbolo == 's' ||
        simbolo == 'l' || simbolo == 'o' || simbolo == 'n' ||
        simbolo == 'k') {
        return 1;
    }
    if (mundo.secreta && (simbolo == 'F' || simbolo == 'T' || simbolo == 'p' || simbolo == 'm')) {
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
    if (!sala->resolvido || !sala->prox->desbloqueada ||
        (sala->luzes & sala->luzes_necessarias) != sala->luzes_necessarias ||
        (sala->duelo_obrigatorio && !sala->duelo_concluido)) {
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
    if (mapa == LAB_COL) { mundo.tx = 6; mundo.ty = 6; }
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
    vis.fala_aberta = 0;
    vis.caderno = 0;
    vis.puzzle_aberto = 0;
    vis.documento_aberto = 0;
    vis.time_aberto = 0;
    mundo.passos_area = 0;
    mundo.ultimo_passo_x = mundo.tx;
    mundo.ultimo_passo_y = mundo.ty;
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
            abrirFala(casoObjetivoAtual(&caso_jogo));
        }
        return;
    }
    if (vis.som_ok) PlaySound(som_porta);
    entrarNaSala();
    vis.entrada_setor = movimento_reduzido ? 0.0f : 0.36f;
    narrarSalaAtual();
}

static void comecarTransicao(void) {
    vis.transicao = 1;
    vis.trans_t = 0.0f;
}

static void prepararBatalha(void) {
    const Pokemon *eu = obterAtivo(&caso_jogo.nucleo.time);
    const Pokemon *op = obterAtivo(&caso_jogo.oponente_atual.time);

    filaLimpar();
    vis.fase = BAT_ANALISE;
    vis.entrada = 0.0f;
    vis.cursor = 0;
    vis.captura_t = 0.0f;
    vis.captura_feita = 0;
    vis.retorno_captura = 0.0f;
    vis.captura_animacao = 0.0f;
    vis.batalha_treinador = caso_jogo.batalha_npc;
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
        abrirFala(sala->resolvido ? sala->texto_resolvido :
                  "Agora examine a mesa com o Alakazam. Use Z diante dela.");
        return;
    }
    if (!casoIniciarBatalha(&caso_jogo)) {
        abrirFala("Não há luta aqui.");
        return;
    }
    clique();
    comecarTransicao();
}

static void abrirDocumento(void) {
    if (casoPaginaDocumento(&caso_jogo, 0) != NULL) {
        vis.documento_aberto = 1;
        vis.documento_pagina = 0;
        vis.caderno = 0;
        vis.fala_aberta = 0;
        clique();
    }
}

static void tentarTreino(int explicar) {
    if (casoIniciarTreino(&caso_jogo)) {
        /* No banco o PP volta ao máximo */
        clique();
        comecarTransicao();
    } else if (explicar) {
        abrirFala("Matos e escombros escondem selvagens. Primeiro conclua a captura desta sala; depois ande na área ou use Z para treinar.");
    }
}

static void tentarLendario(int numero_lendario) {
    if (casoIniciarEncontroLendario(&caso_jogo, numero_lendario)) {
        clique();
        comecarTransicao();
    }
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
        if (strcmp(sala->nome, "Jaula") == 0) abrirDocumento();
        else abrirFala(sala->texto_resolvido);
        return;
    }
    if (sala->inicia_batalha && !sala->batalha_concluida) {
        abrirFala("Primeiro, ajude o Alakazam e capture-o.");
        return;
    }
    if (!timeTemHabilidade(&caso_jogo.nucleo.time, sala->habilidade)) {
        abrirFala("Nenhum Pokémon do time resolve isto.");
        return;
    }
    vis.dica_inicio = 0;
    vis.puzzle_aberto = 1;
    vis.cursor_puzzle = sala->desafio.tipo == DES_GELO ? 4 : 0;
    vis.retorno_puzzle = 0.0f;
    copiarTexto(vis.aviso_puzzle, sizeof vis.aviso_puzzle, "Explore e use a habilidade.");
    clique();
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

static void posicionarMundo(int x, int y) {
    mundo.tx = x;
    mundo.ty = y;
    mundo.de_x = mundo.para_x = x;
    mundo.de_y = mundo.para_y = y;
    mundo.pika_x = mundo.pika_de_x = x;
    mundo.pika_y = mundo.pika_de_y = y + 1;
    mundo.anda = 1.0f;
    mundo.pika_anda = 1.0f;
    mundo.movendo = 0;
    mundo.dir = 0;
    mundo.pika_dir = 0;
    mundo.passos_area = 0;
    mundo.estava_encontro = 0;
    mundo.ultimo_passo_x = x;
    mundo.ultimo_passo_y = y;
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
        if (mundo.secreta) {
            mundo.secreta = 0;
            casoSairAreaSecreta(&caso_jogo);
            posicionarMundo(17, 5);
            return;
        }
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
    if (a_frente == 'C') {
        vis.pc_aberto = 1;
        vis.pc_aba = 0;
        vis.pc_sel_time = 0;
        vis.pc_sel_reserva = 0;
        vis.pc_modo_troca = 0;
        copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Terminal de Equipe ativo.");
        clique();
        return;
    }
    if (a_frente == 'l' || a_frente == 'o') {
        if (casoAcenderPonto(&caso_jogo, a_frente == 'l' ? 0 : 1)) clique();
        else abrirFala("O refletor precisa do gerador do laboratório. Reconecte o circuito com Pikachu primeiro.");
        return;
    }
    if (a_frente == 'n') {
        if (casoIniciarDuelo(&caso_jogo)) { clique(); comecarTransicao(); }
        else abrirFala(sala != NULL && sala->duelo_concluido ? "O guarda recua. Passagem liberada." :
                       "O guarda espera: restaure os refletores e resolva o obstáculo deste setor antes do duelo.");
        return;
    }
    if (a_frente == 'b') {
        if (casoDescansar(&caso_jogo)) {
            memset(vis.pp_init, 0, sizeof vis.pp_init);
            abrirFala("Uma pausa no banco. HP e PP recuperados! T abre o time para escolher quem vai treinar.");
        }
        return;
    }
    if (a_frente == 's') {
        abrirFala("ÁREA DE TREINO. Ande nos matos ou escombros, ou use Z sobre eles. Vitória: 30 XP para cada parceiro. 60 XP sobem um nível. Bancos recuperam HP e PP.");
        return;
    }
    if (a_frente == 'k') {
        if (!casoTemChaveSecreta(&caso_jogo)) {
            casoObterChaveSecreta(&caso_jogo);
            abrirFala("ARQUIVOS CONFIDENCIAIS: Você encontrou a [Chave do Santuário] oculta atrás dos registros do Projeto Mew!");
            clique();
        } else {
            abrirFala("O cofre dos arquivos confidenciais já foi examinado.");
        }
        return;
    }
    if (a_frente == '*' || aqui == '*') {
        int id = idItemDaSala(sala);
        if (!casoItemJaColetado(&caso_jogo, id)) {
            char msg[128];
            if (casoColetarItemMapa(&caso_jogo, id, msg, sizeof msg)) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala(msg);
                clique();
            }
        } else {
            abrirFala("Este compartimento de itens já foi recolhido.");
        }
        return;
    }
    if (a_frente == 'D') {
        if (mundo.secreta) {
            mundo.secreta = 0;
            casoSairAreaSecreta(&caso_jogo);
            posicionarMundo(17, 5);
            clique();
            abrirFala("Você retornou ao Arquivo.");
            return;
        }
        tentarPorta();
        return;
    }
    if (a_frente == 'S' && sala != NULL && strcmp(sala->nome, "Arquivo") == 0) {
        if (!casoTemChaveSecreta(&caso_jogo)) {
            abrirFala("A porta na parede lateral leste está trancada por uma fechadura com o selo de Mew. Encontre a Chave nos arquivos!");
            return;
        }
        if (casoAbrirAreaSecreta(&caso_jogo)) {
            mundo.secreta = 1;
            posicionarMundo(9, 11);
            clique();
            abrirFala("SANTUÁRIO OCULTO: A porta lateral se abriu! Três altares elementais guardam o pedestal. Ative-os com o Pokémon correto!");
        } else {
            abrirFala("A passagem secreta está bloqueada. Resolva o Arquivo primeiro.");
        }
        return;
    }
    if (a_frente == 'm' && mundo.secreta) {
        abrirFala("MURAL ANCESTRAL: 'Traga Charmander, Alakazam e Pikachu aos altares. Quando acesos, toque-os com [Z] para invocar as Aves Lendárias, e o Pedestal Sagrado para invocar Mew e Mewtwo!'");
        return;
    }
    if (a_frente == 'F' && mundo.secreta) {
        if (caso_jogo.altares_secretos & 1) {
            tentarLendario(146);
            abrirFala("O Altar do Fogo queima em chamas azuis! O guardião lendário MOLTRES desperta do vulcão sagrado!");
        } else {
            int res = casoAtivarAltarElemental(&caso_jogo, 0);
            if (res == 2) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Charmander acendeu o Altar do Fogo! Todos os 3 altares estão ativos! O pedestal central liberou a MASTER BALL! Toque no altar novamente para invocar Moltres!");
            } else if (res == 1) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Charmander canalizou suas chamas! O Altar do Fogo foi ativado! Toque-o novamente para invocar Moltres!");
            } else {
                abrirFala("O Altar do Fogo está frio. Exige o poder do tipo FOGO! Use 'T' para colocar Charmander como ativo.");
            }
        }
        return;
    }
    if (a_frente == 'M' && mundo.secreta) {
        if (caso_jogo.altares_secretos & 2) {
            tentarLendario(144);
            abrirFala("O Altar Psíquico sintoniza frequências cósmicas! O guardião lendário ARTICUNO desce dos céus gelados!");
        } else {
            int res = casoAtivarAltarElemental(&caso_jogo, 1);
            if (res == 2) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Alakazam sintonizou o Altar Psíquico! Todos os 3 altares estão ativos! O pedestal central liberou a MASTER BALL! Toque no altar novamente para invocar Articuno!");
            } else if (res == 1) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Alakazam canalizou ondas mentais! O Altar Psíquico foi ativado! Toque-o novamente para invocar Articuno!");
            } else {
                abrirFala("O Altar Psíquico não reage. Exige o poder mental de um tipo PSÍQUICO! Use 'T' para colocar Alakazam como ativo.");
            }
        }
        return;
    }
    if (a_frente == 'T' && mundo.secreta) {
        if (caso_jogo.altares_secretos & 4) {
            tentarLendario(145);
            abrirFala("O Altar do Trovão reluz faíscas douradas! O guardião lendário ZAPDOS desce em meio a relâmpagos!");
        } else {
            int res = casoAtivarAltarElemental(&caso_jogo, 2);
            if (res == 2) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Pikachu eletrificou o Altar do Trovão! Todos os 3 altares estão ativos! O pedestal central liberou a MASTER BALL! Toque no altar novamente para invocar Zapdos!");
            } else if (res == 1) {
                if (vis.som_ok) PlaySound(vis.som_acerto);
                abrirFala("Pikachu disparou raios dourados! O Altar do Trovão foi ativado! Toque-o novamente para invocar Zapdos!");
            } else {
                abrirFala("O Altar do Trovão está descarregado. Exige eletricidade de um tipo ELÉTRICO! Use 'T' para colocar Pikachu como ativo.");
            }
        }
        return;
    }
    if (a_frente == 'p' && mundo.secreta) {
        if (caso_jogo.altares_secretos == 7 || caso_jogo.master_bolas > 0) {
            static int alternar_mito = 0;
            int num = (alternar_mito++ % 2 == 0) ? 151 : 150;
            tentarLendario(num);
            if (num == 151) {
                abrirFala("PEDESTAL SAGRADO: Uma luz etérea resplandece... O mítico MEW surge diante de você!");
            } else {
                abrirFala("PEDESTAL SAGRADO: Ondas psíquicas colossais emergem do núcleo... MEWTWO materializou-se!");
            }
        } else {
            abrirFala("O Pedestal Sagrado está protegido por um campo de força. Ative os 3 altares elementais (Fogo, Mente e Trovão)!");
        }
        return;
    }
    if (a_frente >= '1' && a_frente <= '3' && mundo.secreta) {
        int resultado = casoAtivarSeloSecreto(&caso_jogo, a_frente - '1');
        if (resultado == 2) abrirFala("Os altares se iluminam! Você encontrou uma Master Ball!");
        else if (resultado == 1) abrirFala("Altar ativado com o parceiro correto!");
        else abrirFala("Este altar exige o tipo elemental correto! Use 'T' para trocar o Pokémon ativo.");
        return;
    }
    if (a_frente == 'E' || a_frente == 'G' || aqui == 'E' || aqui == 'G') {
        tentarBatalha();
        return;
    }
    if (a_frente == 'P' || a_frente == 'I' || a_frente == 'F') {
        tentarPuzzle(a_frente);
        return;
    }
    if (a_frente == 'g' || a_frente == 'r' || aqui == 'g' || aqui == 'r') tentarTreino(1);
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

static void atualizarPuzzle(float dt) {
    NoSala *sala = caso_jogo.salas.atual;
    Desafio *d;
    AcaoDesafio acao = ACAO_USAR;
    int enviar = 0, dx = 0, dy = 0, resultado;
    int colunas, total;
    if (vis.retorno_puzzle > 0) vis.retorno_puzzle -= dt;
    if (sala == NULL) { vis.puzzle_aberto = 0; return; }
    d = &sala->desafio;
    if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) { vis.puzzle_aberto = 0; return; }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) dx = 1;
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) dx = -1;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) dy = 1;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) dy = -1;
    if (IsKeyPressed(KEY_R)) { acao = ACAO_REINICIAR; enviar = 1; }
    else if (IsKeyPressed(KEY_U) && d->tipo == DES_ESTANTES) { acao = ACAO_DESFAZER; enviar = 1; }
    else if (d->tipo == DES_ESTANTES && (dx || dy)) {
        acao = dx > 0 ? ACAO_DIREITA : dx < 0 ? ACAO_ESQUERDA : dy > 0 ? ACAO_BAIXO : ACAO_CIMA;
        enviar = 1;
    } else if (d->tipo == DES_GELO && IsKeyPressed(KEY_E)) { acao = ACAO_RESFRIAR; enviar = 1; }
    else {
        colunas = d->tipo == DES_CIRCUITO ? 3 : d->tipo == DES_GELO ? 4 : d->tipo == DES_RASTRO ? 5 : 1;
        total = d->tipo == DES_CIRCUITO ? 9 : d->tipo == DES_GELO ? 12 : d->tipo == DES_RASTRO ? 20 : 4;
        if (dx && colunas > 1 && vis.cursor_puzzle % colunas + dx >= 0 &&
            vis.cursor_puzzle % colunas + dx < colunas) vis.cursor_puzzle += dx;
        if (dy && vis.cursor_puzzle + dy * colunas >= 0 && vis.cursor_puzzle + dy * colunas < total)
            vis.cursor_puzzle += dy * colunas;
        if (acaoPressionada()) enviar = 1;
    }
    if (!enviar) return;
    resultado = casoComandarPuzzle(&caso_jogo, acao, vis.cursor_puzzle);
    clique();
    if (resultado == 2) {
        const char *fala = historiaAoResolver(sala->nome);
        vis.puzzle_aberto = 0;
        if (d->tipo == DES_RASTRO) abrirDocumento();
        else abrirFala(fala != NULL ? fala : sala->texto_resolvido);
    } else {
        copiarTexto(vis.aviso_puzzle, sizeof vis.aviso_puzzle, resultado < 0 ? "Ação indisponível." :
                    d->tipo == DES_GELO ? "Vapor! Resfrie com E." :
                    d->tipo == DES_AGUA ? "Dosagem errada. Recomece." :
                    d->tipo == DES_RASTRO ? "Siga o eco mais forte." : "Bloqueado. U desfaz.");
        vis.retorno_puzzle = resultado <= 0 ? 1.2f : 0;
    }
}

static void atualizarMundo(float dt) {
    NoSala *sala;

    if (vis.transicao != 0) {
        return;
    }
    sala = salaAtual(&caso_jogo.salas);
    if (sala != NULL && strcmp(sala->nome, mundo.sala) != 0) {
        entrarNaSala();
        narrarSalaAtual();
    }
    if (IsKeyPressed(KEY_L)) {
        vis.lanterna_ligada = !vis.lanterna_ligada;
        if (vis.dica_inicio == 1) vis.dica_inicio = 2;
        clique();
    }
    if (vis.minimapa_aberto) {
        if (IsKeyPressed(KEY_M) || voltarPressionado() || IsKeyPressed(KEY_ESCAPE) || acaoPressionada()) {
            vis.minimapa_aberto = 0;
            clique();
        }
        return;
    }
    if (vis.pokedex_aberta) {
        ColetaPk coleta;
        memset(&coleta, 0, sizeof coleta);
        percorrerEmOrdem(&caso_jogo.nucleo.arvore, visitarPk, &coleta);
        if (coleta.n > 0) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                vis.pokedex_sel = (vis.pokedex_sel + 1) % coleta.n;
                clique();
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                vis.pokedex_sel = (vis.pokedex_sel + coleta.n - 1) % coleta.n;
                clique();
            }
        }
        if (IsKeyPressed(KEY_P) || voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) {
            vis.pokedex_aberta = 0;
            clique();
        }
        return;
    }
    if (vis.documento_aberto) {
        if (IsKeyPressed(KEY_RIGHT) || acaoPressionada()) {
            if (vis.documento_pagina == 0) vis.documento_pagina = 1;
            else vis.documento_aberto = 0;
            clique();
        } else if (IsKeyPressed(KEY_LEFT)) vis.documento_pagina = 0;
        else if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_R)) vis.documento_aberto = 0;
        return;
    }
    if (vis.puzzle_aberto) {
        atualizarPuzzle(dt);
        return;
    }
    if (vis.time_aberto) {
        int n = tamanhoTime(&caso_jogo.nucleo.time);
        if (n > 0 && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))) vis.cursor_time = (vis.cursor_time + 1) % n;
        if (n > 0 && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) vis.cursor_time = (vis.cursor_time + n - 1) % n;
        if (acaoPressionada() && n > 0) {
            const Pokemon *escolhido = pokemonNaPosicao(&caso_jogo.nucleo.time, vis.cursor_time);
            if (escolhido != NULL && escolhido->hp_atual > 0) {
                definirAtivoPorPosicao(&caso_jogo.nucleo.time, vis.cursor_time);
                vis.time_aberto = 0;
                clique();
            }
        }
        if (IsKeyPressed(KEY_T) || voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) vis.time_aberto = 0;
        return;
    }
    if (vis.pc_aberto) {
        atualizarPcEquipe();
        return;
    }
    if (vis.fala_aberta) {
        revelarTexto(dt, vis.fala);
        if (acaoPressionada() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            confirmarFala();
        }
        return;
    }
    if (vis.caderno) {
        if (IsKeyPressed(KEY_R)) { abrirDocumento(); return; }
        if (acaoPressionada() || voltarPressionado() || IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
            vis.caderno = 0;
        }
        return;
    }
    if (IsKeyPressed(KEY_P)) {
        vis.pokedex_aberta = 1;
        vis.pokedex_sel = 0;
        clique();
        return;
    }
    if (IsKeyPressed(KEY_M)) {
        vis.minimapa_aberto = 1;
        clique();
        return;
    }
    if (IsKeyPressed(KEY_N)) {
        vis.caderno = 1;
        vis.abertura_caderno = movimento_reduzido ? 0.0f : 0.18f;
        return;
    }
    if (IsKeyPressed(KEY_R)) { abrirDocumento(); return; }
    if (IsKeyPressed(KEY_T)) {
        vis.time_aberto = 1;
        vis.cursor_time = posicaoDoAtivo(&caso_jogo.nucleo.time);
        return;
    }
    if (IsKeyPressed(KEY_V) && casoVoltar(&caso_jogo)) {
        entrarNaSala();
        vis.entrada_setor = movimento_reduzido ? 0.0f : 0.36f;
        narrarSalaAtual();
        return;
    }
    if (acaoPressionada()) {
        interagir();
        return;
    }
    lerMovimento(dt);
    verificarEncontro();
    if (!mundo.movendo && !vis.transicao && !vis.fala_aberta &&
        (mundo.tx != mundo.ultimo_passo_x || mundo.ty != mundo.ultimo_passo_y)) {
        const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
        char simbolo = tileEm(mapa, mundo.tx, mundo.ty);
        mundo.ultimo_passo_x = mundo.tx;
        mundo.ultimo_passo_y = mundo.ty;
        if (simbolo == 'g' || simbolo == 'r') {
            if (++mundo.passos_area >= 4) { mundo.passos_area = 0; tentarTreino(0); }
        } else mundo.passos_area = 0;
    }
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

static void desenharSpriteMonSilhueta(int numero, Rectangle destino) {
    SpriteMon *sp;
    Texture2D tex;
    int ok;
    Rectangle origem;

    if (numero <= 0 || numero >= MAX_POKEMON) {
        DrawRectangleRec(destino, (Color){30, 48, 36, 255});
        return;
    }
    sp = &sprites[numero];
    if (sp->tem_frente) {
        tex = sp->frente;
        ok = 1;
    } else {
        ok = 0;
        memset(&tex, 0, sizeof tex);
    }
    if (!ok) {
        DrawRectangleRec(destino, (Color){30, 48, 36, 255});
        return;
    }
    origem = (Rectangle){0.0f, 0.0f, (float)tex.width, (float)tex.height};
    DrawTexturePro(tex, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, (Color){24, 40, 30, 255});
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

static void desenharPikachuOw(int dir, int quadro, float x, float y) {
    Rectangle origem;
    Rectangle destino;

    if (dir < 0) dir = 0;
    if (dir > 3) dir = 0;
    if (quadro < 0) quadro = 0;
    if (quadro > 2) quadro = 2;
    if (!pikachu_ow_ok) {
        desenharSpriteMon(25, 0, (Rectangle){x, y, 32.0f, 32.0f}, 0.0f);
        return;
    }
    origem = (Rectangle){(float)quadro * 32.0f, (float)dir * 32.0f, 32.0f, 32.0f};
    destino = (Rectangle){x, y, 32.0f, 32.0f};
    DrawTexturePro(pikachu_ow, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void desenharCostas(int quadro, float x, float y) {
    Rectangle origem;
    Rectangle destino;

    if (quadro < 0) quadro = 0;
    if (quadro > 4) quadro = 4;
    if (!costas_ok) {
        return;
    }
    origem = (Rectangle){(float)quadro * 64.0f, 0.0f, 64.0f, 64.0f};
    destino = (Rectangle){x, y, 64.0f, 64.0f};
    DrawTexturePro(costas_tex, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void blitFrlg(int id, int x, int y) {
    Rectangle origem;
    Rectangle destino;
    int coluna;
    int linha;

    if (!frlg_ok || id < 0) {
        DrawRectangle(x, y, TILE, TILE, (Color){180, 180, 188, 255});
        return;
    }
    coluna = id % 32;
    linha = id / 32;
    origem = (Rectangle){(float)coluna * 16.0f, (float)linha * 16.0f, 16.0f, 16.0f};
    destino = (Rectangle){(float)x, (float)y, 16.0f, 16.0f};
    DrawTexturePro(frlg_tex, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void desenharTecla(int indice, int x, int y) {
    static const char *nomes[] = {"Z", "X", "E", "L", "U", "R", "^", ">", "v", "<", "T", "V"};
    if (indice < 0 || indice >= 12) return;
    if (teclas_ok) {
        DrawTexturePro(teclas_tex, (Rectangle){indice * 16.0f, 0, 16, 16},
                       (Rectangle){(float)x, (float)y, 16, 16}, (Vector2){0, 0}, 0, WHITE);
    } else {
        DrawRectangle(x, y, 16, 16, (Color){248, 240, 208, 255});
        texto(nomes[indice], x + 4.0f, y + 4.0f, FONTE_PX, BLACK);
    }
}

static void desenharGuarda(int variante, float x, float y, int escala) {
    if (guardas_ok) {
        DrawTexturePro(guardas_tex, (Rectangle){variante * 16.0f, 0, 16, 24},
                       (Rectangle){x, y, 16.0f * escala, 24.0f * escala}, (Vector2){0, 0}, 0, WHITE);
    } else DrawRectangle((int)x, (int)y, 16 * escala, 24 * escala, (Color){160, 48, 64, 255});
}

static void desenharDetalhe(char simbolo, int x, int y) {
    int i;
    if (simbolo == 'l' || simbolo == 'o') {
        const NoSala *sala = caso_jogo.salas.atual;
        int aceso = sala != NULL && (sala->luzes & (simbolo == 'l' ? 1U : 2U));
        DrawRectangle(x + 5, y + 6, 6, 10, (Color){64, 72, 88, 255});
        DrawRectangle(x + 1, y + 1, 14, 7, (Color){32, 40, 56, 255});
        DrawRectangle(x + 3, y + 2, 10, 4, aceso ? (Color){255, 240, 160, 255} : (Color){112, 88, 64, 255});
    } else if (simbolo == 'g') {
        DrawRectangle(x, y, 16, 16, (Color){68, 112, 56, 255});
        for (i = 0; i < 4; i++) {
            int dx = x + 2 + i * 4;
            DrawRectangle(dx, y + 5 + i % 2 * 4, 2, 6, (Color){136, 188, 72, 255});
            DrawRectangle(dx - 1, y + 7 + i % 2 * 4, 4, 2, (Color){40, 88, 40, 255});
        }
    } else if (simbolo == 'r') {
        DrawRectangle(x + 2, y + 9, 12, 5, (Color){80, 72, 72, 255});
        DrawRectangle(x + 1, y + 7, 6, 5, (Color){152, 140, 120, 255});
        DrawRectangle(x + 8, y + 4, 6, 7, (Color){120, 112, 104, 255});
        DrawRectangle(x + 3, y + 6, 3, 2, (Color){196, 184, 160, 255});
        DrawRectangle(x + 9, y + 3, 4, 2, (Color){196, 184, 160, 255});
    } else if (simbolo == 'c') {
        DrawRectangle(x + 4, y + 3, 8, 5, (Color){64, 88, 120, 255});
        DrawRectangle(x + 3, y + 8, 10, 5, (Color){104, 144, 168, 255});
        DrawRectangle(x + 4, y + 13, 2, 3, (Color){48, 48, 64, 255});
        DrawRectangle(x + 10, y + 13, 2, 3, (Color){48, 48, 64, 255});
    } else if (simbolo == 'b') {
        DrawRectangle(x + 1, y + 2, 14, 5, (Color){88, 56, 40, 255});
        DrawRectangle(x + 1, y + 8, 14, 4, (Color){184, 128, 64, 255});
        DrawRectangle(x + 2, y + 12, 2, 3, (Color){56, 48, 40, 255});
        DrawRectangle(x + 12, y + 12, 2, 3, (Color){56, 48, 40, 255});
    } else if (simbolo == 's') {
        DrawRectangle(x + 7, y + 6, 2, 10, (Color){88, 56, 40, 255});
        DrawRectangle(x + 1, y + 1, 14, 9, (Color){240, 216, 144, 255});
        DrawRectangle(x + 4, y + 4, 8, 2, (Color){72, 88, 48, 255});
    } else if (simbolo == 'k') {
        DrawRectangle(x + 2, y + 2, 12, 12, (Color){64, 72, 88, 255});
        DrawRectangle(x + 3, y + 3, 10, 10, (Color){96, 104, 120, 255});
        DrawRectangle(x + 6, y + 6, 4, 4, (Color){255, 214, 64, 255});
        DrawRectangle(x + 7, y + 8, 2, 3, (Color){40, 40, 48, 255});
    } else if (simbolo == 'F' && mundo.secreta) {
        int ativo = caso_jogo.altares_secretos & 1;
        DrawRectangle(x + 2, y + 6, 12, 10, (Color){88, 56, 48, 255});
        DrawRectangle(x + 4, y + 8, 8, 7, (Color){120, 72, 60, 255});
        if (ativo) {
            DrawCircle(x + 8, y + 5, 5, (Color){64, 160, 240, 220});
            DrawCircle(x + 8, y + 4, 3, (Color){180, 230, 255, 255});
        } else {
            DrawRectangle(x + 5, y + 4, 6, 3, (Color){72, 64, 64, 255});
        }
    } else if (simbolo == 'M' && mundo.secreta) {
        int ativo = caso_jogo.altares_secretos & 2;
        DrawRectangle(x + 4, y + 4, 8, 12, (Color){60, 56, 80, 255});
        DrawRectangle(x + 5, y + 6, 6, 8, (Color){92, 80, 120, 255});
        if (ativo) {
            DrawCircle(x + 8, y + 3, 4, (Color){200, 80, 240, 240});
            DrawCircle(x + 8, y + 3, 2, WHITE);
        } else {
            DrawRectangle(x + 6, y + 2, 4, 4, (Color){100, 90, 110, 255});
        }
    } else if (simbolo == 'T' && mundo.secreta) {
        int ativo = caso_jogo.altares_secretos & 4;
        DrawRectangle(x + 3, y + 5, 10, 11, (Color){72, 72, 60, 255});
        DrawRectangle(x + 5, y + 7, 6, 8, (Color){110, 104, 70, 255});
        if (ativo) {
            DrawCircle(x + 8, y + 4, 5, (Color){255, 230, 60, 240});
            DrawCircle(x + 8, y + 4, 2, WHITE);
        } else {
            DrawRectangle(x + 6, y + 3, 4, 3, (Color){90, 88, 70, 255});
        }
    } else if (simbolo == 'p' && mundo.secreta) {
        int liberado = (caso_jogo.altares_secretos == 7);
        DrawRectangle(x + 1, y + 8, 14, 8, (Color){96, 96, 120, 255});
        DrawRectangle(x + 3, y + 4, 10, 5, (Color){140, 140, 168, 255});
        if (liberado && caso_jogo.master_bolas > 0) {
            DrawCircle(x + 8, y + 3, 4, (Color){160, 48, 200, 255});
            DrawCircle(x + 8, y + 3, 2, (Color){240, 80, 240, 255});
        }
    } else if (simbolo == 'm') {
        DrawRectangle(x + 1, y + 2, 14, 12, (Color){110, 100, 88, 255});
        DrawRectangle(x + 3, y + 4, 10, 8, (Color){160, 150, 130, 255});
        DrawLine(x + 4, y + 6, x + 11, y + 6, (Color){80, 70, 60, 255});
        DrawLine(x + 4, y + 9, x + 9, y + 9, (Color){80, 70, 60, 255});
    } else if (simbolo == 'S') {
        DrawRectangle(x + 2, y + 1, 12, 14, (Color){48, 56, 72, 255});
        DrawRectangle(x + 4, y + 3, 8, 12, casoTemChaveSecreta(&caso_jogo) ? (Color){120, 80, 160, 255} : (Color){24, 28, 40, 255});
        if (!casoTemChaveSecreta(&caso_jogo)) {
            DrawRectangle(x + 7, y + 8, 2, 4, (Color){255, 214, 64, 255});
        }
    } else if (simbolo == '*') {
        const NoSala *sala = caso_jogo.salas.atual;
        int id = idItemDaSala(sala);
        if (!casoItemJaColetado(&caso_jogo, id)) {
            float bob = sinf(vis.tempo * 4.0f) * 1.5f;
            DrawEllipse(x + 8, y + 14, 5.0f, 2.0f, (Color){0, 0, 0, 80});
            DrawCircle(x + 8, (int)(y + 7 + bob), 5.0f, (Color){220, 44, 44, 255});
            DrawRectangle(x + 3, (int)(y + 7 + bob), 11, 5, WHITE);
            DrawRectangle(x + 3, (int)(y + 6 + bob), 11, 2, BLACK);
            DrawCircle(x + 8, (int)(y + 7 + bob), 2.0f, WHITE);
            DrawCircleLines(x + 8, (int)(y + 7 + bob), 5.0f, (Color){40, 40, 40, 255});
            DrawCircle(x + 7, (int)(y + 5 + bob), 1.0f, WHITE);
        }
    }
}

static void desenharMapa(const NoSala *sala, int camx, int camy) {
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    const char *nome = sala == NULL ? "" : sala->nome;
    int limite_x = larguraMapa(mapa);
    int limite_y = alturaMapa(mapa);
    int x0 = camx / TILE - 1;
    int y0 = camy / TILE - 1;
    int x1 = (camx + VIRT_L) / TILE + 2;
    int y1 = (camy + VIRT_A) / TILE + 2;
    int x;
    int y;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > limite_x) x1 = limite_x;
    if (y1 > limite_y) y1 = limite_y;

    if (mapa == LAB_COL && frlg_ok) {
        for (y = y0; y < y1; y++) {
            for (x = x0; x < x1; x++) {
                blitFrlg(LAB_META[y][x], x * TILE - camx, y * TILE - camy);
                desenharDetalhe(tileEm(mapa, x, y), x * TILE - camx, y * TILE - camy);
            }
        }
        return;
    }

    for (y = y0; y < y1; y++) {
        for (x = x0; x < x1; x++) {
            char simbolo = tileEm(mapa, x, y);
            int px = x * TILE - camx;
            int py = y * TILE - camy;
            int aberto = portaAberta(sala);

            if (frlg_ok) {
                blitFrlg(649, px, py);
                if (simbolo == '#') {
                    blitFrlg(109, px, py);
                } else if (simbolo == 'M' || simbolo == 'B' || (simbolo == 'P' && strcmp(nome, "Arquivo") == 0)) {
                    blitFrlg(107, px, py);
                } else if (simbolo == 'C' || simbolo == 'R' || simbolo == 'S' || simbolo == 'P') {
                    blitFrlg(117, px, py);
                } else if (simbolo == 'V') {
                    blitFrlg(667, px, py);
                } else if (simbolo == 'D') {
                    blitFrlg(aberto ? 665 : 664, px, py);
                }
            } else if (simbolo == ',') {
                blitTile(24, 6, px, py);
            } else if (simbolo != '#') {
                blitTile(24, 0, px, py);
            }
            if (!frlg_ok && simbolo == '#') {
                blitTile(0, 13, px, py);
            } else if (!frlg_ok && simbolo == 'M') {
                blitTile(2, 13, px, py);
            } else if (!frlg_ok && (simbolo == 'C' || simbolo == 'R')) {
                blitTile(22, 8, px, py);
            } else if (!frlg_ok && (simbolo == 'B' || (simbolo == 'P' && strcmp(nome, "Arquivo") == 0))) {
                blitTile(5, 13, px, py);
            } else if (!frlg_ok && (simbolo == 'S' || simbolo == 'P')) {
                blitTile(16, 14, px, py);
            } else if (!frlg_ok && simbolo == 'V') {
                blitTile(16, 0, px, py);
            } else if (!frlg_ok && simbolo == 'D') {
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
            } else if (mundo.secreta && simbolo >= '1' && simbolo <= '3') {
                Color selo = simbolo == '1' ? (Color){80, 160, 240, 255} :
                             simbolo == '2' ? (Color){240, 120, 80, 255} : (Color){120, 216, 144, 255};
                DrawRectangle(px + 2, py + 2, 12, 12, (Color){24, 32, 48, 255});
                DrawRectangleLines(px + 2, py + 2, 12, 12, selo);
                texto((char[]){simbolo, '\0'}, px + 6, py + 4, FONTE_PX, selo);
            } else if (mundo.secreta && simbolo == 'm') {
                DrawRectangle(px + 1, py + 1, 14, 14, (Color){144, 96, 48, 255});
                DrawRectangle(px + 3, py + 3, 10, 10, (Color){248, 224, 136, 255});
                texto("?", px + 5, py + 4, FONTE_PX, (Color){72, 48, 32, 255});
            }
            if (simbolo == 'S' && strcmp(nome, "Arquivo") == 0) {
                Color brilho = sala != NULL && sala->resolvido ? (Color){96, 224, 216, 255} : (Color){80, 88, 104, 255};
                DrawRectangle(px + 2, py + 2, 12, 14, (Color){24, 24, 40, 255});
                DrawRectangle(px + 4, py + 4, 8, 12, (Color){64, 48, 88, 255});
                DrawRectangle(px + 3, py + 2, 10, 2, brilho);
                DrawRectangle(px + 2, py + 5, 2, 8, brilho);
                DrawRectangle(px + 12, py + 5, 2, 8, brilho);
            }
            if (simbolo == 'P' && strcmp(nome, "Jaula") == 0) {
                Color cor = sala->resolvido ? (Color){248, 240, 196, 255} :
                                             (Color){176, 112, 232, 255};
                DrawRectangle(px + 5, py + 2, 7, 9, cor);
                DrawRectangle(px + 6, py + 4, 5, 1, (Color){72, 48, 96, 255});
                DrawRectangle(px + 6, py + 7, 4, 1, (Color){72, 48, 96, 255});
            }
            desenharDetalhe(simbolo, px, py);
        }
    }
}

/* Escuridão do mapa */
static void desenharEscuridao(float centro_x, float centro_y, int camx, int camy) {
    const NoSala *sala = caso_jogo.salas.atual;
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    float lampadas_x[2] = {-1000, -1000}, lampadas_y[2] = {-1000, -1000};
    float luz_x = centro_x, luz_y = centro_y;
    int x, y, i;
    if (mundo.secreta || sala == NULL || sala->luzes_necessarias == 0 ||
        (sala->luzes & sala->luzes_necessarias) == sala->luzes_necessarias) return;
    for (y = 0; y < alturaMapa(mapa); y++) for (x = 0; x < larguraMapa(mapa); x++) {
        char simbolo = tileEm(mapa, x, y);
        if (simbolo == 'l' || simbolo == 'o') {
            i = simbolo == 'l' ? 0 : 1;
            if (sala->luzes & (1U << i)) { lampadas_x[i] = x * TILE - camx + 8.0f; lampadas_y[i] = y * TILE - camy + 8.0f; }
        }
    }
    if (vis.lanterna_ligada) {
        if (mundo.dir == 0) luz_y += 20;
        else if (mundo.dir == 3) luz_y -= 20;
        else if (mundo.dir == 1) luz_x -= 20;
        else luz_x += 20;
    }
    for (y = 0; y < VIRT_A; y += 4) for (x = 0; x < VIRT_L; x += 4) {
        float dx = x + 2.0f - luz_x, dy = y + 2.0f - luz_y;
        float raio = vis.lanterna_ligada ? 58.0f : 13.0f;
        float sombra = (sqrtf(dx * dx + dy * dy) - raio + 18.0f) / 18.0f;
        for (i = 0; i < 2; i++) {
            float local;
            dx = x + 2.0f - lampadas_x[i]; dy = y + 2.0f - lampadas_y[i];
            local = (sqrtf(dx * dx + dy * dy) - 74.0f + 24.0f) / 24.0f;
            if (local < sombra) sombra = local;
        }
        if (sombra < 0) sombra = 0;
        if (sombra > 1) sombra = 1;
        DrawRectangle(x, y, 4, 4, (Color){4, 6, 16, (unsigned char)(24 + sombra * 214)});
    }
}

static void desenharContexto(int camx, int camy) {
    const NoSala *sala = caso_jogo.salas.atual;
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    int x = mundo.tx, y = mundo.ty;
    char simbolo;
    const char *rotulo = NULL;
    if (vis.fala_aberta || vis.caderno || vis.puzzle_aberto || vis.documento_aberto || vis.time_aberto || vis.minimapa_aberto || vis.pokedex_aberta || vis.pc_aberto) return;
    if (mundo.dir == 0) y++;
    else if (mundo.dir == 3) y--;
    else if (mundo.dir == 1) x--;
    else x++;
    simbolo = tileEm(mapa, x, y);
    if (simbolo == 'C') rotulo = "Acessar PC: Gerenciar Equipe";
    else if (simbolo == 'm' && mundo.secreta) rotulo = "Ler mural dos santuários";
    else if (simbolo == 'S' && sala != NULL && strcmp(sala->nome, "Arquivo") == 0) {
        rotulo = casoTemChaveSecreta(&caso_jogo) ? "Passagem lateral para o santuário" : "Passagem lateral [TRANCADA - precisa de chave]";
    }
    else if (simbolo == 'k' && sala != NULL && strcmp(sala->nome, "Arquivo") == 0) {
        rotulo = casoTemChaveSecreta(&caso_jogo) ? "Cofre confidencial aberto" : "Examinar cofre confidencial";
    }
    else if (simbolo == 'F' && mundo.secreta) rotulo = "Altar das Chamas (Charmander)";
    else if (simbolo == 'M' && mundo.secreta) rotulo = "Altar da Mente (Alakazam)";
    else if (simbolo == 'T' && mundo.secreta) rotulo = "Altar do Trovão (Pikachu)";
    else if (simbolo == 'p' && mundo.secreta) rotulo = "Pedestal central da Master Ball";
    else if (simbolo >= '1' && simbolo <= '3' && mundo.secreta) rotulo = "Ativar selo do mural";
    else if (simbolo == 'l' || simbolo == 'o') rotulo = "Pikachu: iluminar setor";
    else if (simbolo == 'n') rotulo = sala != NULL && sala->duelo_concluido ? "Guarda derrotado" : "Desafiar guarda Rocket";
    else if (simbolo == 'P' || simbolo == 'I' || simbolo == 'F') rotulo = "Investigar com parceiro";
    else if (simbolo == 'E' || simbolo == 'G') rotulo = "Iniciar confronto";
    else if (simbolo == 'D') rotulo = portaAberta(sala) ? "Entrar no próximo setor" : "Ver objetivo da porta";
    else if (simbolo == 'b') rotulo = "Descansar: HP e PP";
    else if (simbolo == 's') rotulo = "Ler placa de treino";
    else if (simbolo == '*') {
        int id = idItemDaSala(sala);
        if (!casoItemJaColetado(&caso_jogo, id)) {
            rotulo = (id == 0 || id == 2 || id == 4 || id == 6 || id == 7)
                     ? "Pegar item: Rare Candy"
                     : "Pegar suprimentos";
        }
    }
    else if (simbolo == 'g' || simbolo == 'r') rotulo = "Procurar selvagem";
    else if (tileEm(mapa, mundo.tx, mundo.ty) == 'g' || tileEm(mapa, mundo.tx, mundo.ty) == 'r') {
        x = mundo.tx; y = mundo.ty; rotulo = "Procurar selvagem";
    }
    if (rotulo != NULL) {
        DrawRectangleLines(x * TILE - camx - 1, y * TILE - camy - 1, 18, 18, GOLD);
        caixa(4, 134, 232, 24, (Color){16, 22, 34, 235});
        desenharTecla(0, 8, 138);
        textoAte(rotulo, 28, 142, 200, WHITE);
    } else {
        caixa(4, 138, 232, 20, (Color){16, 22, 34, 210});
        const char *objetivo = casoObjetivoAtual(&caso_jogo);
        if (vis.dica_inicio == 1) objetivo = "L: ligue a lanterna";
        if (vis.dica_inicio == 2) objetivo = "Ache o painel do gerador";
        textoAte(objetivo, 8, 143, 224, destaque_caso);
    }
}

static void desenharDocumento(void) {
    char linhas[11][48];
    char pagina[40];
    int n = 0, i;
    const char *conteudo = casoPaginaDocumento(&caso_jogo, vis.documento_pagina);
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){4, 6, 16, 210});
    DrawRectangle(14, 8, 218, 148, (Color){0, 0, 0, 150});
    caixa(8, 4, 220, 148, (Color){248, 236, 196, 255});
    texto("DOCUMENTO REVELADO", 20, 14, FONTE_PX, (Color){96, 40, 40, 255});
    DrawRectangle(20, 28, 196, 1, (Color){160, 120, 80, 255});
    quebrar(conteudo, linhas, &n, 10, 196.0f);
    for (i = 0; i < n; i++) texto(linhas[i], 20, 35.0f + i * 10.0f, FONTE_PX, (Color){48, 40, 32, 255});
    snprintf(pagina, sizeof pagina, "%d/2  Z: %s", vis.documento_pagina + 1,
             vis.documento_pagina == 0 ? "próxima" : "fechar");
    texto(pagina, 20, 138, FONTE_PX, (Color){96, 40, 40, 255});
}

static void desenharPuzzle(void) {
    const NoSala *sala = caso_jogo.salas.atual;
    const Desafio *d;
    int i, x, y;
    char linha[48];
    const char *titulo;
    if (sala == NULL) return;
    d = &sala->desafio;
    titulo = d->tipo == DES_CIRCUITO ? "PIKACHU / REDE ELETRICA" : d->tipo == DES_GELO ? "CHARMANDER / DESGELO" :
             d->tipo == DES_AGUA ? "SQUIRTLE / HIDRÁULICA" : d->tipo == DES_RASTRO ? "ALAKAZAM / INVESTIGAÇÃO" : "ALAKAZAM / ESTANTES";
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){4, 6, 16, 210});
    caixa(4, 4, 232, 152, (Color){20, 28, 48, 255});
    texto(titulo, 12, 13, FONTE_PX, (Color){248, 216, 104, 255});
    if (d->tipo == DES_CIRCUITO) {
        unsigned int vistos = circuitoAlcancado(d);
        static const int terminais[3] = {2, 6, 8};
        for (i = 0; i < 9; i++) {
            int mascara = mascaraCircuito(d, i);
            Color cor = vistos & (1U << i) ? (Color){128, 232, 144, 255} : (Color){112, 128, 160, 255};
            x = 12 + i % 3 * 28; y = 32 + i / 3 * 28;
            caixa(x, y, 26, 26, (Color){32, 44, 64, 255});
            if (mascara & 1) DrawRectangle(x + 11, y + 1, 4, 13, cor);
            if (mascara & 2) DrawRectangle(x + 12, y + 11, 13, 4, cor);
            if (mascara & 4) DrawRectangle(x + 11, y + 12, 4, 13, cor);
            if (mascara & 8) DrawRectangle(x + 1, y + 11, 13, 4, cor);
            if (i == 2 || i == 6 || i == 8) {
                DrawRectangle(x + 16, y + 16, 9, 9, (Color){20, 28, 48, 255});
                texto(i == 2 ? "1" : i == 6 ? "2" : "3", x + 17.0f, y + 17.0f, FONTE_PX, GOLD);
            }
            if (i == vis.cursor_puzzle) DrawRectangleLines(x, y, 26, 26, YELLOW);
        }
        texto("FONTE >", 112, 36, FONTE_PX, GREEN);
        DrawRectangle(6, 43, 6, 4, GREEN);
        for (i = 0; i < 3; i++) {
            snprintf(linha, sizeof linha, "Terminal %d", i + 1);
            texto(linha, 112, 58.0f + i * 18, FONTE_PX, vistos & (1U << terminais[i]) ? GREEN : GRAY);
        }
        texto("Ligue os 3 terminais.", 12, 121, FONTE_PX, WHITE);
    } else if (d->tipo == DES_GELO) {
        for (i = 0; i < 12; i++) {
            int tubo = i == 1 || i == 2 || i == 8 || i == 11;
            x = 12 + i % 4 * 24; y = 32 + i / 4 * 24;
            caixa(x, y, 22, 22, tubo ? (Color){128, 64, 56, 255} : d->gelo[i] ? (Color){120, 184, 216, 255} : (Color){40, 56, 72, 255});
            if (tubo) { DrawRectangle(x + 8, y + 2, 6, 18, (Color){200, 120, 80, 255}); }
            if (d->gelo[i]) { snprintf(linha, sizeof linha, "%d", d->gelo[i]); texto(linha, x + 7.0f, y + 7.0f, FONTE_PX, BLACK); }
            if (i == vis.cursor_puzzle) DrawRectangleLines(x, y, 22, 22, YELLOW);
        }
        texto("TUBOS", 118, 34, FONTE_PX, (Color){232, 136, 96, 255});
        texto("são frágeis!", 118, 46, FONTE_PX, WHITE);
        snprintf(linha, sizeof linha, "Calor %d", d->calor); texto(linha, 118, 64, FONTE_PX, WHITE);
        DrawRectangle(118, 78, 100, 8, (Color){64, 72, 88, 255});
        DrawRectangle(118, 78, d->calor, 8, d->calor >= 75 ? RED : ORANGE);
        desenharTecla(2, 118, 92); texto("Esfriar", 138, 96, FONTE_PX, WHITE);
        texto("Derreta as 4 travas.", 12, 121, FONTE_PX, WHITE);
    } else if (d->tipo == DES_AGUA) {
        static const char *botoes[4] = {"Encher A", "A -> B", "Drenar B", "Mangueira"};
        caixa(16, 34, 36, 68, (Color){32, 48, 64, 255});
        caixa(72, 58, 36, 44, (Color){32, 48, 64, 255});
        DrawRectangle(20, 98 - d->tanque_a * 12, 28, d->tanque_a * 12, (Color){64, 152, 216, 255});
        DrawRectangle(76, 98 - d->tanque_b * 12, 28, d->tanque_b * 12, (Color){64, 152, 216, 255});
        for (i = 0; i < 4; i++) {
            caixa(128, 32 + i * 20, 96, 18, vis.cursor_puzzle == i ? (Color){248, 216, 104, 255} : (Color){216, 224, 232, 255});
            texto(botoes[i], 134, 37.0f + i * 20, FONTE_PX, BLACK);
        }
        snprintf(linha, sizeof linha, "A %d/5", d->tanque_a); texto(linha, 12, 108, FONTE_PX, WHITE);
        snprintf(linha, sizeof linha, "B %d/3", d->tanque_b); texto(linha, 70, 108, FONTE_PX, WHITE);
        texto("Mangueira: A=4 e B=3.", 12, 121, FONTE_PX, WHITE);
    } else if (d->tipo == DES_RASTRO) {
        int distancia = distanciaRastro(d, vis.cursor_puzzle);

        for (i = 0; i < 20; i++) {
            int exam = (d->examinados & (1U << i));
            int dist_i = distanciaRastro(d, i);
            x = 10 + (i % 5) * 21;
            y = 28 + (i / 5) * 21;

            caixa(x, y, 19, 19, exam ? (Color){28, 36, 52, 255} : (Color){44, 52, 70, 255});
            DrawRectangleLines(x, y, 19, 19, (Color){32, 40, 56, 255});

            if (exam) {
                if (dist_i == 0) {
                    DrawCircle(x + 7, y + 8, 2.0f, (Color){80, 240, 180, 255});
                    DrawCircle(x + 5, y + 6, 1.0f, (Color){140, 255, 210, 255});
                    DrawCircle(x + 7, y + 5, 1.0f, (Color){140, 255, 210, 255});
                    DrawCircle(x + 9, y + 6, 1.0f, (Color){140, 255, 210, 255});

                    DrawCircle(x + 12, y + 13, 2.0f, (Color){80, 240, 180, 255});
                    DrawCircle(x + 10, y + 11, 1.0f, (Color){140, 255, 210, 255});
                    DrawCircle(x + 12, y + 10, 1.0f, (Color){140, 255, 210, 255});
                    DrawCircle(x + 14, y + 11, 1.0f, (Color){140, 255, 210, 255});
                } else if (dist_i == 1) {
                    DrawCircle(x + 9, y + 10, 1.8f, (Color){248, 216, 104, 200});
                    DrawCircle(x + 8, y + 7, 0.8f, (Color){255, 235, 140, 200});
                    DrawCircle(x + 10, y + 7, 0.8f, (Color){255, 235, 140, 200});
                } else {
                    DrawCircle(x + 9, y + 9, 1.5f, (Color){70, 80, 105, 255});
                }
            }

            if (i == vis.cursor_puzzle) {
                float flutua_lupa = sinf(vis.tempo * 4.0f) * 1.0f;
                int lx = x + 9;
                int ly = y + 9 + (int)flutua_lupa;

                DrawCircleLines(lx, ly, 7.5f, GOLD);
                DrawCircleLines(lx, ly, 6.5f, (Color){255, 235, 130, 255});
                DrawCircle(lx, ly, 6.0f, (Color){140, 230, 255, 120});
                DrawLine(lx - 3, ly - 3, lx, ly - 5, WHITE);
                DrawLine(lx, ly - 3, lx, ly + 3, (Color){255, 255, 255, 180});
                DrawLine(lx - 3, ly, lx + 3, ly, (Color){255, 255, 255, 180});
                DrawLineEx((Vector2){(float)lx + 5, (float)ly + 5},
                           (Vector2){(float)lx + 10, (float)ly + 10}, 2.5f, (Color){140, 70, 30, 255});
                DrawCircle(lx + 10, ly + 10, 1.2f, GOLD);
            }
        }

        caixa(120, 28, 112, 84, (Color){14, 22, 36, 255});
        DrawRectangleLines(120, 28, 112, 84, (Color){50, 90, 140, 255});
        texto("LUPA DETETIVE", 126, 33, FONTE_PX, GOLD);

        texto("SINAL:", 126, 46, FONTE_PX, (Color){180, 200, 220, 255});
        if (distancia == 0) {
            caixa(126, 57, 100, 14, (Color){20, 80, 50, 255});
            DrawRectangleLines(126, 57, 100, 14, (Color){80, 240, 160, 255});
            texto("PEGADA AQUI!", 130, 60, FONTE_PX, (Color){100, 255, 180, 255});
        } else if (distancia == 1) {
            caixa(126, 57, 100, 14, (Color){80, 48, 16, 255});
            DrawRectangleLines(126, 57, 100, 14, (Color){255, 160, 60, 255});
            texto("MUITO QUENTE", 128, 60, FONTE_PX, (Color){255, 180, 80, 255});
        } else if (distancia <= 3) {
            caixa(126, 57, 100, 14, (Color){60, 60, 20, 255});
            DrawRectangleLines(126, 57, 100, 14, (Color){230, 210, 60, 255});
            texto("RASTRO MORNO", 128, 60, FONTE_PX, (Color){240, 220, 80, 255});
        } else {
            caixa(126, 57, 100, 14, (Color){20, 40, 60, 255});
            DrawRectangleLines(126, 57, 100, 14, (Color){60, 140, 200, 255});
            texto("RASTRO FRIO", 130, 60, FONTE_PX, (Color){140, 200, 240, 255});
        }

        snprintf(linha, sizeof linha, "Ecos %d/3", d->rastros);
        texto(linha, 126, 78, FONTE_PX, WHITE);
        for (i = 0; i < 3; i++) {
            int achada = (i < d->rastros);
            DrawCircle(196 + i * 11, 81, 3.5f, achada ? (Color){80, 240, 180, 255} : (Color){40, 50, 70, 255});
            DrawCircle(196 + i * 11, 81, 1.5f, achada ? WHITE : (Color){60, 70, 90, 255});
        }

        texto("Z: examinar", 126, 96, FONTE_PX, (Color){180, 190, 210, 255});
        texto("Mova a lupa com as setas.", 12, 121, FONTE_PX, WHITE);
    } else {
        int marcas = !!(d->caixas & (1UL << 10)) + !!(d->caixas & (1UL << 28));
        for (i = 0; i < 36; i++) {
            x = 12 + i % 6 * 15; y = 28 + i / 6 * 15;
            DrawRectangle(x, y, 14, 14, paredeEstantes(i) ? (Color){104, 112, 128, 255} : (Color){40, 48, 64, 255});
            if (i == 10 || i == 28) DrawRectangleLines(x + 2, y + 2, 10, 10, GOLD);
            if (i < 32 && (d->caixas & (1UL << i))) {
                DrawRectangle(x + 2, y + 2, 10, 10, (Color){136, 88, 56, 255});
                DrawRectangle(x + 3, y + 4, 8, 2, GOLD);
                DrawRectangle(x + 3, y + 8, 8, 2, GOLD);
            }
            if (i == d->operador) DrawCircle(x + 7, y + 7, 5, (Color){184, 136, 232, 255});
        }
        texto("Empurre", 120, 34, FONTE_PX, WHITE);
        texto("2 estantes.", 120, 46, FONTE_PX, WHITE);
        texto("Não puxe!", 120, 64, FONTE_PX, (Color){232, 160, 128, 255});
        snprintf(linha, sizeof linha, "Marcas %d/2", marcas); texto(linha, 120, 88, FONTE_PX, GOLD);
        snprintf(linha, sizeof linha, "Movimentos: %d", d->movimentos); texto(linha, 12, 121, FONTE_PX, WHITE);
    }
    if (vis.retorno_puzzle > 0) {
        DrawRectangle(8, 119, 224, 14, (Color){20, 28, 48, 255});
        textoAte(vis.aviso_puzzle, 12, 121, 212, GOLD);
    }
    for (i = 0; i < 4; i++) desenharTecla(6 + i, 12 + i * 17, 136);
    desenharTecla(d->tipo == DES_ESTANTES ? 4 : 0, 90, 136);
    texto(d->tipo == DES_ESTANTES ? "desfaz" : "agir", 108, 140, FONTE_PX, WHITE);
    desenharTecla(5, 164, 136);
    desenharTecla(1, 210, 136);
}

static void desenharTimeExploracao(void) {
    const Pokemon *sel_pk = pokemonNaPosicao(&caso_jogo.nucleo.time, vis.cursor_time);
    const NoTime *no;
    int i = 0;
    int ativo_idx = posicaoDoAtivo(&caso_jogo.nucleo.time);
    char buf[64];

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){4, 6, 16, 220});
    caixa(4, 4, 232, 152, (Color){16, 24, 38, 255});
    DrawRectangleLines(4, 4, 232, 152, (Color){50, 90, 140, 255});

    /* Ficha do Pokémon */
    caixa(8, 8, 98, 144, (Color){10, 18, 30, 255});
    DrawRectangleLines(8, 8, 98, 144, (Color){30, 80, 120, 255});
    DrawRectangle(11, 11, 92, 54, (Color){14, 24, 40, 255});
    DrawRectangleLines(11, 11, 92, 54, (Color){24, 60, 90, 255});

    if (sel_pk != NULL) {
        float bob = sinf(vis.tempo * 3.5f) * 1.5f;
        desenharSpriteMon(sel_pk->numero, 0, (Rectangle){29, 7 + bob, 56, 56}, 0.0f);

        if (sel_pk->brilhante) {
            int s;
            for (s = 0; s < 3; s++) {
                float ang = vis.tempo * 4.0f + (float)s * 2.094f;
                DrawCircle((int)(57.0f + cosf(ang) * 20.0f), (int)(38.0f + sinf(ang) * 14.0f), 1.5f, GOLD);
            }
        }

        nomeNaFicha(sel_pk->nome, 12, 68, 90, WHITE);
        snprintf(buf, sizeof buf, "Nv%d", sel_pk->nivel);
        texto(buf, 12, 78, FONTE_PX, (Color){248, 220, 120, 255});

        if (sel_pk->tipo2 == TIPO_NENHUM) {
            DrawRectangle(46, 77, 54, 10, corDoTipo(sel_pk->tipo1));
            nomeNaFicha(nomeTipo(sel_pk->tipo1), 48, 78, 50, WHITE);
        } else {
            DrawRectangle(46, 77, 28, 10, corDoTipo(sel_pk->tipo1));
            nomeNaFicha(nomeTipo(sel_pk->tipo1), 48, 78, 24, WHITE);
            DrawRectangle(76, 77, 26, 10, corDoTipo(sel_pk->tipo2));
            nomeNaFicha(nomeTipo(sel_pk->tipo2), 78, 78, 22, WHITE);
        }

        snprintf(buf, sizeof buf, "HP %d/%d", sel_pk->hp_atual, sel_pk->hp_max);
        texto(buf, 12, 89, FONTE_PX, (Color){200, 220, 240, 255});
        desenharBarraHp(12, 98, 90, sel_pk->hp_max > 0 ? (float)sel_pk->hp_atual / (float)sel_pk->hp_max : 0.0f);

        snprintf(buf, sizeof buf, "A%d D%d", sel_pk->ataque, sel_pk->defesa);
        textoAte(buf, 12, 107, 92, (Color){170, 200, 230, 255});
        snprintf(buf, sizeof buf, "VEL %d", sel_pk->velocidade);
        textoAte(buf, 12, 117, 92, (Color){170, 200, 230, 255});

        if (sel_pk->qtd_golpes > 0) {
            nomeNaFicha(sel_pk->golpes[0].nome, 12, 126, 90, (Color){220, 230, 240, 255});
        }
        if (sel_pk->qtd_golpes > 1) {
            nomeNaFicha(sel_pk->golpes[1].nome, 12, 136, 90, (Color){220, 230, 240, 255});
        }
    } else {
        texto("Sem Pokemon", 14, 50, FONTE_PX, (Color){100, 120, 140, 255});
    }

    /* Lista da equipe */
    texto("EQUIPE POKEMON", 112, 10, FONTE_PX, (Color){80, 210, 255, 255});

    for (no = caso_jogo.nucleo.time.inicio; no != NULL && i < 6; no = no->prox, i++) {
        char nome[64], nv[16];
        int y = 22 + i * 19;
        int sel = (i == vis.cursor_time);
        int e_ativo = (i == ativo_idx);
        float razao = 0.0f;
        if (no->pokemon.hp_max > 0) {
            razao = (float)no->pokemon.hp_atual / (float)no->pokemon.hp_max;
        }

        caixa(110, y, 122, 18, sel ? (Color){30, 60, 110, 255} : (Color){20, 30, 46, 255});
        DrawRectangleLines(110, y, 122, 18, sel ? GOLD : (Color){36, 50, 72, 255});

        if (sprites[no->pokemon.numero].tem_icone) {
            DrawTexturePro(sprites[no->pokemon.numero].icone,
                           (Rectangle){0, 0, (float)sprites[no->pokemon.numero].icone.width, (float)sprites[no->pokemon.numero].icone.height},
                           (Rectangle){112, (float)y + 2, 14, 14}, (Vector2){0, 0}, 0.0f, WHITE);
        }

        snprintf(nome, sizeof nome, "%s", no->pokemon.nome);
        snprintf(nv, sizeof nv, "N%d", no->pokemon.nivel);
        if (e_ativo) {
            nomeNaFicha(nome, 128, (float)y + 1, 60, sel ? (Color){255, 230, 100, 255} : WHITE);
            texto("LÍDER", 190, (float)y + 1, FONTE_PX, (Color){60, 220, 140, 255});
        } else if (no->pokemon.hp_atual <= 0) {
            nomeNaFicha(nome, 128, (float)y + 1, 60, sel ? (Color){255, 230, 100, 255} : WHITE);
            texto("FNT", 200, (float)y + 2, 6.0f, (Color){240, 80, 80, 255});
        } else {
            nomeNaFicha(nome, 128, (float)y + 1, 66, sel ? (Color){255, 230, 100, 255} : WHITE);
            texto(nv, 198, (float)y + 1, FONTE_PX, (Color){248, 220, 120, 255});
        }

        desenharBarraHp(128, y + 11, 96, razao);
    }

    texto("Z: ativo", 108, 140, FONTE_PX, (Color){180, 200, 220, 255});
    texto("X: sair", 176, 140, FONTE_PX, (Color){180, 200, 220, 255});
}

static void atualizarPcEquipe(void) {
    int n_time = tamanhoTime(&caso_jogo.nucleo.time);
    int n_reserva = casoTamanhoReserva(&caso_jogo);

    if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) {
        if (vis.pc_modo_troca) {
            vis.pc_modo_troca = 0;
            copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Troca cancelada.");
        } else {
            vis.pc_aberto = 0;
        }
        clique();
        return;
    }

    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) ||
        IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D)) {
        vis.pc_aba = 1 - vis.pc_aba;
        clique();
        return;
    }

    if (vis.pc_aba == 0) {
        if (n_time > 0) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                vis.pc_sel_time = (vis.pc_sel_time + 1) % n_time;
                clique();
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                vis.pc_sel_time = (vis.pc_sel_time + n_time - 1) % n_time;
                clique();
            }
        }
    } else {
        if (n_reserva > 0) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                vis.pc_sel_reserva = (vis.pc_sel_reserva + 1) % n_reserva;
                clique();
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                vis.pc_sel_reserva = (vis.pc_sel_reserva + n_reserva - 1) % n_reserva;
                clique();
            }
        }
    }

    /* Escolhe o líder */
    if (IsKeyPressed(KEY_L)) {
        if (vis.pc_aba == 0 && n_time > 0) {
            casoDefinirLider(&caso_jogo, vis.pc_sel_time);
            vis.pc_sel_time = 0;
            copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Novo lider ativo definido!");
            if (vis.som_ok) PlaySound(vis.som_acerto);
        } else {
            copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Apenas membros do time podem ser lideres.");
        }
        return;
    }

    /* Z troca, deposita ou retira */
    if (acaoPressionada()) {
        if (!vis.pc_modo_troca) {
            if (vis.pc_aba == 0 && n_time > 0) {
                vis.pc_modo_troca = 1;
                vis.pc_troca_origem_aba = 0;
                vis.pc_troca_origem_pos = vis.pc_sel_time;
                copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Mover: selecione destino e tecle [Z].");
                clique();
            } else if (vis.pc_aba == 1 && n_reserva > 0) {
                vis.pc_modo_troca = 1;
                vis.pc_troca_origem_aba = 1;
                vis.pc_troca_origem_pos = vis.pc_sel_reserva;
                copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Retirar: tecle [Z] no slot de destino.");
                clique();
            }
        } else {
            int orig_aba = vis.pc_troca_origem_aba;
            int orig_pos = vis.pc_troca_origem_pos;
            int dest_aba = vis.pc_aba;
            int dest_pos = (dest_aba == 0) ? vis.pc_sel_time : vis.pc_sel_reserva;

            if (orig_aba == 0 && dest_aba == 0) {
                casoTrocarOrdemTime(&caso_jogo, orig_pos, dest_pos);
                copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Ordem do time alterada com sucesso!");
                if (vis.som_ok) PlaySound(vis.som_acerto);
            } else if (orig_aba == 0 && dest_aba == 1) {
                if (casoDepositarPokemon(&caso_jogo, orig_pos)) {
                    copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Pokemon depositado no PC.");
                    if (vis.pc_sel_time >= tamanhoTime(&caso_jogo.nucleo.time)) {
                        vis.pc_sel_time = tamanhoTime(&caso_jogo.nucleo.time) - 1;
                    }
                    if (vis.som_ok) PlaySound(vis.som_acerto);
                } else {
                    copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "O time precisa ter pelo menos 1 Pokemon!");
                }
            } else if (orig_aba == 1 && dest_aba == 0) {
                if (tamanhoTime(&caso_jogo.nucleo.time) < MAX_TIME) {
                    if (casoRetirarPokemon(&caso_jogo, orig_pos)) {
                        copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Pokemon retirado para o time!");
                        if (vis.som_ok) PlaySound(vis.som_acerto);
                    }
                } else {
                    const Pokemon *p_res = casoPokemonReserva(&caso_jogo, orig_pos);
                    const Pokemon *p_tim = pokemonNaPosicao(&caso_jogo.nucleo.time, dest_pos);
                    if (p_res != NULL && p_tim != NULL) {
                        Pokemon cop_res = *p_res;
                        Pokemon cop_tim = *p_tim;
                        removerPokemonPorPosicao(&caso_jogo.nucleo.time, dest_pos);
                        removerPokemonPorPosicao(&caso_jogo.reserva_pc, orig_pos);
                        adicionarPokemon(&caso_jogo.nucleo.time, &cop_res);
                        adicionarPokemon(&caso_jogo.reserva_pc, &cop_tim);
                        copiarTexto(vis.pc_aviso, sizeof vis.pc_aviso, "Pokemon trocado com a reserva!");
                        if (vis.som_ok) PlaySound(vis.som_acerto);
                    }
                }
            }
            vis.pc_modo_troca = 0;
        }
    }
}

static void desenharPcEquipe(void) {
    int n_time = tamanhoTime(&caso_jogo.nucleo.time);
    int n_reserva = casoTamanhoReserva(&caso_jogo);
    const Pokemon *sel = NULL;
    char buf[64];
    int j;

    if (vis.pc_aba == 0) {
        if (vis.pc_sel_time >= n_time) vis.pc_sel_time = n_time > 0 ? n_time - 1 : 0;
        sel = pokemonNaPosicao(&caso_jogo.nucleo.time, vis.pc_sel_time);
    } else {
        if (vis.pc_sel_reserva >= n_reserva) vis.pc_sel_reserva = n_reserva > 0 ? n_reserva - 1 : 0;
        sel = casoPokemonReserva(&caso_jogo, vis.pc_sel_reserva);
    }

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){6, 12, 22, 245});
    caixa(4, 4, 232, 152, (Color){16, 26, 42, 255});
    DrawRectangleLines(4, 4, 232, 152, (Color){40, 160, 220, 255});

    DrawCircle(14, 12, 3, (Color){40, 220, 120, 255});
    DrawCircle(22, 12, 3, (Color){40, 180, 240, 255});
    texto("TERMINAL PC", 32, 9, FONTE_PX, (Color){60, 230, 255, 255});
    snprintf(buf, sizeof buf, "T:%d/6 PC:%d", n_time, n_reserva);
    textoAte(buf, 134, 9, 96, (Color){248, 220, 120, 255});

    /* Ficha do Pokémon */
    caixa(8, 21, 98, 120, (Color){10, 18, 30, 255});
    DrawRectangleLines(8, 21, 98, 120, (Color){30, 80, 120, 255});
    DrawRectangle(11, 24, 92, 54, (Color){14, 24, 40, 255});
    DrawRectangleLines(11, 24, 92, 54, (Color){24, 60, 90, 255});

    if (sel != NULL) {
        float bob = sinf(vis.tempo * 3.5f) * 1.5f;
        desenharSpriteMon(sel->numero, 0, (Rectangle){29, 20 + bob, 56, 56}, 0.0f);

        nomeNaFicha(sel->nome, 12, 80, 90, WHITE);
        snprintf(buf, sizeof buf, "Nv%d", sel->nivel);
        texto(buf, 12, 90, FONTE_PX, (Color){248, 220, 120, 255});

        if (sel->tipo2 == TIPO_NENHUM) {
            DrawRectangle(46, 89, 54, 10, corDoTipo(sel->tipo1));
            nomeNaFicha(nomeTipo(sel->tipo1), 48, 90, 50, WHITE);
        } else {
            DrawRectangle(46, 89, 28, 10, corDoTipo(sel->tipo1));
            nomeNaFicha(nomeTipo(sel->tipo1), 48, 90, 24, WHITE);
            DrawRectangle(76, 89, 26, 10, corDoTipo(sel->tipo2));
            nomeNaFicha(nomeTipo(sel->tipo2), 78, 90, 22, WHITE);
        }

        snprintf(buf, sizeof buf, "HP %d/%d", sel->hp_atual, sel->hp_max);
        texto(buf, 12, 101, FONTE_PX, (Color){200, 220, 240, 255});
        desenharBarraHp(12, 110, 90, sel->hp_max > 0 ? (float)sel->hp_atual / (float)sel->hp_max : 0.0f);

        snprintf(buf, sizeof buf, "A%d D%d", sel->ataque, sel->defesa);
        textoAte(buf, 12, 114, 90, (Color){170, 200, 230, 255});

        snprintf(buf, sizeof buf, "VEL %d", sel->velocidade);
        textoAte(buf, 12, 123, 90, (Color){220, 230, 240, 255});
    } else {
        texto("Sem Pokemon", 14, 50, FONTE_PX, (Color){100, 120, 140, 255});
        texto("selecionado.", 14, 62, FONTE_PX, (Color){100, 120, 140, 255});
    }

    /* Abas do PC */
    {
        int aba_t = (vis.pc_aba == 0);
        int aba_r = (vis.pc_aba == 1);
        caixa(110, 21, 54, 14, aba_t ? (Color){36, 120, 190, 255} : (Color){20, 32, 50, 255});
        DrawRectangleLines(110, 21, 54, 14, aba_t ? (Color){80, 200, 255, 255} : (Color){40, 60, 90, 255});
        texto("EQUIPE", 114, 24, FONTE_PX, aba_t ? WHITE : (Color){140, 160, 180, 255});

        caixa(166, 21, 66, 14, aba_r ? (Color){36, 120, 190, 255} : (Color){20, 32, 50, 255});
        DrawRectangleLines(166, 21, 66, 14, aba_r ? (Color){80, 200, 255, 255} : (Color){40, 60, 90, 255});
        texto("RESERVA", 170, 24, FONTE_PX, aba_r ? WHITE : (Color){140, 160, 180, 255});
    }

    caixa(110, 37, 122, 104, (Color){14, 22, 36, 255});
    DrawRectangleLines(110, 37, 122, 104, (Color){30, 80, 120, 255});

    if (vis.pc_aba == 0) {
        for (j = 0; j < 6; j++) {
            const Pokemon *p = pokemonNaPosicao(&caso_jogo.nucleo.time, j);
            int y = 40 + j * 16;
            int e_sel = (j == vis.pc_sel_time);
            int e_origem = (vis.pc_modo_troca && vis.pc_troca_origem_aba == 0 && vis.pc_troca_origem_pos == j);

            if (p != NULL) {
                if (e_origem) {
                    DrawRectangle(112, y, 118, 15, (Color){180, 120, 20, 255});
                    DrawRectangleLines(112, y, 118, 15, GOLD);
                } else if (e_sel) {
                    DrawRectangle(112, y, 118, 15, (Color){30, 70, 140, 255});
                    DrawRectangleLines(112, y, 118, 15, (Color){80, 180, 255, 255});
                }
                if (sprites[p->numero].tem_icone) {
                    DrawTexturePro(sprites[p->numero].icone,
                                   (Rectangle){0, 0, (float)sprites[p->numero].icone.width, (float)sprites[p->numero].icone.height},
                                   (Rectangle){113, (float)y, 14, 14}, (Vector2){0, 0}, 0.0f, WHITE);
                }
                snprintf(buf, sizeof buf, "%s%s", j == 0 ? "*" : " ", p->nome);
                nomeNaFicha(buf, 128, (float)y + 3, 70, e_sel ? (Color){255, 230, 100, 255} : WHITE);
                snprintf(buf, sizeof buf, "N%d", p->nivel);
                texto(buf, 200, (float)y + 3, FONTE_PX, (Color){248, 220, 120, 255});
            } else {
                DrawRectangleLines(112, y, 118, 15, (Color){24, 36, 54, 255});
                texto("VAZIO", 146, (float)y + 4, FONTE_PX, (Color){60, 80, 100, 255});
            }
        }
    } else {
        int topo = vis.pc_sel_reserva - 2;
        if (topo < 0) topo = 0;
        if (n_reserva <= 0) {
            texto("Sem Pokemon", 120, 70, FONTE_PX, (Color){100, 120, 140, 255});
            texto("na reserva.", 116, 82, FONTE_PX, (Color){100, 120, 140, 255});
        }
        for (j = 0; j < 6; j++) {
            int idx = topo + j;
            int y = 40 + j * 16;
            int e_sel = (idx == vis.pc_sel_reserva);
            int e_origem = (vis.pc_modo_troca && vis.pc_troca_origem_aba == 1 && vis.pc_troca_origem_pos == idx);
            if (idx < n_reserva) {
                const Pokemon *p = casoPokemonReserva(&caso_jogo, idx);
                if (p != NULL) {
                    if (e_origem) {
                        DrawRectangle(112, y, 118, 15, (Color){180, 120, 20, 255});
                        DrawRectangleLines(112, y, 118, 15, GOLD);
                    } else if (e_sel) {
                        DrawRectangle(112, y, 118, 15, (Color){30, 70, 140, 255});
                        DrawRectangleLines(112, y, 118, 15, (Color){80, 180, 255, 255});
                    }
                    if (sprites[p->numero].tem_icone) {
                        DrawTexturePro(sprites[p->numero].icone,
                                       (Rectangle){0, 0, (float)sprites[p->numero].icone.width, (float)sprites[p->numero].icone.height},
                                       (Rectangle){113, (float)y, 14, 14}, (Vector2){0, 0}, 0.0f, WHITE);
                    }
                    snprintf(buf, sizeof buf, "%s", p->nome);
                    nomeNaFicha(buf, 128, (float)y + 3, 70, e_sel ? (Color){255, 230, 100, 255} : WHITE);
                    snprintf(buf, sizeof buf, "N%d", p->nivel);
                    texto(buf, 200, (float)y + 3, FONTE_PX, (Color){248, 220, 120, 255});
                }
            }
        }
    }

    if (vis.pc_modo_troca) {
        textoAte("TROCA: escolha destino e Z", 12, 144, 216, (Color){255, 220, 80, 255});
    } else {
        textoAte(vis.pc_aviso[0] ? vis.pc_aviso : "Z:move L:lider TAB X:sai", 12, 144, 216, (Color){180, 210, 240, 255});
    }
}

static Color corDoTipo(Tipo tipo) {
    switch (tipo) {
        case TIPO_FOGO: return (Color){240, 128, 48, 255};
        case TIPO_AGUA: return (Color){104, 144, 240, 255};
        case TIPO_ELETRICO: return (Color){248, 208, 48, 255};
        case TIPO_GRAMA: return (Color){120, 200, 80, 255};
        case TIPO_GELO: return (Color){152, 216, 216, 255};
        case TIPO_LUTADOR: return (Color){192, 48, 40, 255};
        case TIPO_VENENO: return (Color){160, 64, 160, 255};
        case TIPO_TERRA: return (Color){224, 192, 104, 255};
        case TIPO_VOADOR: return (Color){168, 144, 240, 255};
        case TIPO_PSIQUICO: return (Color){248, 88, 136, 255};
        case TIPO_INSETO: return (Color){168, 184, 32, 255};
        case TIPO_PEDRA: return (Color){184, 160, 56, 255};
        case TIPO_FANTASMA: return (Color){112, 88, 152, 255};
        case TIPO_DRAGAO: return (Color){112, 56, 248, 255};
        default: return (Color){168, 168, 120, 255};
    }
}

static void desenharMinimapaModal(void) {
    const NoSala *sala = caso_jogo.salas.atual;
    const char **mapa = mapaDa(sala == NULL ? "" : sala->nome);
    const NoSala *cursor;
    int idx = 0;
    int x, y;
    int sala_atual_idx = indiceDaSala(sala);
    static const char *nomes_curtos[8] = {
        "LAB", "PATIO", "FRIA", "TANQ",
        "OFIC", "JAULA", "ARQ", "CHEFE"
    };

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){4, 6, 14, 220});
    caixa(6, 4, 228, 152, (Color){16, 24, 38, 255});
    DrawRectangleLines(6, 4, 228, 152, (Color){60, 80, 110, 255});
    texto("MAPA TÁTICO DA INSTALAÇÃO", 14, 8, FONTE_PX, GOLD);
    DrawRectangle(14, 18, 212, 1, (Color){70, 90, 120, 200});

    /* Mapa dos setores */
    cursor = caso_jogo.salas.inicio;
    while (cursor != NULL && idx < 8) {
        int linha = idx / 4;
        int col = idx % 4;
        int bx = 12 + col * 54;
        int by = 22 + linha * 20;
        int e_atual = (!mundo.secreta && idx == sala_atual_idx);
        int resolvido = cursor->resolvido;
        Color fundo = e_atual ? (Color){32, 84, 160, 255} :
                      resolvido ? (Color){24, 68, 44, 255} :
                      cursor->desbloqueada ? (Color){36, 44, 58, 255} : (Color){20, 24, 32, 255};
        Color borda = e_atual ? GOLD :
                      resolvido ? (Color){100, 220, 130, 255} : (Color){60, 70, 90, 255};

        caixa(bx, by, 50, 17, fundo);
        DrawRectangleLines(bx, by, 50, 17, borda);
        textoAte(nomes_curtos[idx], bx + 3, by + 3, 40, WHITE);
        if (e_atual) {
            DrawRectangle(bx + 44, by + 5, 3, 5, GOLD);
        } else if (resolvido) {
            DrawRectangle(bx + 44, by + 5, 3, 5, (Color){120, 240, 140, 255});
        }
        cursor = cursor->prox;
        idx++;
    }

    /* Santuário secreto */
    {
        int sx = 44, sy = 64;
        int e_secreta = mundo.secreta;
        int tem_chave = casoTemChaveSecreta(&caso_jogo);
        int alt = caso_jogo.altares_secretos;
        Color fundo_sec = e_secreta ? (Color){80, 24, 96, 255} :
                          tem_chave ? (Color){44, 24, 56, 255} : (Color){22, 16, 28, 255};
        Color borda_sec = e_secreta ? GOLD :
                          tem_chave ? (Color){180, 100, 220, 255} : (Color){64, 48, 80, 255};
        caixa(sx, sy, 182, 17, fundo_sec);
        DrawRectangleLines(sx, sy, 182, 17, borda_sec);
        if (tem_chave) {
            char status_sec[64];
            snprintf(status_sec, sizeof status_sec, "SANT. F:%c M:%c T:%c %s",
                     (alt & 1) ? '*' : '-', (alt & 2) ? '*' : '-', (alt & 4) ? '*' : '-',
                     (alt == 7) ? "[MB]" : "");
            texto(status_sec, (float)sx + 4, (float)sy + 4, FONTE_PX, e_secreta ? GOLD : (Color){230, 200, 255, 255});
        } else {
            texto("SANTUÁRIO: TRANCADO", (float)sx + 4, (float)sy + 4, FONTE_PX, (Color){140, 120, 160, 255});
        }
    }

    /* Sala atual */
    {
        int rx = 14, ry = 86;
        int escala = 2;
        int mw = larguraMapa(mapa);
        int mh = alturaMapa(mapa);
        if (mw > 20) mw = 20;
        if (mh > 14) mh = 14;
        DrawRectangle(rx - 2, ry - 2, mw * escala + 4, mh * escala + 4, (Color){10, 14, 22, 255});
        DrawRectangleLines(rx - 2, ry - 2, mw * escala + 4, mh * escala + 4, (Color){70, 90, 120, 255});
        for (y = 0; y < mh; y++) {
            for (x = 0; x < mw; x++) {
                char s = tileEm(mapa, x, y);
                int px = rx + x * escala;
                int py = ry + y * escala;
                Color c = (Color){160, 170, 180, 255};
                if (s == '#' || s == 'H') c = (Color){40, 44, 54, 255};
                else if (s == 'D' || s == 'S') c = GOLD;
                else if (s == 'k') c = (Color){64, 220, 255, 255};
                else if (s == 'F') c = (Color){255, 120, 40, 255};
                else if (s == 'M') c = (Color){255, 90, 200, 255};
                else if (s == 'T') c = (Color){255, 240, 40, 255};
                else if (s == 'p') c = (Color){200, 140, 255, 255};
                else if (s == 'g' || s == 'r') c = (Color){60, 130, 70, 255};
                DrawRectangle(px, py, escala, escala, c);
            }
        }
        DrawRectangle(rx + mundo.pika_x * escala, ry + mundo.pika_y * escala, escala, escala, YELLOW);
        DrawRectangle(rx + mundo.tx * escala, ry + mundo.ty * escala, escala, escala, (Color){40, 220, 255, 255});
    }

    /* Dados da sala */
    {
        char buf[64];
        const char *nome_loc = mundo.secreta ? "Santuário Secreto" : (sala ? sala->nome : "Laboratório");
        char objetivo[2][48];
        int n_obj = 0;
        int k;
        textoAte(nome_loc, 84, 86, 142, WHITE);
        snprintf(buf, sizeof buf, "Pos: X%02d Y%02d", mundo.tx, mundo.ty);
        texto(buf, 84, 98, FONTE_PX, (Color){180, 200, 220, 255});
        snprintf(buf, sizeof buf, "CHAVE: %s", casoTemChaveSecreta(&caso_jogo) ? "ENCONTRADA" : "PENDENTE");
        texto(buf, 84, 110, FONTE_PX, casoTemChaveSecreta(&caso_jogo) ? GOLD : (Color){150, 160, 180, 255});
        quebrar(casoObjetivoAtual(&caso_jogo), objetivo, &n_obj, 2, 208.0f);
        for (k = 0; k < n_obj; k++) {
            texto(objetivo[k], 14, 120.0f + (float)k * 10.0f, FONTE_PX, (Color){248, 216, 104, 255});
        }
    }

    texto("M/X: fechar  P: Pokédex", 14, 142, FONTE_PX, (Color){180, 200, 220, 255});
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
    int maxx = larguraMapa(mapa) * TILE - VIRT_L;
    int maxy = alturaMapa(mapa) * TILE - VIRT_A;
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
    if (maxx < 0) {
        camx = maxx / 2;
    } else {
        if (camx < 0) camx = 0;
        if (camx > maxx) camx = maxx;
    }
    if (maxy < 0) {
        camy = maxy / 2;
    } else {
        if (camy < 0) camy = 0;
        if (camy > maxy) camy = maxy;
    }

    if (frlg_ok && larguraMapa(mapa) * TILE < VIRT_L) {
        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){214, 206, 176, 255});
    }
    desenharMapa(sala, camx, camy);
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
        float fy = piy - (float)camy - 28.0f;
        int quadro_pika = 0;
        if (mundo.pika_anda < 1.0f) {
            quadro_pika = mundo.pika_anda < 0.5f ? 1 : 2;
        }
        if (piy <= jy) {
            DrawEllipse((int)fx + 16, (int)fy + 30, 8.0f, 3.0f, (Color){0, 0, 0, 70});
            desenharPikachuOw(mundo.pika_dir, quadro_pika, fx, fy);
        }
        DrawEllipse((int)px + 8, (int)py + 30, 7.0f, 3.0f, (Color){0, 0, 0, 70});
        desenharFolha(detetive_tex, detetive_ok, mundo.dir, quadro, px, py);
        if (piy > jy) {
            DrawEllipse((int)fx + 16, (int)fy + 30, 8.0f, 3.0f, (Color){0, 0, 0, 70});
            desenharPikachuOw(mundo.pika_dir, quadro_pika, fx, fy);
        }
    }

    numero = numeroEncontro(sala == NULL ? "" : sala->nome);
    for (y = 0; y < alturaMapa(mapa); y++) {
        for (x = 0; x < larguraMapa(mapa); x++) {
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
            if (simbolo == 'n' && sala != NULL) {
                desenharGuarda(strcmp(sala->nome, "Oficina") == 0, ex, ey + 8, 1);
                if (!sala->duelo_concluido) texto("!", ex + 4, ey - 2, FONTE_PX, GOLD);
            }
        }
    }

    /* Escuridão só no mapa */
    desenharEscuridao(px + 8.0f, py + 20.0f, camx, camy);
    if (sala != NULL && !vis.fala_aberta) {
        const char *nome_setor = mundo.secreta ? "Santuario" : sala->nome;
        int indice = indiceDaSala(sala);
        char tag[32];
        snprintf(tag, sizeof tag, "S%d %s", indice + 1, nome_setor);
        caixa(3, 3, 153, 16, fundo_caso);
        textoAte(tag, 7, 6, 144, texto_caso);
        caixa(184, 3, 52, 16, fundo_caso);
        texto("ESC", 198, 6, FONTE_PX, texto_secundario);

        if (sala->luzes_necessarias) {
            char luzes[16];
            int n = !!(sala->luzes & 1U) + !!(sala->luzes & 2U);
            snprintf(luzes, sizeof luzes, "Luz %d/2", n);
            caixa(152, 22, 84, 23, (Color){12, 16, 26, 220});
            desenharTecla(3, 154, 25);
            textoAte(luzes, 174, 28, 58, WHITE);
        }

        if (casoTemChaveSecreta(&caso_jogo)) {
            caixa(126, 22, 23, 16, (Color){40, 24, 60, 230});
            DrawRectangle(130, 28, 6, 3, GOLD);
            DrawCircle(139, 29, 3.0f, GOLD);
        }
    }
    desenharContexto(camx, camy);
    if (vis.fala_aberta) {
        desenharMensagem(vis.fala, 0, 112, VIRT_L, 48);
    }
    if (vis.minimapa_aberto) { desenharMinimapaModal(); return; }
    if (vis.pokedex_aberta) { desenharPokedex(); return; }
    if (vis.documento_aberto) { desenharDocumento(); return; }
    if (vis.puzzle_aberto) { desenharPuzzle(); return; }
    if (vis.time_aberto) { desenharTimeExploracao(); return; }
    if (vis.pc_aberto) { desenharPcEquipe(); return; }
    if (vis.caderno) {
        char linhas[8][LOG_MAX];
        int n;
        int i;
        NoSala *cursor;
        const NoLog *pista;

        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){0, 0, 0, 90});
        if (vis.abertura_caderno > 0.0f) {
            int largura = 24 + (int)(184.0f * (1.0f - vis.abertura_caderno / 0.18f));
            caixa(120 - largura / 2, 12, largura, 136, (Color){236, 224, 184, 255});
            return;
        }
        caixa(16, 12, 208, 136, (Color){236, 224, 184, 255});
        texto("CADERNO", 24.0f, 18.0f, FONTE_PX, (Color){80, 48, 24, 255});
        n = copiarHistorico(linhas, 6);
        for (i = 0; i < n; i++) {
            char trecho[1][48];
            int ntrecho = 0;
            quebrar(linhas[i], trecho, &ntrecho, 1, 184.0f);
            texto(ntrecho > 0 ? trecho[0] : "", 24.0f, 34.0f + (float)i * 10.0f, FONTE_PX,
                  (Color){40, 32, 24, 255});
        }
        /* A pista sai da pilha do log */
        pista = caso_jogo.nucleo.log.topo;
        while (pista != NULL && strncmp(pista->texto, "Documento:", 10) != 0) {
            pista = pista->prox;
        }
        if (pista != NULL) {
            char dicas[5][48];
            int quantidade = 0;
            quebrar(pista->texto, dicas, &quantidade, 5, 192.0f);
            for (i = 0; i < quantidade; i++) {
                texto(dicas[i], 24, 96.0f + i * 10.0f, FONTE_PX,
                      (Color){72, 40, 96, 255});
            }
            texto("R: ler documento", 24, 140, FONTE_PX, (Color){96, 40, 40, 255});
            return;
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
    caixa(x, y, w, mostrar_numeros ? 42 : 32, (Color){248, 248, 240, 255});
    snprintf(nivel, sizeof nivel, "Nv%d", pokemon->nivel);
    textoAte(pokemon->nome, (float)x + 6.0f, (float)y + 4.0f,
             (float)w - larguraTexto(nivel, FONTE_PX) - 16.0f, BLACK);
    texto(nivel, (float)x + (float)w - larguraTexto(nivel, FONTE_PX) - 6.0f, (float)y + 4.0f, FONTE_PX, BLACK);
    texto("HP", (float)x + 6.0f, (float)y + 16.0f, FONTE_PX, (Color){216, 168, 48, 255});
    desenharBarraHp(x + 24, y + 18, w - 32, razao);
    if (mostrar_numeros && pokemon->hp_max > 0) {
        float exp_razao = (float)(pokemon->experiencia % 60) / 60.0f;
        int exp_w;
        int i_slot;
        int n_m = tamanhoTime(&caso_jogo.nucleo.time);

        atual = (int)(razao * (float)pokemon->hp_max + 0.5f);
        if (atual < 0) atual = 0;
        if (atual > pokemon->hp_max) atual = pokemon->hp_max;
        snprintf(pontos, sizeof pontos, "%d/%d", atual, pokemon->hp_max);
        texto(pontos, (float)x + (float)w - larguraTexto(pontos, FONTE_PX) - 6.0f,
              (float)y + 26.0f, FONTE_PX, BLACK);

        exp_w = w - (int)larguraTexto(pontos, FONTE_PX) - 46;
        if (exp_w < 0) exp_w = 0;
        /* EXP ao lado do HP */
        texto("EXP", (float)x + 6.0f, (float)y + 27.0f, FONTE_PX, (Color){40, 140, 200, 255});
        DrawRectangle(x + 34, y + 30, exp_w, 3, (Color){40, 48, 64, 255});
        if (exp_w > 2 && exp_razao > 0.0f) {
            DrawRectangle(x + 35, y + 31, (int)((float)(exp_w - 2) * exp_razao), 1, (Color){60, 180, 240, 255});
        }

        /* Pokébolas do time */
        for (i_slot = 0; i_slot < n_m && i_slot < 6; i_slot++) {
            const Pokemon *m = pokemonNaPosicao(&caso_jogo.nucleo.time, i_slot);
            int vivo = (m != NULL && m->hp_atual > 0);
            DrawCircle(x + 8 + i_slot * 8, y + 36, 2.5f, vivo ? (Color){220, 50, 40, 255} : (Color){110, 110, 120, 255});
            DrawCircle(x + 8 + i_slot * 8, y + 36, 1.0f, WHITE);
        }
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
    if (vis.resultado == 3) {
        sairParaMundo();
        return;
    }
    if (vis.resultado == 0 && caso_jogo.caso_encerrado) {
        vis.fase = BAT_FIM;
        vis.vitoria = 1;
        vis.depois_fala = 4;
        abrirPaginas(historiaDesfecho);
        return;
    }
    if (vis.resultado == 0) {
        if (vis.batalha_treinador) memset(vis.pp_init, 0, sizeof vis.pp_init);
        sairParaMundo();
        return;
    }
    if (vis.resultado < 0 && !caso_jogo.em_batalha) {
        vis.fase = BAT_FIM;
        vis.vitoria = 0;
        vis.depois_fala = 3;
        abrirFala("Seu time desmaiou.");
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
    if (vis.fase == BAT_ENTRADA && vis.entrada < 1.0f) {
        vis.entrada += dt / 1.35f;
    } else if (vis.entrada < 1.0f) {
        vis.entrada += dt / 0.4f;
    }
    if (vis.entrada > 1.0f) vis.entrada = 1.0f;

    if (vis.fase == BAT_ANALISE) {
        int n = tamanhoTime(&caso_jogo.oponente_atual.time);
        if (n > 0 && (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)))
            vis.cursor_time = (vis.cursor_time + 1) % n;
        if (n > 0 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)))
            vis.cursor_time = (vis.cursor_time + n - 1) % n;
        if (acaoPressionada()) {
            vis.fase = BAT_ENTRADA;
            vis.entrada = 0.0f;
            vis.cursor_time = 0;
            clique();
        }
        return;
    }

    if (vis.fase == BAT_ENTRADA) {
        if (vis.entrada >= 1.0f || acaoPressionada()) {
            vis.entrada = 1.0f;
            vis.fase = BAT_TEXTO;
        }
        return;
    }
    if (vis.fase == BAT_CAPTURA) {
        int comando = -1, resultado = -1;
        vis.captura_t += dt;
        vis.captura_animacao += dt;
        if (vis.captura_feita) {
            if (vis.captura_t > 1.0f) {
                char msg[LOG_MAX];
                snprintf(msg, sizeof msg, "%s foi capturado!", caso_jogo.captura_pendente.nome);
                filaLimpar(); filaPor(msg);
                vis.resultado = 0; vis.fase = BAT_TEXTO;
            }
            return;
        }
        if (vis.retorno_captura > 0) { vis.retorno_captura -= dt; vis.captura_t = 0; return; }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) comando = 0;
        else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) comando = 1;
        else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) comando = 2;
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) comando = 3;
        else if (acaoPressionada()) comando = 4;
        if (caso_jogo.captura_acertos > 0 && vis.captura_t * 1000 > casoPrazoCaptura(&caso_jogo)) {
            resultado = casoResponderCaptura(&caso_jogo, casoComandoCaptura(&caso_jogo), (int)(vis.captura_t * 1000));
        } else if (comando >= 0) resultado = casoResponderCaptura(&caso_jogo, comando, (int)(vis.captura_t * 1000));
        if (resultado >= 0) {
            vis.captura_t = 0;
            clique();
            if (caso_jogo.captura_acertos <= 1) vis.captura_animacao = 0;
            if (resultado == 0) vis.retorno_captura = 0.8f;
            if (resultado == 2) vis.captura_feita = casoCapturar(&caso_jogo);
        }
        return;
    }
    if (vis.fase == BAT_FIM) {
        revelarTexto(dt, vis.fala);
        if (acaoPressionada()) {
            confirmarFala();
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
                vis.fase = BAT_BOLSA;
                vis.cursor_bolsa = 0;
            } else if (vis.cursor == 2) {
                vis.fase = BAT_TIME;
                vis.cursor_time = posicaoDoAtivo(&caso_jogo.nucleo.time);
                if (vis.cursor_time < 0) vis.cursor_time = 0;
            } else {
                SaidaBatalha saida;
                int resultado;
                filaLimpar();
                saida.narrar = narrarUi;
                saida.contexto = NULL;
                saida.log = &caso_jogo.nucleo.log;
                resultado = casoTentarFugir(&caso_jogo, &saida);
                vis.resultado = (resultado == -2) ? 1 : resultado;
                vis.fase = BAT_TEXTO;
                if (vis.fila_n == 0) {
                    filaPor("Não deu para fugir.");
                }
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
            } else if (vis.cursor_time == posicaoDoAtivo(&caso_jogo.nucleo.time)) {
                char msg[LOG_MAX];
                snprintf(msg, sizeof msg, "%s já está em combate!", escolhido->nome);
                filaLimpar();
                filaPor(msg);
                vis.fase = BAT_TEXTO;
                vis.resultado = 1;
                clique();
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
        return;
    }
    if (vis.fase == BAT_BOLSA) {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            vis.cursor_bolsa = (vis.cursor_bolsa + 1) % ITEM_CONTAGEM;
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            vis.cursor_bolsa = (vis.cursor_bolsa + ITEM_CONTAGEM - 1) % ITEM_CONTAGEM;
        }
        if (voltarPressionado()) {
            vis.fase = BAT_COMANDO;
            return;
        }
        if (acaoPressionada()) {
            SaidaBatalha saida;
            int resultado;
            filaLimpar();
            saida.narrar = narrarUi;
            saida.contexto = NULL;
            saida.log = &caso_jogo.nucleo.log;
            resultado = casoUsarItem(&caso_jogo, vis.cursor_bolsa, &saida);
            if (resultado == -2 && vis.fila_n == 0) {
                vis.fase = BAT_BOLSA;
                return;
            }
            vis.resultado = (resultado == -2) ? 1 : resultado;
            vis.fase = BAT_TEXTO;
            clique();
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
    int treinador = chefe || vis.batalha_treinador;
    int numero_opp = 0;
    int intro = vis.fase == BAT_ENTRADA;
    float t_intro = intro ? vis.entrada : 1.0f;
    float entra = t_intro / 0.40f;
    float sai = 0.0f;
    float sx_eu = 0.0f;
    float sx_opp = 0.0f;
    float x_costas;
    float x_gio;
    float x_poke;
    float x_inimigo;
    int quadro_costas = 0;
    Rectangle jogador;
    Rectangle inimigo;
    Color ceu;
    Color chao;
    Color plat_c;
    Color plat_e;
    const char *comandos[] = {"LUTAR", "BOLSA", "TIME", "FUGIR"};
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
    } else if (vis.batalha_treinador) {
        ceu = (Color){88, 104, 160, 255};
        chao = (Color){56, 64, 112, 255};
        plat_c = (Color){144, 152, 200, 255};
        plat_e = (Color){32, 40, 80, 255};
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
    if (entra > 1.0f) entra = 1.0f;
    if (t_intro > 0.58f) sai = (t_intro - 0.58f) / 0.42f;
    if (sai > 1.0f) sai = 1.0f;
    if (!intro) {
        entra = 1.0f;
        sai = 1.0f;
    }
    x_costas = (-56.0f * (1.0f - entra) + 8.0f * entra);
    x_costas = x_costas + ((-96.0f) - x_costas) * sai;
    x_poke = -48.0f * (1.0f - sai) + (26.0f + sx_eu) * sai;
    x_gio = 210.0f * (1.0f - entra) + 148.0f * entra;
    x_gio = x_gio + (250.0f - x_gio) * sai;
    x_inimigo = treinador
        ? (-20.0f * (1.0f - sai) + (136.0f + sx_opp) * sai)
        : (210.0f * (1.0f - entra) + (136.0f + sx_opp) * entra);
    if (sai > 0.2f && sai < 0.75f) quadro_costas = 3;

    if (intro && sai < 0.98f) {
        desenharCostas(quadro_costas, x_costas, 40.0f);
        if (vis.batalha_treinador) desenharGuarda(strcmp(sala->nome, "Oficina") == 0, x_gio, -4, 3);
        if (chefe && giovanni_bat_ok) {
            DrawTexturePro(giovanni_batalha,
                           (Rectangle){0, 0, (float)giovanni_batalha.width, (float)giovanni_batalha.height},
                           (Rectangle){x_gio, -4.0f, 80.0f, 80.0f}, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
    {
        float low_hp_mult = (vis.hp_eu <= 0.2f && vis.hp_eu > 0.0f) ? 2.0f : 1.0f;
        float bob_eu = sinf(vis.tempo * 3.2f * low_hp_mult) * (1.5f * (low_hp_mult > 1.0f ? 0.7f : 1.0f));
        float bob_opp = cosf(vis.tempo * 2.8f) * 1.5f;
        float breathe_eu = sinf(vis.tempo * 3.2f * low_hp_mult) * (0.02f * low_hp_mult);
        float breathe_opp = cosf(vis.tempo * 2.8f) * 0.02f;

        DrawEllipse((int)x_poke + 32, 101, 24.0f, 6.0f, (Color){0, 0, 0, 75});
        DrawEllipse((int)x_inimigo + 32, 59, 21.0f, 5.0f, (Color){0, 0, 0, 75});

        float lunge_x = 0.0f;
        float lunge_y = 0.0f;
        if (vis.flash_opp > 0.04f || vis.tremor_opp > 0.1f) {
            lunge_x = 10.0f;
            lunge_y = -4.0f;
        }

        jogador = (Rectangle){x_poke - (32.0f * breathe_eu) + lunge_x,
                              40.0f + bob_eu - (64.0f * breathe_eu) + lunge_y,
                              64.0f * (1.0f + breathe_eu),
                              64.0f * (1.0f + breathe_eu)};
        if (eu != NULL && (!intro || sai > 0.02f)) {
            desenharSpriteMon(eu->numero, 1, jogador, vis.flash_eu);
        }
        inimigo = (Rectangle){x_inimigo - (32.0f * breathe_opp),
                              (chefe ? 6.0f : 2.0f) + bob_opp - (64.0f * breathe_opp),
                              64.0f * (1.0f + breathe_opp),
                              64.0f * (1.0f + breathe_opp)};
    }
    if (vis.fase == BAT_CAPTURA && (vis.captura_feita || (caso_jogo.captura_acertos > 0 && vis.captura_animacao > 0.55f))) {
        numero_opp = 0;
    } else if (vis.fase == BAT_CAPTURA && caso_jogo.captura_acertos > 0 && vis.captura_animacao > 0.4f) {
        float k = 1.0f - (vis.captura_animacao - 0.4f) / 0.15f;
        if (k < 0.0f) k = 0.0f;
        inimigo.width *= k;
        inimigo.height *= k;
        inimigo.x += (64.0f - inimigo.width) * 0.5f;
        inimigo.y += 64.0f - inimigo.height;
    }
    if (numero_opp > 0 && (!intro || (treinador ? sai > 0.02f : entra > 0.08f))) {
        desenharSpriteMon(numero_opp, 0, inimigo, vis.flash_opp);
    }
    if (op != NULL && op->brilhante && vis.fase != BAT_ANALISE) {
        int s_idx;
        for (s_idx = 0; s_idx < 3; s_idx++) {
            float ang = vis.tempo * 4.0f + (float)s_idx * 2.094f;
            int px = (int)(x_inimigo + 32.0f + cosf(ang) * 26.0f);
            int py = (int)(inimigo.y + 32.0f + sinf(ang) * 18.0f);
            DrawCircle(px, py, 2.0f, GOLD);
            DrawCircle(px, py, 1.0f, WHITE);
        }
        caixa(148, 67, 76, 16, (Color){248, 216, 104, 255});
        texto("SHINY!", 157, 71, FONTE_PX, (Color){120, 56, 16, 255});
    }
    if (eu != NULL && eu->brilhante && (!intro || sai > 0.02f)) {
        int s_idx;
        for (s_idx = 0; s_idx < 3; s_idx++) {
            float ang = vis.tempo * 4.0f + (float)s_idx * 2.094f;
            int px = (int)(jogador.x + 32.0f + cosf(ang) * 26.0f);
            int py = (int)(jogador.y + 32.0f + sinf(ang) * 18.0f);
            DrawCircle(px, py, 2.0f, GOLD);
            DrawCircle(px, py, 1.0f, WHITE);
        }
    }
    if (vis.flash_opp > 0.04f) {
        int spark;
        for (spark = 0; spark < 4; spark++) {
            float sa = (float)spark * 1.57f + vis.tempo * 12.0f;
            float dist = (1.0f - vis.flash_opp) * 22.0f;
            int sx = (int)(inimigo.x + 32.0f + cosf(sa) * dist);
            int sy = (int)(inimigo.y + 32.0f + sinf(sa) * dist);
            DrawCircle(sx, sy, 2.5f, GOLD);
            DrawCircle(sx, sy, 1.0f, WHITE);
        }
    }
    if (vis.flash_eu > 0.04f) {
        int spark;
        for (spark = 0; spark < 4; spark++) {
            float sa = (float)spark * 1.57f + vis.tempo * 12.0f;
            float dist = (1.0f - vis.flash_eu) * 20.0f;
            int sx = (int)(jogador.x + 32.0f + cosf(sa) * dist);
            int sy = (int)(jogador.y + 32.0f + sinf(sa) * dist);
            DrawCircle(sx, sy, 2.5f, (Color){255, 120, 80, 255});
            DrawCircle(sx, sy, 1.0f, WHITE);
        }
    }

    if (op != NULL) {
        desenharCaixaHp(op, vis.hp_opp, 4, 4, 136, 0);
    } else if (vis.fase == BAT_CAPTURA) {
        desenharCaixaHp(&caso_jogo.captura_pendente, vis.hp_opp, 4, 4, 136, 0);
    }
    desenharCaixaHp(eu, vis.hp_eu, 112, 64, 122, 1);

    if (vis.fase == BAT_CAPTURA) {
        int comando = casoComandoCaptura(&caso_jogo);
        int indice = comando >= 0 && comando < 4 ? 6 + comando : 0;
        float u = vis.captura_animacao / 0.5f;
        float bx, by;
        float prazo = (float)casoPrazoCaptura(&caso_jogo) / 1000.0f;
        float restante = prazo > 0 ? 1 - vis.captura_t / prazo : 1;
        if (u > 1) u = 1;
        if (restante < 0) restante = 0;
        bx = 28 + 148 * u;
        by = 90 - 44 * u - sinf(u * 3.1415926f) * 30;
        if (vis.captura_animacao > 0.55f) { bx = 172 + sinf(vis.captura_animacao * 18) * 3; by = 40; }
        if (caso_jogo.captura_acertos > 0 || vis.captura_feita) {
            if (pokebola_ok) DrawTexturePro(pokebola_tex, (Rectangle){0, 0, (float)pokebola_tex.width, (float)pokebola_tex.height},
                                          (Rectangle){bx, by, 16, 16}, (Vector2){0, 0}, 0, WHITE);
            else DrawRectangle((int)bx, (int)by, 12, 12, RED);
        } else DrawRectangleLines(146, 0, 66, 64, GOLD);
        caixa(0, 106, VIRT_L, 54, (Color){20, 28, 48, 255});
        texto(vis.captura_feita ? "CAPTURA CONCLUIDA!" : vis.retorno_captura > 0 ? "ESCAPOU! TENTE DE NOVO." :
              caso_jogo.captura_acertos == 0 ? "LANCE A POKE BOLA" : "CONTENHA O SELVAGEM!", 8, 114, FONTE_PX, GOLD);
        for (i = 0; i < 5; i++) DrawRectangle(8 + i * 16, 128, 12, 5,
                            i < caso_jogo.captura_acertos ? GREEN : (Color){64, 72, 96, 255});
        if (caso_jogo.captura_acertos > 0 && !vis.captura_feita) {
            DrawRectangle(8, 137, 174, 4, (Color){64, 72, 96, 255});
            DrawRectangle(8, 137, (int)(174 * restante), 4, restante < 0.3f ? RED : GREEN);
        }
        texto("Siga a tecla destacada.", 8, 148, FONTE_PX, WHITE);
        DrawRectangleLines(200, 126, 28, 28, GOLD);
        desenharTecla(indice, 206, 132);
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
                nomeNaFicha(eu->golpes[i].nome, (float)cx + 4.0f, (float)cy + 3.0f, 110.0f, tinta);
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

            if (i == 0) {
                DrawRectangle(cx + 4, cy + 9, 5, 5, (Color){220, 50, 40, 255});
                DrawRectangle(cx + 7, cy + 7, 2, 8, (Color){250, 190, 60, 255});
            } else if (i == 1) {
                DrawRectangle(cx + 4, cy + 8, 6, 7, (Color){160, 100, 50, 255});
                DrawRectangle(cx + 5, cy + 6, 4, 3, (Color){200, 160, 80, 255});
            } else if (i == 2) {
                DrawCircle(cx + 7, cy + 12, 4, (Color){220, 40, 40, 255});
                DrawRectangle(cx + 3, cy + 12, 8, 4, WHITE);
                DrawCircle(cx + 7, cy + 12, 1, BLACK);
            } else if (i == 3) {
                DrawTriangle((Vector2){(float)cx + 10, (float)cy + 12},
                             (Vector2){(float)cx + 4, (float)cy + 8},
                             (Vector2){(float)cx + 4, (float)cy + 16}, (Color){60, 180, 240, 255});
            }
            textoAte(comandos[i], (float)cx + 14.0f, (float)cy + 8.0f, (float)largura - 16.0f,
                     selecionado ? (Color){248, 248, 240, 255} : BLACK);
        }
    } else if (vis.fase == BAT_BOLSA) {
        int i;
        caixa(24, 36, 192, 112, (Color){248, 248, 240, 255});
        texto("BOLSA", 32.0f, 44.0f, FONTE_PX, BLACK);
        for (i = 0; i < ITEM_CONTAGEM; i++) {
            char linha[48];
            Color tinta = (vis.cursor_bolsa == i) ? (Color){180, 32, 32, 255} : BLACK;
            snprintf(linha, sizeof linha, "%s x%d", casoNomeItem(i), casoQuantidadeItem(&caso_jogo, i));
            texto(linha, 32.0f, 62.0f + (float)i * 15.0f, FONTE_PX, tinta);
        }
            texto("Z usa. X volta.", 32.0f, 128.0f, FONTE_PX, (Color){80, 80, 80, 255});
    } else if (vis.fase == BAT_FIM) {
        desenharMensagem(vis.fala, 0, 112, VIRT_L, 48);
    } else {
        desenharMensagem(textoBatalha(), 0, 112, VIRT_L, 48);
    }

    if (vis.fase == BAT_TIME) {
        int n = tamanhoTime(&caso_jogo.nucleo.time);
        int ativo_idx = posicaoDoAtivo(&caso_jogo.nucleo.time);
        const Pokemon *sel_pk = pokemonNaPosicao(&caso_jogo.nucleo.time, vis.cursor_time);
        char buf[64];

        DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){4, 6, 16, 220});
        caixa(4, 4, 232, 152, (Color){16, 24, 38, 255});
        DrawRectangleLines(4, 4, 232, 152, (Color){50, 90, 140, 255});

        /* Ficha do Pokémon */
        caixa(8, 8, 98, 144, (Color){10, 18, 30, 255});
        DrawRectangleLines(8, 8, 98, 144, (Color){30, 80, 120, 255});
        DrawRectangle(11, 11, 92, 54, (Color){14, 24, 40, 255});
        DrawRectangleLines(11, 11, 92, 54, (Color){24, 60, 90, 255});

        if (sel_pk != NULL) {
            float bob = sinf(vis.tempo * 3.5f) * 1.5f;
            desenharSpriteMon(sel_pk->numero, 0, (Rectangle){29, 7 + bob, 56, 56}, 0.0f);

            if (sel_pk->brilhante) {
                int s;
                for (s = 0; s < 3; s++) {
                    float ang = vis.tempo * 4.0f + (float)s * 2.094f;
                    DrawCircle((int)(57.0f + cosf(ang) * 20.0f), (int)(38.0f + sinf(ang) * 14.0f), 1.5f, GOLD);
                }
            }

            nomeNaFicha(sel_pk->nome, 12, 68, 90, WHITE);
            snprintf(buf, sizeof buf, "Nv%d", sel_pk->nivel);
            texto(buf, 12, 78, FONTE_PX, (Color){248, 220, 120, 255});

            DrawRectangle(46, 77, 28, 9, corDoTipo(sel_pk->tipo1));
            textoAte(nomeTipo(sel_pk->tipo1), 48, 78, 24, WHITE);
            if (sel_pk->tipo2 != TIPO_NENHUM) {
                DrawRectangle(76, 77, 28, 9, corDoTipo(sel_pk->tipo2));
                textoAte(nomeTipo(sel_pk->tipo2), 78, 78, 24, WHITE);
            }

            snprintf(buf, sizeof buf, "HP %d/%d", sel_pk->hp_atual, sel_pk->hp_max);
            texto(buf, 12, 89, FONTE_PX, (Color){200, 220, 240, 255});
            desenharBarraHp(12, 98, 90, sel_pk->hp_max > 0 ? (float)sel_pk->hp_atual / (float)sel_pk->hp_max : 0.0f);

            snprintf(buf, sizeof buf, "A%d D%d", sel_pk->ataque, sel_pk->defesa);
            textoAte(buf, 12, 107, 92, (Color){170, 200, 230, 255});
            snprintf(buf, sizeof buf, "VEL %d", sel_pk->velocidade);
            textoAte(buf, 12, 117, 92, (Color){170, 200, 230, 255});

            if (sel_pk->qtd_golpes > 0) {
                snprintf(buf, sizeof buf, "1:%s", sel_pk->golpes[0].nome);
                textoAte(buf, 12, 128, 92, (Color){220, 230, 240, 255});
            }
            if (sel_pk->qtd_golpes > 1) {
                snprintf(buf, sizeof buf, "2:%s", sel_pk->golpes[1].nome);
                textoAte(buf, 12, 138, 92, (Color){220, 230, 240, 255});
            }
        } else {
            texto("Sem Pokemon", 14, 50, FONTE_PX, (Color){100, 120, 140, 255});
        }

        /* Quem entra na luta */
        texto("ESCOLHA O TIME", 112, 10, FONTE_PX, (Color){80, 210, 255, 255});

        for (i = 0; i < n && i < 6; i++) {
            const Pokemon *membro = pokemonNaPosicao(&caso_jogo.nucleo.time, i);
            float razao = 0.0f;
            int y = 22 + i * 19;
            int sel = (vis.cursor_time == i);
            int e_ativo = (i == ativo_idx);
            char nv[16];
            if (membro == NULL) continue;
            if (membro->hp_max > 0) razao = (float)membro->hp_atual / (float)membro->hp_max;

            caixa(110, y, 122, 18, sel ? (Color){30, 60, 110, 255} : (Color){20, 30, 46, 255});
            DrawRectangleLines(110, y, 122, 18, sel ? GOLD : (Color){36, 50, 72, 255});

            if (sprites[membro->numero].tem_icone) {
                DrawTexturePro(sprites[membro->numero].icone,
                               (Rectangle){0, 0, (float)sprites[membro->numero].icone.width, (float)sprites[membro->numero].icone.height},
                               (Rectangle){112, (float)y + 2, 14, 14}, (Vector2){0, 0}, 0.0f, WHITE);
            }
            textoAte(membro->nome, 128, (float)y + 2, 60, sel ? (Color){255, 230, 100, 255} : WHITE);
            snprintf(nv, sizeof nv, "N%d", membro->nivel);
            texto(nv, 192, (float)y + 2, FONTE_PX, (Color){248, 220, 120, 255});

            if (e_ativo) {
                texto("ATIVO", 128, (float)y + 10, FONTE_PX, (Color){60, 220, 140, 255});
            } else if (membro->hp_atual <= 0) {
                texto("[FNT]", 128, (float)y + 10, FONTE_PX, (Color){240, 80, 80, 255});
            }

            desenharBarraHp(176, y + 11, 48, razao);
        }

        textoAte("Z:enviar X:sair", 112, 140, 120, (Color){180, 200, 220, 255});
    }
    if (vis.fase == BAT_ANALISE) {
        const Pokemon *alvo = pokemonNaPosicao(&caso_jogo.oponente_atual.time, vis.cursor_time);
        if (alvo != NULL) {
            float scan_y = 38.0f + sinf(vis.tempo * 3.8f) * 26.0f + 26.0f;
            char linha[72];

            /* Scanner */
            DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){6, 14, 28, 240});

            texto("SCANNER POKEDEX", 14, 6, FONTE_PX, (Color){40, 220, 240, 255});
            texto("ATIVO", 184, 6, FONTE_PX, (Color){80, 240, 160, 255});

            caixa(8, 17, 96, 124, (Color){10, 22, 34, 255});
            DrawRectangleLines(8, 17, 96, 124, (Color){30, 80, 110, 255});

            DrawLine(12, 21, 20, 21, (Color){60, 240, 220, 255});
            DrawLine(12, 21, 12, 29, (Color){60, 240, 220, 255});
            DrawLine(100, 21, 92, 21, (Color){60, 240, 220, 255});
            DrawLine(100, 21, 100, 29, (Color){60, 240, 220, 255});
            DrawLine(12, 137, 20, 137, (Color){60, 240, 220, 255});
            DrawLine(12, 137, 12, 129, (Color){60, 240, 220, 255});
            DrawLine(100, 137, 92, 137, (Color){60, 240, 220, 255});
            DrawLine(100, 137, 100, 129, (Color){60, 240, 220, 255});

            DrawCircleLines(56, 67, 32.0f, (Color){40, 180, 200, 120});
            DrawCircleLines(56, 67, 20.0f, (Color){40, 180, 200, 80});
            DrawLine(56, 26, 56, 108, (Color){40, 180, 200, 70});
            DrawLine(16, 67, 96, 67, (Color){40, 180, 200, 70});

            DrawLine(14, (int)scan_y, 98, (int)scan_y, (Color){80, 255, 210, 200});

            desenharSpriteMon(alvo->numero, 0, (Rectangle){24, 35, 64, 64}, 0.0f);

            snprintf(linha, sizeof linha, "ID: #%03d", alvo->numero);
            texto(linha, 14, 114, FONTE_PX, (Color){160, 210, 240, 255});
            texto("ALVO FIXO", 14, 126, FONTE_PX, (Color){80, 240, 160, 255});

            caixa(108, 17, 124, 124, (Color){14, 22, 36, 255});
            DrawRectangleLines(108, 17, 124, 124, (Color){40, 70, 100, 255});

            snprintf(linha, sizeof linha, "%s%s", alvo->nome, alvo->brilhante ? " *" : "");
            nomeNaFicha(linha, 114, 23, 112, alvo->brilhante ? GOLD : WHITE);

            DrawRectangle(114, 35, 54, 11, corDoTipo(alvo->tipo1));
            nomeNaFicha(nomeTipo(alvo->tipo1), 116, 36, 50, WHITE);
            if (alvo->tipo2 != TIPO_NENHUM) {
                DrawRectangle(170, 35, 54, 11, corDoTipo(alvo->tipo2));
                nomeNaFicha(nomeTipo(alvo->tipo2), 172, 36, 50, WHITE);
            }

            snprintf(linha, sizeof linha, "N%d HP%d/%d", alvo->nivel, alvo->hp_atual, alvo->hp_max);
            texto(linha, 114, 50, FONTE_PX, (Color){248, 220, 120, 255});
            desenharBarraHp(114, 60, 112, (float)alvo->hp_atual / (float)alvo->hp_max);

            snprintf(linha, sizeof linha, "A%d D%d V%d", ataqueEmBatalha(alvo),
                     defesaEmBatalha(alvo), velocidadeEmBatalha(alvo));
            textoAte(linha, 114, 70, 112, (Color){180, 210, 240, 255});

            texto("GOLPES:", 114, 83, FONTE_PX, (Color){130, 160, 190, 255});
            for (i = 0; i < alvo->qtd_golpes && i < 3; i++) {
                nomeNaFicha(alvo->golpes[i].nome, 112, 93 + i * 10, 116, WHITE);
            }

            texto("< >: alvo  Z: duelo", 14, 146, FONTE_PX, (Color){200, 220, 240, 255});
        }
    }
}

static void textoCentral(const char *rotulo, int y, float tamanho, Color tinta) {
    float x = floorf((VIRT_L - larguraTexto(rotulo, tamanho)) / 2.0f);
    texto(rotulo, x, y, tamanho, tinta);
}

static void painelCaso(int x, int y, int largura, int altura) {
    DrawRectangle(x, y, largura, altura, painel_caso);
    DrawRectangleLines(x, y, largura, altura, borda_caso);
}

static void desenharFachada(int deslocamento, float tempo) {
    int base = 112 + deslocamento;
    int quadro = (int)(tempo * 5.0f) % 3;
    Color ceu = {9, 15, 32, 255};
    Color parede = {29, 43, 65, 255};
    Color metal = {53, 69, 95, 255};
    Color janela = ((int)(tempo * 1.3f) % 5 == 4)
                  ? psiquico_caso : (Color){102, 88, 150, 255};

    DrawRectangle(0, 0, VIRT_L, VIRT_A, ceu);
    DrawCircle(204, 40 + deslocamento, 21, (Color){31, 39, 66, 255});
    DrawCircle(211, 34 + deslocamento, 17, ceu);
    for (int i = 0; i < 12; i++) {
        int x = i * 23 - 8;
        DrawTriangle((Vector2){x, base + 8}, (Vector2){x + 11, base - 35 - (i % 3) * 5},
                     (Vector2){x + 23, base + 8}, (Color){13, 24, 43, 255});
    }
    DrawRectangle(0, base, VIRT_L, VIRT_A - base, (Color){14, 25, 42, 255});
    DrawRectangle(27, base - 44, 186, 47, parede);
    DrawRectangle(22, base - 47, 196, 4, metal);
    DrawRectangle(26, base - 43, 188, 2, fundo_caso);
    DrawRectangle(62, base - 59, 116, 17, parede);
    DrawRectangle(59, base - 61, 121, 3, metal);
    DrawRectangle(115, base - 65, 3, 5, metal);
    DrawRectangle(114, base - 66, 5, 2, destaque_caso);
    for (int i = 0; i < 6; i++) {
        int x = 37 + i * 29 + (i >= 3 ? 8 : 0);
        if (i == 2 || i == 3) continue;
        DrawRectangle(x, base - 33, 20, 15, fundo_caso);
        DrawRectangle(x + 2, base - 31, 16, 10, i == 4 ? janela : metal);
        DrawRectangle(x + 9, base - 31, 2, 10, parede);
        DrawRectangle(x - 1, base - 19, 22, 2, metal);
    }
    DrawRectangle(96, base - 25, 48, 29, metal);
    DrawRectangle(100, base - 22, 40, 25, fundo_caso);
    DrawRectangle(103, base - 21, 33, 23, (Color){20, 30, 48, 255});
    DrawRectangle(104, base - 27, 30, 2, destaque_caso);
    DrawRectangle(118, base - 18, 2, 19, metal);
    DrawRectangle(109, base + 4, 22, 2, metal);
    DrawLine(108, base + 6, 90, base + 25, metal);
    DrawLine(132, base + 6, 150, base + 25, metal);
    for (int i = 0; i < 11; i++) {
        int x = (i * 73 + 5) % VIRT_L;
        DrawRectangle(x, base + 7 + (i * 7) % 20, 4 + i % 7, 1, (Color){39, 62, 87, 255});
    }
    int aproxima = deslocamento < 0 ? (int)fminf(14.0f, tempo * 4.0f) : 0;
    desenharFolha(detetive_tex, detetive_ok, 3, aproxima > 0 && aproxima < 14 ? quadro : 0,
                  113, base - 7 - aproxima);
    desenharPikachuOw(3, quadro, 134, base - 4 - aproxima);
    for (int i = 0; i < 55; i++) {
        int x = (i * 53 + 11 + (int)(tempo * 18)) % VIRT_L;
        int y = (i * 47 + (int)(tempo * 68)) % (base + 27);
        DrawLine(x, y, x - 2, y + 3, (Color){113, 156, 208, 80});
    }
}

static void desenharTitulo(void) {
    float tempo = movimento_reduzido ? 0.0f : vis.tempo;
    const char *titulo = "PokéBattle";

    desenharFachada(0, tempo);
    textoCentral(titulo, 14, 16, BLACK);
    textoCentral(titulo, 12, 16, destaque_caso);
    textoCentral("O Caso do Laboratório", 33, FONTE_PX, texto_caso);
    DrawRectangle(54, 141, 132, 15, fundo_caso);
    DrawRectangleLines(54, 141, 132, 15, borda_caso);
    textoCentral("Z: abrir menu", 145, FONTE_PX, destaque_caso);
}

static Rectangle botaoMenu(int indice) {
    return (Rectangle){105, 29 + indice * 18, 121, 16};
}

static void desenharMenu(void) {
    static const char *rotulos[MENU_TOTAL] = {
        "Iniciar caso", "Como jogar", "Pokédex", "Ajustes", "Créditos", "Sair"
    };
    Color papel = {225, 213, 180, 255};
    Color tinta = {28, 36, 52, 255};

    desenharFachada(0, movimento_reduzido ? 0.0f : vis.tempo);
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){5, 10, 21, 207});
    DrawRectangle(10, 12, 82, 17, papel);
    DrawRectangle(10, 23, 222, 116, papel);
    DrawRectangle(98, 23, 1, 116, (Color){162, 148, 120, 255});
    texto("ARQUIVO", 18, 16, FONTE_PX, tinta);
    DrawRectangle(19, 32, 70, 77, (Color){244, 234, 207, 255});
    DrawRectangle(22, 35, 64, 62, (Color){38, 54, 76, 255});
    desenharSpriteMon(25, 0, (Rectangle){23, 32, 62, 62}, 0.0f);
    DrawRectangle(27, 101, 54, 2, (Color){177, 161, 129, 255});
    texto("CASO 01", 26, 120, FONTE_PX, tinta);

    for (int i = 0; i < MENU_TOTAL; i++) {
        Rectangle b = botaoMenu(i);
        Color fundo = i == vis.menu_cursor ? destaque_caso : (Color){238, 227, 195, 255};
        DrawRectangleRec(b, fundo);
        if (i == vis.menu_cursor) DrawRectangle((int)b.x, (int)b.y, 3, (int)b.height, tinta);
        texto(rotulos[i], b.x + 10, b.y + 4, FONTE_PX, tinta);
    }
    textoCentral("Z: abrir   X: voltar", 147, FONTE_PX, texto_caso);
    if (vis.tempo_menu < 0.2f && !movimento_reduzido) {
        int largura = (int)(232.0f * (1.0f - vis.tempo_menu / 0.2f));
        DrawRectangle(232 - largura, 23, largura, 116, papel);
    }
}

static void desenharIntro(void) {
    static const char *locais[INTRO_TOTAL] = {
        "CONTENÇÃO", "RÁDIO", "CÂMERA INTERNA", "ENTRADA"
    };
    const char *fala = historiaIntro(vis.cena_intro);
    float tempo = movimento_reduzido ? 0.0f : vis.tempo_intro;
    char linhas[3][48];
    int quantidade;

    DrawRectangle(0, 0, VIRT_L, VIRT_A, fundo_caso);
    if (vis.cena_intro == INTRO_ALERTA || vis.cena_intro == INTRO_RADIO) {
        Color sinal = vis.cena_intro == INTRO_ALERTA ? (Color){252, 130, 115, 255}
                                                     : destaque_caso;
        painelCaso(18, 25, 204, 80);
        DrawRectangle(26, 33, 188, 64, fundo_caso);
        for (int y = 38; y < 96; y += 5) {
            DrawLine(27, y, 212, y, (Color){30, 46, 70, 255});
        }
        if (vis.cena_intro == INTRO_ALERTA) {
            DrawRectangleLines(112, 42, 16, 16, sinal);
            texto("!", 116, 46, FONTE_PX, sinal);
            textoCentral("SINAL PERDIDO", 75, FONTE_PX, sinal);
        } else {
            for (int i = 0; i < 25; i++) {
                int altura = 3 + (int)(fabsf(sinf(i * 0.49f + tempo * 4.0f)) * (5 + i % 9));
                DrawRectangle(46 + i * 6, 63 - altura, 2, altura * 2, sinal);
            }
        }
    } else if (vis.cena_intro == INTRO_TANQUE) {
        DrawRectangle(0, 18, VIRT_L, 90, (Color){18, 14, 37, 255});
        DrawRectangle(75, 28, 90, 70, (Color){45, 30, 73, 255});
        DrawRectangle(80, 34, 80, 58, (Color){73, 48, 112, 255});
        DrawEllipse(120, 86, 38.0f, 6.0f, (Color){94, 61, 143, 255});
        desenharSpriteMonSilhueta(150, (Rectangle){89, 31 + floorf(sinf(tempo * 1.5f) * 2), 62, 62});
        DrawRectangle(80, 34, 2, 58, psiquico_caso);
        DrawRectangle(158, 34, 2, 58, psiquico_caso);
        DrawRectangle(73, 96, 94, 5, borda_caso);
    } else {
        desenharFachada(-28, tempo);
        if (!movimento_reduzido && tempo > 3.4f) {
            float escuro = fminf(0.85f, (tempo - 3.4f) / 1.6f);
            DrawRectangle(0, 0, VIRT_L, 108, Fade(BLACK, escuro));
        }
    }

    DrawRectangle(0, 0, VIRT_L, 18, fundo_caso);
    texto(locais[vis.cena_intro], 9, 5, FONTE_PX, texto_secundario);
    for (int i = 0; i < INTRO_TOTAL; i++) {
        DrawRectangle(188 + i * 11, 8, 7, 2, i <= (int)vis.cena_intro ? destaque_caso : borda_caso);
    }
    DrawRectangle(0, 109, VIRT_L, 51, fundo_caso);
    painelCaso(5, 110, 230, 32);
    quebrar(fala, linhas, &quantidade, 3, 210.0f);
    for (int i = 0; i < quantidade; i++) {
        texto(linhas[i], 12, 114 + i * 9, FONTE_PX, texto_caso);
    }
    textoCentral("Z: avançar  X: pular", 148, FONTE_PX, texto_secundario);
}

static Rectangle linhaPainel(int indice) {
    return (Rectangle){17, 39 + indice * 19, 206, 17};
}

static void desenharLinhaPainel(int indice, int cursor, const char *rotulo, const char *valor) {
    Rectangle caixa = linhaPainel(indice);
    Color tinta = indice == cursor ? fundo_caso : texto_caso;

    DrawRectangleRec(caixa, indice == cursor ? destaque_caso : fundo_caso);
    texto(rotulo, caixa.x + 6, caixa.y + 4, FONTE_PX, tinta);
    if (valor != NULL) {
        texto(valor, caixa.x + caixa.width - 6 - larguraTexto(valor, FONTE_PX),
              caixa.y + 4, FONTE_PX, tinta);
    }
}

static void desenharAjustes(void) {
    char porcentagem[8];

    DrawRectangle(0, 0, VIRT_L, VIRT_A, fundo_caso);
    painelCaso(8, 8, 224, 144);
    texto("AJUSTES", 17, 17, FONTE_PX, destaque_caso);
    snprintf(porcentagem, sizeof porcentagem, "%d%%", volume);
    desenharLinhaPainel(0, vis.ajustes_cursor, "Volume", porcentagem);
    desenharLinhaPainel(1, vis.ajustes_cursor, "Menos movimento", movimento_reduzido ? "Sim" : "Não");
    desenharLinhaPainel(2, vis.ajustes_cursor, "Tela cheia",
                        IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE) ? "Sim" : "Não");
    desenharLinhaPainel(3, vis.ajustes_cursor, "Voltar", NULL);
    texto("< >: mudar   X: voltar", 17, 128, FONTE_PX, texto_secundario);
}

static void desenharPausa(void) {
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){5, 10, 21, 216});
    painelCaso(8, 8, 224, 144);
    texto(vis.confirmar_menu ? "VOLTAR AO MENU?" : "PAUSA", 17, 17, FONTE_PX, destaque_caso);
    if (vis.confirmar_menu) {
        desenharLinhaPainel(0, vis.pausa_cursor, "Ficar no caso", NULL);
        desenharLinhaPainel(1, vis.pausa_cursor, "Encerrar caso", NULL);
        texto("Progresso será perdido.", 17, 95, FONTE_PX, texto_secundario);
    } else {
        desenharLinhaPainel(0, vis.pausa_cursor, "Continuar", NULL);
        desenharLinhaPainel(1, vis.pausa_cursor, "Ajustes", NULL);
        desenharLinhaPainel(2, vis.pausa_cursor, "Menu principal", NULL);
        if (tela_pausada == TELA_SALA) {
            texto("P Pokédex   M Mapa", 17, 107, FONTE_PX, texto_secundario);
            texto("N Caderno   T Equipe", 17, 119, FONTE_PX, texto_secundario);
        }
    }
    texto("Z: confirmar   X: voltar", 17, 138, FONTE_PX, texto_secundario);
}

static void desenharAjuda(void) {
    DrawRectangle(0, 0, VIRT_L, VIRT_A, fundo_caso);
    painelCaso(8, 6, 224, 148);
    texto("COMO JOGAR", 16.0f, 12.0f, FONTE_PX, destaque_caso);
    texto("Setas/WASD: andar", 16.0f, 26.0f, FONTE_PX, texto_caso);
    texto("Z: agir  X: cancelar", 16.0f, 37.0f, FONTE_PX, texto_caso);
    texto("P: Pokédex com sprites", 16.0f, 48.0f, FONTE_PX, texto_caso);
    texto("M: Minimapa tático", 16.0f, 59.0f, FONTE_PX, texto_caso);
    texto("L: lanterna  N: caderno", 16.0f, 70.0f, FONTE_PX, texto_caso);
    texto("R: documento  T: time", 16.0f, 81.0f, FONTE_PX, texto_caso);
    texto("V: sala anterior", 16.0f, 92.0f, FONTE_PX, texto_caso);
    texto("Matos/escombros: treino", 16.0f, 103.0f, FONTE_PX, texto_caso);
    texto("Bancos: recuperar time", 16.0f, 114.0f, FONTE_PX, texto_caso);
    texto("Puzzles: R reinicia", 16.0f, 125.0f, FONTE_PX, texto_caso);
    texto("U desfaz o passo", 16.0f, 136.0f, FONTE_PX, texto_caso);
    texto("X: voltar", 16.0f, 145.0f, FONTE_PX, texto_secundario);
}

static void desenharCreditos(void) {
    DrawRectangle(0, 0, VIRT_L, VIRT_A, fundo_caso);
    painelCaso(8, 6, 224, 148);
    texto("PokéBattle", 16.0f, 12.0f, FONTE_PX, (Color){255, 214, 64, 255});
    texto("O Caso do Laboratório", 16.0f, 22.0f, FONTE_PX, (Color){248, 248, 236, 255});
    texto("João Pedro Araújo Nóbrega", 16.0f, 40.0f, FONTE_PX, (Color){248, 248, 236, 255});
    texto("Leonardo Matos", 16.0f, 52.0f, FONTE_PX, (Color){248, 248, 236, 255});
    texto("Algoritmos e", 16.0f, 66.0f, FONTE_PX, (Color){180, 200, 230, 255});
    texto("Estruturas de Dados", 16.0f, 76.0f, FONTE_PX, (Color){180, 200, 230, 255});
    texto("CESAR School", 16.0f, 90.0f, FONTE_PX, (Color){180, 200, 230, 255});
    texto("2026.2", 16.0f, 102.0f, FONTE_PX, (Color){180, 200, 230, 255});
    texto("PokéAPI", 16.0f, 116.0f, FONTE_PX, (Color){220, 220, 210, 255});
    texto("Nintendo / Game Freak", 16.0f, 126.0f, FONTE_PX, (Color){220, 220, 210, 255});
    texto("Z volta ao menu", 16.0f, 140.0f, FONTE_PX, (Color){255, 214, 64, 255});
}

static void desenharGameOver(void) {
    const char *opcoes[] = {"TENTAR DE NOVO", "MENU"};
    int i;

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){24, 8, 12, 255});
    texto("Seu time desmaiou.", 16.0f, 36.0f, FONTE_PX, (Color){248, 220, 210, 255});
    for (i = 0; i < 2; i++) {
        int selecionado = vis.menu_cursor == i;
        caixa(40, 70 + i * 28, 160, 22, selecionado ? (Color){255, 214, 64, 255} : (Color){248, 248, 240, 255});
        texto(opcoes[i], 48.0f, 76.0f + (float)i * 28.0f, FONTE_PX, BLACK);
    }
}

static void visitarPk(const Pokemon *pokemon, void *contexto) {
    ColetaPk *coleta = contexto;
    if (pokemon != NULL && coleta->n < (int)(sizeof coleta->itens / sizeof coleta->itens[0])) {
        coleta->itens[coleta->n++] = pokemon;
    }
}

static void desenharPokedex(void) {
    ColetaPk coleta;
    int j;
    const Pokemon *sel = NULL;
    char buf[64];
    int total_vistos = 0;
    int descoberto = 0;

    memset(&coleta, 0, sizeof coleta);
    percorrerEmOrdem(&caso_jogo.nucleo.arvore, visitarPk, &coleta);

    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){10, 14, 24, 230});
    /* Pokédex */
    caixa(4, 4, 232, 152, (Color){192, 36, 44, 255});
    DrawRectangleLines(4, 4, 232, 152, (Color){120, 20, 26, 255});

    DrawCircle(18, 14, 7, (Color){60, 160, 240, 255});
    DrawCircle(16, 12, 2, (Color){230, 245, 255, 255});
    DrawCircleLines(18, 14, 7, WHITE);
    DrawCircle(32, 12, 3, (Color){240, 70, 70, 255});
    DrawCircle(40, 12, 3, (Color){240, 220, 60, 255});
    DrawCircle(48, 12, 3, (Color){60, 220, 90, 255});

    texto("POKEDEX", 58, 10, FONTE_PX, WHITE);

    if (coleta.n <= 0) {
        caixa(16, 30, 208, 100, (Color){248, 248, 240, 255});
        texto("Nenhum registro encontrado.", 28, 70, FONTE_PX, BLACK);
        texto("[X/ESC] Fechar", 28, 142, FONTE_PX, WHITE);
        return;
    }

    for (j = 0; j < coleta.n; j++) {
        if (coleta.itens[j] != NULL && casoFoiVisto(&caso_jogo, coleta.itens[j]->numero)) {
            total_vistos++;
        }
    }

    if (vis.pokedex_sel < 0) vis.pokedex_sel = 0;
    if (vis.pokedex_sel >= coleta.n) vis.pokedex_sel = coleta.n - 1;

    if (vis.pokedex_sel < vis.pokedex_topo) vis.pokedex_topo = vis.pokedex_sel;
    if (vis.pokedex_sel >= vis.pokedex_topo + 4) vis.pokedex_topo = vis.pokedex_sel - 3;
    if (vis.pokedex_topo < 0) vis.pokedex_topo = 0;

    snprintf(buf, sizeof buf, "V:%d/%d", total_vistos, coleta.n);
    texto(buf, 144, 10, FONTE_PX, (Color){248, 220, 120, 255});

    sel = coleta.itens[vis.pokedex_sel];
    descoberto = (sel != NULL && casoFoiVisto(&caso_jogo, sel->numero));

    caixa(10, 23, 96, 120, (Color){216, 224, 216, 255});
    DrawRectangleLines(10, 23, 96, 120, (Color){150, 160, 150, 255});
    DrawRectangle(13, 26, 90, 66, (Color){188, 208, 188, 255});
    DrawRectangleLines(13, 26, 90, 66, (Color){140, 160, 140, 255});

    if (sel != NULL) {
        if (descoberto) {
            desenharSpriteMon(sel->numero, 0, (Rectangle){26, 27, 64, 64}, 0.0f);

            snprintf(buf, sizeof buf, "#%03d", sel->numero);
            texto(buf, 14, 96, FONTE_PX, (Color){30, 48, 36, 255});
            nomeNaFicha(sel->nome, 14, 106, 88, (Color){12, 24, 16, 255});

            {
                Color c1 = corDoTipo(sel->tipo1);
                DrawRectangle(14, 122, 42, 13, c1);
                DrawRectangleLines(14, 122, 42, 13, (Color){30, 30, 30, 200});
                textoAte(nomeTipo(sel->tipo1), 16, 125, 38, WHITE);
                if (sel->tipo2 != TIPO_NENHUM) {
                    Color c2 = corDoTipo(sel->tipo2);
                    DrawRectangle(58, 122, 44, 13, c2);
                    DrawRectangleLines(58, 122, 44, 13, (Color){30, 30, 30, 200});
                    textoAte(nomeTipo(sel->tipo2), 60, 125, 40, WHITE);
                }
            }
        } else {
            desenharSpriteMonSilhueta(sel->numero, (Rectangle){26, 27, 64, 64});

            snprintf(buf, sizeof buf, "#%03d", sel->numero);
            texto(buf, 14, 96, FONTE_PX, (Color){50, 70, 60, 255});
            textoAte("NAO VISTO", 14, 106, 88, (Color){36, 56, 44, 255});

            DrawRectangle(14, 122, 42, 13, (Color){100, 115, 110, 255});
            DrawRectangleLines(14, 122, 42, 13, (Color){60, 75, 70, 255});
            texto("???", 26, 125, FONTE_PX, (Color){220, 235, 230, 255});
        }
    }

    /* Lista de espécies */
    caixa(110, 23, 122, 64, (Color){244, 246, 248, 255});
    DrawRectangleLines(110, 23, 122, 64, (Color){160, 170, 180, 255});

    for (j = 0; j < 4; j++) {
        int idx = vis.pokedex_topo + j;
        int y = 25 + j * 15;
        if (idx < coleta.n) {
            const Pokemon *p = coleta.itens[idx];
            int e_sel = (idx == vis.pokedex_sel);
            int p_visto = (p != NULL && casoFoiVisto(&caso_jogo, p->numero));
            if (e_sel) {
                DrawRectangle(112, y, 118, 14, (Color){36, 80, 170, 255});
            }
            if (p != NULL) {
                if (p_visto) {
                    if (sprites[p->numero].tem_icone) {
                        DrawTexturePro(sprites[p->numero].icone,
                                       (Rectangle){0, 0, (float)sprites[p->numero].icone.width,
                                                   (float)sprites[p->numero].icone.height},
                                       (Rectangle){114, (float)y - 1, 16, 14}, (Vector2){0, 0}, 0.0f, WHITE);
                    }
                    snprintf(buf, sizeof buf, "%s#%03d %s", e_sel ? ">" : " ", p->numero, p->nome);
                    textoAte(buf, 130, (float)y + 3, 98, e_sel ? (Color){255, 230, 100, 255} : BLACK);
                } else {
                    DrawCircle(121, y + 6, 4, (Color){180, 190, 195, 255});
                    texto("?", 119, y + 2, FONTE_PX, (Color){70, 80, 90, 255});
                    snprintf(buf, sizeof buf, "%s#%03d ????????", e_sel ? ">" : " ", p->numero);
                    textoAte(buf, 130, (float)y + 3, 98, e_sel ? (Color){220, 235, 250, 255} : (Color){120, 130, 140, 255});
                }
            }
        }
    }

    /* Atributos e golpes */
    caixa(110, 90, 122, 53, (Color){20, 26, 40, 255});
    DrawRectangleLines(110, 90, 122, 53, (Color){70, 96, 140, 255});

    if (sel != NULL) {
        if (descoberto) {
            snprintf(buf, sizeof buf, "HP:%d A:%d", sel->hp_base, sel->ataque);
            texto(buf, 114, 93, FONTE_PX, (Color){248, 224, 120, 255});
            snprintf(buf, sizeof buf, "D:%d V:%d", sel->defesa, sel->velocidade);
            texto(buf, 114, 103, FONTE_PX, (Color){248, 224, 120, 255});
            if (sel->qtd_golpes > 0) {
                snprintf(buf, sizeof buf, "- %s (%d)", sel->golpes[0].nome, sel->golpes[0].poder);
                textoAte(buf, 114, 115, 114, WHITE);
            }
            if (sel->qtd_golpes > 1) {
                snprintf(buf, sizeof buf, "- %s (%d)", sel->golpes[1].nome, sel->golpes[1].poder);
                textoAte(buf, 114, 126, 114, (Color){200, 220, 240, 255});
            }
        } else {
            texto("HP:??? A:???", 114, 93, FONTE_PX, (Color){150, 165, 180, 255});
            texto("D:??? V:???", 114, 103, FONTE_PX, (Color){150, 165, 180, 255});
            texto("DADOS OCULTOS", 114, 115, FONTE_PX, (Color){240, 130, 80, 255});
            texto("Veja no mapa!", 114, 126, FONTE_PX, (Color){140, 160, 180, 255});
        }
    }

    texto("W/S: navegar  X: fechar", 18, 144, FONTE_PX, (Color){240, 240, 240, 255});
}

static void atualizarMewtwo(float dt) {
    vis.mewtwo_t += dt;

    if (vis.mewtwo_etapa == 0) {
        if (acaoPressionada()) {
            vis.mewtwo_etapa = 1;
            clique();
        }
    } else if (vis.mewtwo_etapa == 1) {
        if (acaoPressionada()) {
            vis.mewtwo_etapa = 2;
            vis.mewtwo_opcao = 0;
            clique();
        }
    } else if (vis.mewtwo_etapa == 2) {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            vis.mewtwo_opcao = (vis.mewtwo_opcao + 1) % 3;
            clique();
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            vis.mewtwo_opcao = (vis.mewtwo_opcao + 2) % 3;
            clique();
        }
        if (acaoPressionada()) {
            if (vis.mewtwo_opcao == 0 || vis.mewtwo_opcao == 2) {
                vis.mewtwo_convencido = 1;
            } else {
                vis.mewtwo_convencido = 0;
            }
            vis.mewtwo_etapa = 3;
            clique();
        }
    } else if (vis.mewtwo_etapa == 3) {
        if (acaoPressionada()) {
            if (vis.mewtwo_convencido) {
                casoConvidarMewtwo(&caso_jogo);
                if (vis.som_ok) PlaySound(vis.som_acerto);
            } else {
                clique();
            }
            vis.mewtwo_etapa = 4;
        }
    } else if (vis.mewtwo_etapa == 4) {
        if (acaoPressionada()) {
            tela = TELA_CREDITOS;
            vis.vitoria = 1;
            vis.menu_cursor = 0;
            clique();
        }
    }
}

static void desenharMewtwo(void) {
    float tempo = movimento_reduzido ? 0.0f : vis.mewtwo_t;
    float bob = sinf(tempo * 2.8f) * 3.5f;
    int i;

    /* Mewtwo */
    DrawRectangle(0, 0, VIRT_L, VIRT_A, (Color){10, 8, 20, 255});

    for (i = 0; i < 3; i++) {
        float r = 24.0f + (float)i * 18.0f + sinf(tempo * 2.0f + (float)i) * 4.0f;
        DrawCircleLines(VIRT_L / 2, 40, r, (Color){160, 90, 230, 45});
    }

    for (i = 0; i < 5; i++) {
        float ang = tempo * 2.5f + (float)i * 1.256f;
        int px = (int)((float)VIRT_L / 2.0f + cosf(ang) * (34.0f + (float)i * 5.0f));
        int py = (int)(40.0f + sinf(ang) * (20.0f + (float)i * 3.0f));
        DrawCircle(px, py, 1.5f, (Color){190, 140, 255, 200});
        DrawCircle(px, py, 0.8f, WHITE);
    }

    DrawEllipse(VIRT_L / 2, 74, 22, 6, (Color){20, 10, 32, 200});

    desenharSpriteMon(150, 0, (Rectangle){(float)VIRT_L / 2.0f - 32.0f, 8.0f + bob, 64.0f, 64.0f}, 0.0f);

    if (vis.mewtwo_etapa >= 3 && vis.mewtwo_convencido) {
        for (i = 0; i < 4; i++) {
            float ang = tempo * 3.5f + (float)i * 1.57f;
            int sx = (int)((float)VIRT_L / 2.0f + cosf(ang) * 26.0f);
            int sy = (int)(38.0f + sinf(ang) * 22.0f);
            DrawCircle(sx, sy, 2.0f, GOLD);
            DrawCircle(sx, sy, 1.0f, WHITE);
        }
    }

    /* Três respostas da história */
    painelCaso(6, 76, 228, 80);
    DrawRectangle(6, 76, 228, 1, psiquico_caso);
    texto("MEWTWO", 12, 82, FONTE_PX, psiquico_caso);

    if (vis.mewtwo_etapa == 2) {
        static const char *opcoes[] = {
            "Amigos. Venha conosco.",
            "Preciso do seu poder.",
            "Siga seu próprio caminho."
        };
        for (int op = 0; op < 3; op++) {
            int selecionado = vis.mewtwo_opcao == op;
            int y = 95 + op * 14;
            if (selecionado) DrawRectangle(11, y - 1, 218, 12, fundo_caso);
            texto(selecionado ? ">" : " ", 14, y, FONTE_PX, destaque_caso);
            texto(opcoes[op], 25, y, FONTE_PX, selecionado ? destaque_caso : texto_caso);
        }
        texto("W/S: escolher  Z: enviar", 12, 143, FONTE_PX, texto_secundario);
    } else {
        const char *fala;
        const char *acao;
        char linhas[4][48];
        int quantidade;

        if (vis.mewtwo_etapa == 0) {
            fala = "Giovanni recuou. Os cientistas tentaram me transformar numa arma.";
            acao = "Z: ouvir";
        } else if (vis.mewtwo_etapa == 1) {
            fala = "Vi você lutar ao lado dos seus parceiros. O que eles significam para você?";
            acao = "Z: responder";
        } else if (vis.mewtwo_etapa == 3) {
            fala = vis.mewtwo_convencido
                   ? "Nem todos os humanos são cruéis. Aceito o seu convite."
                   : "Você pensa como Giovanni. Não vou pertencer a nenhum humano.";
            acao = vis.mewtwo_convencido ? "Z: convidar" : "Z: continuar";
        } else {
            fala = vis.mewtwo_convencido
                   ? "Mewtwo entrou para a equipe. O caso está encerrado."
                   : "Mewtwo desapareceu. Seu paradeiro é um mistério.";
            acao = "Z: créditos";
        }
        quebrar(fala, linhas, &quantidade, 4, 216);
        for (int linha = 0; linha < quantidade; linha++) {
            texto(linhas[linha], 12, 95 + linha * 10, FONTE_PX, texto_caso);
        }
        texto(acao, 12, 143, FONTE_PX, destaque_caso);
    }
}

static void desenharTudo(void) {
    BeginTextureMode(alvo);
    ClearBackground(BLACK);
    if (tela == TELA_TITULO) desenharTitulo();
    else if (tela == TELA_MENU) desenharMenu();
    else if (tela == TELA_INTRO) desenharIntro();
    else if (tela == TELA_AJUSTES) desenharAjustes();
    else if (tela == TELA_PAUSA) {
        if (tela_pausada == TELA_SALA) desenharMundo();
        else desenharBatalha();
        desenharPausa();
    }
    else if (tela == TELA_AJUDA) desenharAjuda();
    else if (tela == TELA_POKEDEX) desenharPokedex();
    else if (tela == TELA_SALA) desenharMundo();
    else if (tela == TELA_CREDITOS) desenharCreditos();
    else if (tela == TELA_GAMEOVER) desenharGameOver();
    else if (tela == TELA_MEWTWO) desenharMewtwo();
    else desenharBatalha();
    if (tela == TELA_SALA && vis.entrada_setor > 0.0f) {
        DrawRectangle(0, 0, VIRT_L, VIRT_A, Fade(BLACK, vis.entrada_setor / 0.36f));
    }
    if (vis.transicao != 0 && !movimento_reduzido && tela != TELA_AJUSTES && tela != TELA_PAUSA) {
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

/* Escala inteira, centralizada */
static Rectangle calcularAreaJogo(int largura, int altura) {
    float escala = fminf((float)largura / VIRT_L, (float)altura / VIRT_A);
    Rectangle area;
    if (escala >= 1.0f) escala = floorf(escala);
    if (escala <= 0.0f) escala = 1.0f;
    area.width = VIRT_L * escala;
    area.height = VIRT_A * escala;
    area.x = floorf(((float)largura - area.width) * 0.5f);
    area.y = floorf(((float)altura - area.height) * 0.5f);
    return area;
}

/* Mouse na mesma escala da tela */
static Vector2 pontoNoJogo(Vector2 ponto, Rectangle area) {
    return (Vector2){(ponto.x - area.x) * VIRT_L / area.width,
                     (ponto.y - area.y) * VIRT_A / area.height};
}

static void apresentar(const char *png) {
    Rectangle origem = {0.0f, 0.0f, (float)VIRT_L, -(float)VIRT_A};
    Rectangle destino = calcularAreaJogo(GetScreenWidth(), GetScreenHeight());

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(alvo.texture, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
    if (png != NULL) {
        /* Segundo quadro para o screenshot no Windows */
        apresentar(NULL);
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

static Vector2 mouseNoJogo(void) {
    Rectangle area = calcularAreaJogo(GetScreenWidth(), GetScreenHeight());
    return pontoNoJogo(GetMousePosition(), area);
}

/* Teclado e clique na mesma lista */
static int selecionarOpcao(int *cursor, int total, Rectangle (*linha)(int)) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        *cursor = (*cursor + 1) % total;
        clique();
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        *cursor = (*cursor + total - 1) % total;
        clique();
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = mouseNoJogo();
        for (int i = 0; i < total; i++) {
            if (pontoNoRetangulo(linha(i), mouse)) {
                *cursor = i;
                return 1;
            }
        }
    }
    return acaoPressionada();
}

static void mudarCenaIntro(CenaIntro cena) {
    vis.cena_intro = cena;
    vis.tempo_intro = 0.0f;
    if (vis.som_ok) {
        if (cena == INTRO_ALERTA || cena == INTRO_RADIO) PlaySound(som_radio);
        if (cena == INTRO_TANQUE || cena == INTRO_ENTRADA) PlaySound(som_porta);
    }
}

static void iniciarPartida(int com_intro) {
    if (!comecarCaso(&caso_jogo)) return;
    zerarVisual();
    entrarNaSala();
    vis.intro_feita = 1;
    vis.salas_vistas[0] = 1;
    vis.dica_inicio = 1;
    tela = com_intro ? TELA_INTRO : TELA_SALA;
    if (com_intro) mudarCenaIntro(INTRO_ALERTA);
}

static void atualizarIntro(float dt) {
    vis.tempo_intro += dt;
    if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE) ||
        (vis.cena_intro == INTRO_ENTRADA &&
         (acaoPressionada() || vis.tempo_intro >= duracao_intro[INTRO_ENTRADA]))) {
        tela = TELA_SALA;
        vis.entrada_setor = movimento_reduzido ? 0.0f : 0.36f;
    } else if (acaoPressionada() || vis.tempo_intro >= duracao_intro[vis.cena_intro]) {
        mudarCenaIntro(vis.cena_intro + 1);
    }
}

static void abrirAjustes(Tela retorno) {
    retorno_ajustes = retorno;
    vis.ajustes_cursor = 0;
    tela = TELA_AJUSTES;
}

static void atualizarAjustes(void) {
    int esquerda = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
    int direita = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);
    int selecionou = selecionarOpcao(&vis.ajustes_cursor, 4, linhaPainel);

    if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE) || (selecionou && vis.ajustes_cursor == 3)) {
        tela = retorno_ajustes;
        clique();
        return;
    }
    if (!esquerda && !direita && !selecionou) return;
    switch (vis.ajustes_cursor) {
    case 0:
        volume += esquerda ? -10 : 10;
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;
        if (vis.som_ok) SetMasterVolume(volume / 100.0f);
        break;
    case 1:
        movimento_reduzido = !movimento_reduzido;
        break;
    case 2:
        ToggleBorderlessWindowed();
        break;
    }
    clique();
}

static void abrirPausa(void) {
    tela_pausada = tela;
    vis.pausa_cursor = 0;
    vis.confirmar_menu = 0;
    tela = TELA_PAUSA;
    clique();
}

static void atualizarPausa(void) {
    if (IsKeyPressed(KEY_ESCAPE) || voltarPressionado()) {
        if (vis.confirmar_menu) {
            vis.confirmar_menu = 0;
            vis.pausa_cursor = 0;
        } else {
            tela = tela_pausada;
        }
        clique();
        return;
    }
    if (!selecionarOpcao(&vis.pausa_cursor, vis.confirmar_menu ? 2 : 3, linhaPainel)) return;
    clique();
    if (vis.confirmar_menu) {
        if (vis.pausa_cursor == 1) {
            vis.menu_cursor = 0;
            tela = TELA_MENU;
        }
        vis.confirmar_menu = 0;
        vis.pausa_cursor = 0;
    } else if (vis.pausa_cursor == 0) {
        tela = tela_pausada;
    } else if (vis.pausa_cursor == 1) {
        abrirAjustes(TELA_PAUSA);
    } else {
        vis.confirmar_menu = 1;
        vis.pausa_cursor = 0;
    }
}

static void atualizarMenu(void) {
    static const int teclas[MENU_TOTAL] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX};

    if (voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) {
        tela = TELA_TITULO;
        clique();
        return;
    }
    for (int i = 0; i < MENU_TOTAL; i++) {
        if (IsKeyPressed(teclas[i])) vis.menu_cursor = i;
    }
    if (!selecionarOpcao(&vis.menu_cursor, MENU_TOTAL, botaoMenu)) return;

    clique();
    switch (vis.menu_cursor) {
    case MENU_INICIAR: iniciarPartida(1); break;
    case MENU_AJUDA: tela = TELA_AJUDA; break;
    case MENU_POKEDEX: tela = TELA_POKEDEX; break;
    case MENU_AJUSTES: abrirAjustes(TELA_MENU); break;
    case MENU_CREDITOS: tela = TELA_CREDITOS; break;
    case MENU_SAIR: vis.vitoria = -1; break;
    }
}

static void atualizar(float dt) {
    if (IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) &&
        (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))) {
        ToggleBorderlessWindowed();
        return;
    }
    if (dt > 0.05f) dt = 0.05f;
    if (tela == TELA_PAUSA) {
        atualizarPausa();
        return;
    }
    if (tela == TELA_AJUSTES) {
        atualizarAjustes();
        return;
    }
    vis.tempo += dt;
    if (tela == TELA_SALA) {
        vis.entrada_setor = fmaxf(0.0f, vis.entrada_setor - dt);
        vis.abertura_caderno = fmaxf(0.0f, vis.abertura_caderno - dt);
    }
    atualizarTransicao(dt);
    if (vis.transicao == 1) {
        return;
    }
    if (tela == TELA_TITULO) {
        if (acaoPressionada() || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            pontoNoRetangulo((Rectangle){54, 141, 132, 15}, mouseNoJogo()))) {
            vis.menu_cursor = 0;
            vis.tempo_menu = 0.0f;
            tela = TELA_MENU;
            clique();
        }
    } else if (tela == TELA_MENU) {
        vis.tempo_menu += dt;
        atualizarMenu();
    } else if (tela == TELA_INTRO) {
        atualizarIntro(dt);
    } else if (tela == TELA_AJUDA || tela == TELA_POKEDEX) {
        if (tela == TELA_POKEDEX) {
            ColetaPk coleta;
            memset(&coleta, 0, sizeof coleta);
            percorrerEmOrdem(&caso_jogo.nucleo.arvore, visitarPk, &coleta);
            if (coleta.n > 0) {
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                    vis.pokedex_sel = (vis.pokedex_sel + 1) % coleta.n;
                    clique();
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                    vis.pokedex_sel = (vis.pokedex_sel + coleta.n - 1) % coleta.n;
                    clique();
                }
            }
        }
        if (acaoPressionada() || IsKeyPressed(KEY_ESCAPE) || voltarPressionado() || (tela == TELA_POKEDEX && IsKeyPressed(KEY_P))) {
            tela = TELA_MENU;
        }
    } else if (tela == TELA_SALA) {
        if (IsKeyPressed(KEY_ESCAPE) && !vis.fala_aberta && !vis.caderno &&
            !vis.documento_aberto && !vis.puzzle_aberto && !vis.time_aberto &&
            !vis.minimapa_aberto && !vis.pokedex_aberta && !vis.pc_aberto) {
            abrirPausa();
            return;
        }
        atualizarMundo(dt);
    } else if (tela == TELA_BATALHA) {
        if (IsKeyPressed(KEY_ESCAPE)) abrirPausa();
        else atualizarBatalha(dt);
    } else if (tela == TELA_MEWTWO) {
        atualizarMewtwo(dt);
    } else if (tela == TELA_CREDITOS) {
        if (acaoPressionada() || voltarPressionado() || IsKeyPressed(KEY_ESCAPE)) {
            tela = TELA_MENU;
        }
    } else if (tela == TELA_GAMEOVER) {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            vis.menu_cursor = 1 - vis.menu_cursor;
        }
        if (acaoPressionada()) {
            if (vis.menu_cursor == 0) {
                iniciarPartida(0);
            } else {
                tela = TELA_MENU;
            }
        }
    }
}

int executarInterfaceRaylib(int tela_cheia) {
    if (!abrirGrafico(tela_cheia)) {
        fprintf(stderr, "Não foi possível abrir a janela.\n");
        return 1;
    }
    if (!iniciarCaso(&caso_jogo)) {
        fecharGrafico();
        return 1;
    }
    caso_ok = 1;
    zerarVisual();
    tela = TELA_TITULO;

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

/* Demo usa a mesma regra do puzzle */
static int resolverDemonstracao(void) {
    return simularPuzzleAtual(&caso_jogo);
}

static int irAteOChefe(void) {
    int passos = 0;

    while (passos < 16) {
        NoSala *sala = salaAtual(&caso_jogo.salas);

        if (sala == NULL) {
            return 0;
        }
        if (strcmp(sala->nome, "Sala do chefe") == 0) {
            casoDescansar(&caso_jogo);
            return casoIniciarBatalha(&caso_jogo);
        }
        if (sala->inicia_batalha && !sala->batalha_concluida) {
            if (!lutarAteVencer()) {
                return 0;
            }
            if (caso_jogo.captura_disponivel && !simularCaptura(&caso_jogo)) {
                return 0;
            }
        }
        if (!sala->resolvido) {
            const char *arquivo = sala->habilidade == HAB_FOGO ? "captura_calor.png" :
                                  sala->habilidade == HAB_AGUA ? "captura_reservatorios.png" : sala->desafio.tipo == DES_ESTANTES ? "captura_estantes.png" : "captura_rastros.png";
            entrarNaSala();
            tela = TELA_SALA;
            vis.puzzle_aberto = 1;
            vis.cursor_puzzle = sala->desafio.tipo == DES_GELO ? 4 : 0;
            vis.retorno_puzzle = 0;
            desenharTudo();
            apresentar(arquivo);
            vis.puzzle_aberto = 0;
            if (!resolverDemonstracao()) return 0;
        }
        if (sala->luzes_necessarias) {
            casoAcenderPonto(&caso_jogo, 0);
            casoAcenderPonto(&caso_jogo, 1);
        }
        if (sala->duelo_obrigatorio && !sala->duelo_concluido) {
            casoDescansar(&caso_jogo);
            if (!casoIniciarDuelo(&caso_jogo)) return 0;
            tela = TELA_BATALHA;
            prepararBatalha();
            vis.entrada = 0.35f;
            desenharTudo();
            apresentar(strcmp(sala->nome, "Oficina") == 0 ? "captura_engenheira.png" : "captura_vigia.png");
            if (!lutarAteVencer()) return 0;
        }
        if (strcmp(sala->nome, "Jaula") == 0) {
            entrarNaSala();
            tela = TELA_SALA;
            abrirDocumento();
            desenharTudo();
            apresentar("captura_documento.png");
            vis.documento_pagina = 1;
            desenharTudo();
            apresentar("captura_documento_2.png");
            vis.documento_aberto = 0;
            vis.caderno = 1;
            desenharTudo();
            apresentar("captura_caderno.png");
            vis.caderno = 0;
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

    if (!abrirGrafico(0)) {
        return 1;
    }
    if (!iniciarCaso(&caso_jogo)) {
        fecharGrafico();
        return 1;
    }
    caso_ok = 1;
    zerarVisual();
    tela = TELA_TITULO;
    desenharTudo();
    apresentar("captura_titulo.png");
    tela = TELA_MENU;
    vis.tempo_menu = 0.2f;
    desenharTudo();
    apresentar("captura_menu.png");
    tela = TELA_INTRO;
    for (int i = 0; i < INTRO_TOTAL; i++) {
        char captura[48];
        vis.cena_intro = i;
        desenharTudo();
        snprintf(captura, sizeof captura, "captura_abertura_%d.png", i + 1);
        apresentar(captura);
    }

    if (!comecarCaso(&caso_jogo)) {
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    zerarVisual();
    entrarNaSala();
    tela = TELA_SALA;
    mundo.tx = 6;
    mundo.ty = 6;
    mundo.de_x = 6;
    mundo.de_y = 6;
    mundo.para_x = 6;
    mundo.para_y = 7;
    mundo.anda = 0.4f;
    mundo.movendo = 1;
    mundo.dir = 0;
    mundo.pika_x = 6;
    mundo.pika_y = 4;
    mundo.pika_de_x = 6;
    mundo.pika_de_y = 4;
    mundo.pika_anda = 0.4f;
    mundo.pika_dir = 0;
    vis.intro_feita = 1;
    abrirFala(historiaAoEntrar("Laboratório"));
    vis.texto_completo = 1;
    vis.bytes = 800;
    desenharTudo();
    apresentar("captura_intro_fala.png");
    vis.fala_aberta = 0;
    vis.pagina_n = 0;
    vis.salas_vistas[0] = 1;
    desenharTudo();
    apresentar("captura_sala.png");
    vis.lanterna_ligada = 1;
    desenharTudo();
    apresentar("captura_lanterna.png");
    tela_pausada = TELA_SALA;
    tela = TELA_PAUSA;
    desenharTudo();
    apresentar("captura_pausa.png");
    tela = TELA_AJUSTES;
    desenharTudo();
    apresentar("captura_ajustes.png");
    tela = TELA_SALA;
    mundo.movendo = 0;
    mundo.anda = 1.0f;
    mundo.pika_anda = 1.0f;

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
        if (strcmp(sala->nome, "Laboratório") == 0) {
            mundo.tx = 4;
            mundo.ty = 6;
            mundo.de_x = 4;
            mundo.de_y = 6;
            mundo.para_x = 4;
            mundo.para_y = 6;
            mundo.dir = 1;
            mundo.pika_x = 5;
            mundo.pika_y = 6;
            mundo.pika_de_x = 5;
            mundo.pika_de_y = 6;
            mundo.pika_dir = 1;
        }
        abrirFala(sala->texto_puzzle);
    }
    vis.texto_completo = 1;
    vis.bytes = 400;
    desenharTudo();
    apresentar("captura_puzzle.png");
    vis.fala_aberta = 0;

    vis.puzzle_aberto = 1;
    vis.cursor_puzzle = 0;
    vis.retorno_puzzle = 0;
    desenharTudo();
    apresentar("captura_circuito.png");
    vis.puzzle_aberto = 0;
    if (!resolverDemonstracao()) {
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    desenharTudo();
    apresentar("captura_gerador.png");
    casoAcenderPonto(&caso_jogo, 0);
    mundo.tx = mundo.de_x = mundo.para_x = 1;
    mundo.ty = mundo.de_y = mundo.para_y = 9;
    mundo.dir = 0;
    desenharTudo();
    apresentar("captura_refletor.png");
    casoAcenderPonto(&caso_jogo, 1);
    desenharTudo();
    apresentar("captura_energia.png");
    if (!casoAvancar(&caso_jogo)) {
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
    vis.entrada = 0.32f;
    vis.fase = BAT_ENTRADA;
    vis.texto_completo = 1;
    vis.bytes = 400;
    vis.transicao = 0;
    desenharTudo();
    apresentar("captura_intro.png");
    vis.entrada = 1.0f;
    vis.fase = BAT_COMANDO;
    desenharTudo();
    apresentar("captura_batalha.png");
    vis.fase = BAT_BOLSA;
    vis.cursor_bolsa = 0;
    desenharTudo();
    apresentar("captura_bolsa.png");
    {
        SaidaBatalha saida;
        filaLimpar();
        saida.narrar = narrarUi;
        saida.contexto = NULL;
        saida.log = &caso_jogo.nucleo.log;
        casoTentarFugir(&caso_jogo, &saida);
        vis.fase = BAT_TEXTO;
        vis.texto_completo = 1;
        vis.bytes = 400;
        vis.resultado = 3;
        desenharTudo();
        apresentar("captura_fuga.png");
        casoIniciarBatalha(&caso_jogo);
        prepararBatalha();
        vis.entrada = 1.0f;
        vis.fase = BAT_COMANDO;
        vis.transicao = 0;
    }

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
    casoResponderCaptura(&caso_jogo, 4, 0);
    vis.captura_t = 0.2f;
    vis.captura_animacao = 0.6f;
    desenharTudo();
    apresentar("captura_contencao.png");

    if (caso_jogo.captura_disponivel) {
        simularCaptura(&caso_jogo);
    }
    entrarNaSala();
    tela = TELA_SALA;
    desenharTudo();
    apresentar("captura_area_treino.png");
    if (!casoDescansar(&caso_jogo) || !casoIniciarTreino(&caso_jogo)) {
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    tela = TELA_BATALHA;
    prepararBatalha();
    vis.fase = BAT_COMANDO;
    vis.entrada = 1.0f;
    desenharTudo();
    apresentar("captura_treino.png");
    if (!lutarAteVencer()) {
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    if (caso_jogo.captura_disponivel) simularCaptura(&caso_jogo);
    if (!irAteOChefe()) {
        fprintf(stderr, "Não chegou na luta do Giovanni.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    tela = TELA_BATALHA;
    prepararBatalha();
    vis.entrada = 0.32f;
    vis.fase = BAT_ENTRADA;
    vis.texto_completo = 1;
    vis.bytes = 400;
    vis.transicao = 0;
    desenharTudo();
    apresentar("captura_giovanni_intro.png");
    vis.entrada = 1.0f;
    vis.fase = BAT_TEXTO;
    desenharTudo();
    apresentar("captura_boss.png");
    if (!lutarAteVencer()) {
        fprintf(stderr, "O chefe não caiu na demonstração.\n");
        encerrarCaso(&caso_jogo);
        fecharGrafico();
        return 1;
    }
    abrirPaginas(historiaDesfecho);
    vis.texto_completo = 1;
    vis.bytes = 800;
    vis.fase = BAT_FIM;
    desenharTudo();
    apresentar("captura_desfecho.png");
    tela = TELA_MEWTWO;
    vis.mewtwo_etapa = 2;
    desenharTudo();
    apresentar("captura_dialogo_mewtwo.png");
    tela = TELA_CREDITOS;
    desenharTudo();
    apresentar("captura_creditos.png");
    tela = TELA_GAMEOVER;
    vis.menu_cursor = 0;
    desenharTudo();
    apresentar("captura_gameover.png");

    encerrarCaso(&caso_jogo);
    caso_ok = 0;
    fecharGrafico();
    return 0;
}
