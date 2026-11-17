/*
 * dados_pokemon.c
 * ---------------
 * 20 espécies da 1ª geração e os cinco desafiantes da arena.
 * Golpes são representativos (não a lista oficial de level-up).
 * Ver dados_pokemon.h.
 */

#include "dados_pokemon.h"

#include "util.h"

#include <string.h>
#include <stdio.h>

static void adicionarGolpe(Pokemon *pokemon, const char *nome, Tipo tipo, int poder, int prioridade) {
    Golpe *golpe;

    if (pokemon == NULL || pokemon->qtd_golpes >= MAX_GOLPES) {
        return;
    }

    golpe = &pokemon->golpes[pokemon->qtd_golpes];
    copiarTexto(golpe->nome, sizeof golpe->nome, nome);
    golpe->tipo = tipo;
    golpe->poder = poder;
    golpe->prioridade = prioridade;
    pokemon->qtd_golpes++;
}

static Pokemon especie(int numero, const char *nome, Tipo tipo1, Tipo tipo2,
                       int hp, int ataque, int defesa, int velocidade) {
    Pokemon pokemon;

    memset(&pokemon, 0, sizeof pokemon);
    pokemon.numero = numero;
    copiarTexto(pokemon.nome, sizeof pokemon.nome, nome);
    pokemon.tipo1 = tipo1;
    pokemon.tipo2 = tipo2;
    pokemon.hp_base = hp;
    pokemon.ataque = ataque;
    pokemon.defesa = defesa;
    pokemon.velocidade = velocidade;
    return pokemon;
}

static int publicar(HashPokedex *pokedex, ArvorePokedex *arvore, const Pokemon *pokemon) {
    if (!inserirNaPokedex(pokedex, pokemon)) {
        return 0;
    }
    if (!inserirNaArvore(arvore, pokemon)) {
        return 0;
    }
    return 1;
}

static Tipo tipoDaApi(const char *nome) {
    if (strcmp(nome, "normal") == 0) return TIPO_NORMAL;
    if (strcmp(nome, "fire") == 0) return TIPO_FOGO;
    if (strcmp(nome, "water") == 0) return TIPO_AGUA;
    if (strcmp(nome, "electric") == 0) return TIPO_ELETRICO;
    if (strcmp(nome, "grass") == 0) return TIPO_GRAMA;
    if (strcmp(nome, "ice") == 0) return TIPO_GELO;
    if (strcmp(nome, "fighting") == 0) return TIPO_LUTADOR;
    if (strcmp(nome, "poison") == 0) return TIPO_VENENO;
    if (strcmp(nome, "ground") == 0) return TIPO_TERRA;
    if (strcmp(nome, "flying") == 0) return TIPO_VOADOR;
    if (strcmp(nome, "psychic") == 0) return TIPO_PSIQUICO;
    if (strcmp(nome, "bug") == 0) return TIPO_INSETO;
    if (strcmp(nome, "rock") == 0) return TIPO_PEDRA;
    if (strcmp(nome, "ghost") == 0) return TIPO_FANTASMA;
    if (strcmp(nome, "dragon") == 0) return TIPO_DRAGAO;
    return TIPO_NENHUM;
}

static int completarCatalogoLocal(HashPokedex *pokedex, ArvorePokedex *arvore) {
    FILE *arquivo = fopen("assets/pokemon/dados.txt", "r");
    char linha[256];
    if (arquivo == NULL) arquivo = fopen("../assets/pokemon/dados.txt", "r");
    if (arquivo == NULL) return 1; /* Mantém o cadastro embutido se os assets não estiverem no cwd. */
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        Pokemon pokemon;
        char nome[NOME_MAX], tipo1[16], tipo2[16];
        int numero, hp, ataque, defesa, especial_ataque, especial_defesa, velocidade;
        int lidos;
        if (linha[0] == '#') continue;
        lidos = sscanf(linha, "%d %39s %15s %15s %d %d %d %d %d %d", &numero, nome,
                       tipo1, tipo2, &hp, &ataque, &defesa, &especial_ataque,
                       &especial_defesa, &velocidade);
        if (lidos != 10 || buscarPorNumero(arvore, numero) != NULL) continue;
        pokemon = especie(numero, nome, tipoDaApi(tipo1), tipoDaApi(tipo2), hp,
                          especial_ataque > ataque ? especial_ataque : ataque,
                          especial_defesa > defesa ? especial_defesa : defesa, velocidade);
        adicionarGolpe(&pokemon, "Investida", TIPO_NORMAL, 40, 0);
        adicionarGolpe(&pokemon, "Golpe de tipo", pokemon.tipo1, 60, 0);
        if (pokemon.tipo2 != TIPO_NENHUM && pokemon.tipo2 != pokemon.tipo1)
            adicionarGolpe(&pokemon, "Golpe secundário", pokemon.tipo2, 60, 0);
        adicionarGolpe(&pokemon, "Ataque forte", pokemon.tipo1, 90, 0);
        if (!publicar(pokedex, arvore, &pokemon)) {
            fclose(arquivo);
            return 0;
        }
    }
    fclose(arquivo);
    return 1;
}

int registrarEspecies(HashPokedex *pokedex, ArvorePokedex *arvore) {
    Pokemon p;

    if (pokedex == NULL || arvore == NULL) {
        return 0;
    }

    p = especie(1, "Bulbassauro", TIPO_GRAMA, TIPO_VENENO, 45, 65, 49, 45);
    adicionarGolpe(&p, "Investida", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Chicote de Vinha", TIPO_GRAMA, 45, 0);
    adicionarGolpe(&p, "Folha Navalha", TIPO_GRAMA, 55, 0);
    adicionarGolpe(&p, "Bomba de Lodo", TIPO_VENENO, 90, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(3, "Venusauro", TIPO_GRAMA, TIPO_VENENO, 80, 100, 83, 80);
    adicionarGolpe(&p, "Folha Navalha", TIPO_GRAMA, 55, 0);
    adicionarGolpe(&p, "Bomba de Lodo", TIPO_VENENO, 90, 0);
    adicionarGolpe(&p, "Pancada Corporal", TIPO_NORMAL, 85, 0);
    adicionarGolpe(&p, "Raio Solar", TIPO_GRAMA, 120, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(4, "Charmander", TIPO_FOGO, TIPO_NENHUM, 39, 60, 43, 65);
    adicionarGolpe(&p, "Arranhão", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Brasa", TIPO_FOGO, 40, 0);
    adicionarGolpe(&p, "Talho", TIPO_NORMAL, 70, 0);
    adicionarGolpe(&p, "Lança-chamas", TIPO_FOGO, 90, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(6, "Charizard", TIPO_FOGO, TIPO_VOADOR, 78, 109, 78, 100);
    adicionarGolpe(&p, "Lança-chamas", TIPO_FOGO, 90, 0);
    adicionarGolpe(&p, "Ataque de Asa", TIPO_VOADOR, 60, 0);
    adicionarGolpe(&p, "Talho", TIPO_NORMAL, 70, 0);
    adicionarGolpe(&p, "Rajada de Fogo", TIPO_FOGO, 110, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(7, "Squirtle", TIPO_AGUA, TIPO_NENHUM, 44, 50, 65, 43);
    adicionarGolpe(&p, "Investida", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Revólver d'Água", TIPO_AGUA, 40, 0);
    adicionarGolpe(&p, "Mordida", TIPO_NORMAL, 60, 0);
    adicionarGolpe(&p, "Hidro Bomba", TIPO_AGUA, 110, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(9, "Blastoise", TIPO_AGUA, TIPO_NENHUM, 79, 85, 100, 78);
    adicionarGolpe(&p, "Revólver d'Água", TIPO_AGUA, 40, 0);
    adicionarGolpe(&p, "Mordida", TIPO_NORMAL, 60, 0);
    adicionarGolpe(&p, "Hidro Bomba", TIPO_AGUA, 110, 0);
    adicionarGolpe(&p, "Pancada Corporal", TIPO_NORMAL, 85, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(18, "Pidgeot", TIPO_NORMAL, TIPO_VOADOR, 83, 80, 75, 101);
    adicionarGolpe(&p, "Rajada", TIPO_VOADOR, 40, 0);
    adicionarGolpe(&p, "Ataque Rápido", TIPO_NORMAL, 40, 1);
    adicionarGolpe(&p, "Ataque de Asa", TIPO_VOADOR, 60, 0);
    adicionarGolpe(&p, "Hiper-raio", TIPO_NORMAL, 150, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(25, "Pikachu", TIPO_ELETRICO, TIPO_NENHUM, 35, 80, 40, 90);
    adicionarGolpe(&p, "Trovoada", TIPO_ELETRICO, 40, 0);
    adicionarGolpe(&p, "Ataque Rápido", TIPO_NORMAL, 40, 1);
    adicionarGolpe(&p, "Choque do Trovão", TIPO_ELETRICO, 90, 0);
    adicionarGolpe(&p, "Trovão", TIPO_ELETRICO, 110, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(52, "Meowth", TIPO_NORMAL, TIPO_NENHUM, 40, 45, 35, 90);
    adicionarGolpe(&p, "Arranhão", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Mordida", TIPO_NORMAL, 60, 0);
    adicionarGolpe(&p, "Talho", TIPO_NORMAL, 70, 0);
    adicionarGolpe(&p, "Dia de Pagamento", TIPO_NORMAL, 40, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(54, "Psyduck", TIPO_AGUA, TIPO_NENHUM, 50, 65, 48, 55);
    adicionarGolpe(&p, "Arranhão", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Revólver d'Água", TIPO_AGUA, 40, 0);
    adicionarGolpe(&p, "Confusão", TIPO_PSIQUICO, 50, 0);
    adicionarGolpe(&p, "Hidro Bomba", TIPO_AGUA, 110, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(65, "Alakazam", TIPO_PSIQUICO, TIPO_NENHUM, 55, 135, 45, 120);
    adicionarGolpe(&p, "Confusão", TIPO_PSIQUICO, 50, 0);
    adicionarGolpe(&p, "Feixe Psíquico", TIPO_PSIQUICO, 65, 0);
    adicionarGolpe(&p, "Psíquico", TIPO_PSIQUICO, 90, 0);
    adicionarGolpe(&p, "Bola Sombria", TIPO_FANTASMA, 80, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(68, "Machamp", TIPO_LUTADOR, TIPO_NENHUM, 90, 130, 80, 55);
    adicionarGolpe(&p, "Golpe de Caratê", TIPO_LUTADOR, 50, 0);
    adicionarGolpe(&p, "Rasteira", TIPO_LUTADOR, 70, 0);
    adicionarGolpe(&p, "Golpe Cruzado", TIPO_LUTADOR, 100, 0);
    adicionarGolpe(&p, "Força", TIPO_NORMAL, 80, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(74, "Geodude", TIPO_PEDRA, TIPO_TERRA, 40, 80, 100, 20);
    adicionarGolpe(&p, "Investida", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Lançamento de Rocha", TIPO_PEDRA, 50, 0);
    adicionarGolpe(&p, "Terremoto", TIPO_TERRA, 100, 0);
    adicionarGolpe(&p, "Megassoco", TIPO_NORMAL, 80, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(94, "Gengar", TIPO_FANTASMA, TIPO_VENENO, 60, 130, 60, 110);
    adicionarGolpe(&p, "Lambida", TIPO_FANTASMA, 30, 0);
    adicionarGolpe(&p, "Bola Sombria", TIPO_FANTASMA, 80, 0);
    adicionarGolpe(&p, "Bomba de Lodo", TIPO_VENENO, 90, 0);
    adicionarGolpe(&p, "Sombra Noturna", TIPO_FANTASMA, 80, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(95, "Onix", TIPO_PEDRA, TIPO_TERRA, 35, 45, 160, 70);
    adicionarGolpe(&p, "Investida", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Lançamento de Rocha", TIPO_PEDRA, 50, 0);
    adicionarGolpe(&p, "Terremoto", TIPO_TERRA, 100, 0);
    adicionarGolpe(&p, "Pancada Corporal", TIPO_NORMAL, 85, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(130, "Gyarados", TIPO_AGUA, TIPO_VOADOR, 95, 125, 79, 81);
    adicionarGolpe(&p, "Mordida", TIPO_NORMAL, 60, 0);
    adicionarGolpe(&p, "Hidro Bomba", TIPO_AGUA, 110, 0);
    adicionarGolpe(&p, "Ataque de Asa", TIPO_VOADOR, 60, 0);
    adicionarGolpe(&p, "Hiper-raio", TIPO_NORMAL, 150, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(143, "Snorlax", TIPO_NORMAL, TIPO_NENHUM, 160, 110, 65, 30);
    adicionarGolpe(&p, "Investida", TIPO_NORMAL, 40, 0);
    adicionarGolpe(&p, "Pancada Corporal", TIPO_NORMAL, 85, 0);
    adicionarGolpe(&p, "Terremoto", TIPO_TERRA, 100, 0);
    adicionarGolpe(&p, "Hiper-raio", TIPO_NORMAL, 150, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(144, "Articuno", TIPO_GELO, TIPO_VOADOR, 90, 85, 100, 85);
    adicionarGolpe(&p, "Raio de Gelo", TIPO_GELO, 90, 0);
    adicionarGolpe(&p, "Rajada", TIPO_VOADOR, 40, 0);
    adicionarGolpe(&p, "Nevasca", TIPO_GELO, 110, 0);
    adicionarGolpe(&p, "Ataque Rápido", TIPO_NORMAL, 40, 1);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(145, "Zapdos", TIPO_ELETRICO, TIPO_VOADOR, 90, 90, 85, 100);
    adicionarGolpe(&p, "Choque do Trovão", TIPO_ELETRICO, 90, 0);
    adicionarGolpe(&p, "Rajada", TIPO_VOADOR, 40, 0);
    adicionarGolpe(&p, "Trovão", TIPO_ELETRICO, 110, 0);
    adicionarGolpe(&p, "Ataque Rápido", TIPO_NORMAL, 40, 1);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(146, "Moltres", TIPO_FOGO, TIPO_VOADOR, 90, 100, 90, 90);
    adicionarGolpe(&p, "Lança-chamas", TIPO_FOGO, 90, 0);
    adicionarGolpe(&p, "Rajada", TIPO_VOADOR, 40, 0);
    adicionarGolpe(&p, "Rajada de Fogo", TIPO_FOGO, 110, 0);
    adicionarGolpe(&p, "Ataque Rápido", TIPO_NORMAL, 40, 1);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(149, "Dragonite", TIPO_DRAGAO, TIPO_VOADOR, 91, 134, 95, 80);
    adicionarGolpe(&p, "Ataque de Asa", TIPO_VOADOR, 60, 0);
    adicionarGolpe(&p, "Garra de Dragão", TIPO_DRAGAO, 80, 0);
    adicionarGolpe(&p, "Lança-chamas", TIPO_FOGO, 90, 0);
    adicionarGolpe(&p, "Hiper-raio", TIPO_NORMAL, 150, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    /*
     * Mewtwo é o chefe. O ataque não usa os 154 oficiais para a luta
     * caber em alguns turnos com um atributo só; ainda assim é o
     * maior ofensivo da Pokédex, com cobertura de gelo e fogo.
     */
    p = especie(150, "Mewtwo", TIPO_PSIQUICO, TIPO_NENHUM, 130, 120, 90, 130);
    adicionarGolpe(&p, "Psíquico", TIPO_PSIQUICO, 90, 0);
    adicionarGolpe(&p, "Bola Sombria", TIPO_FANTASMA, 80, 0);
    adicionarGolpe(&p, "Raio de Gelo", TIPO_GELO, 90, 0);
    adicionarGolpe(&p, "Lança-chamas", TIPO_FOGO, 90, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    p = especie(151, "Mew", TIPO_PSIQUICO, TIPO_NENHUM, 100, 100, 100, 100);
    adicionarGolpe(&p, "Psíquico", TIPO_PSIQUICO, 90, 0);
    adicionarGolpe(&p, "Bola Sombria", TIPO_FANTASMA, 80, 0);
    adicionarGolpe(&p, "Choque do Trovão", TIPO_ELETRICO, 90, 0);
    adicionarGolpe(&p, "Tesoura X", TIPO_INSETO, 80, 0);
    if (!publicar(pokedex, arvore, &p)) return 0;

    return completarCatalogoLocal(pokedex, arvore);
}

static int incluirTreinador(FilaTreinadores *fila, HashPokedex *pokedex,
                            const char *nome, const char *fala,
                            const char **especies, const int *niveis, int quantidade) {
    Treinador treinador;
    int i;

    criarTreinador(&treinador, nome, fala);

    for (i = 0; i < quantidade; i++) {
        const Pokemon *modelo = buscarPokemon(pokedex, especies[i]);
        Pokemon preparado;

        if (modelo == NULL) {
            liberarListaTime(&treinador.time);
            return 0;
        }
        preparado = pokemonPreparar(modelo, niveis[i]);
        if (!adicionarPokemon(&treinador.time, &preparado)) {
            liberarListaTime(&treinador.time);
            return 0;
        }
    }

    if (!enfileirarTreinador(fila, &treinador)) {
        liberarListaTime(&treinador.time);
        return 0;
    }
    return 1;
}

int montarArena(FilaTreinadores *fila, HashPokedex *pokedex) {
    static const char *brock[] = {"Geodude", "Onix"};
    static const int nv_brock[] = {44, 46};

    static const char *misty[] = {"Psyduck", "Gyarados"};
    static const int nv_misty[] = {45, 47};

    static const char *rocket[] = {"Meowth", "Pidgeot"};
    static const int nv_rocket[] = {46, 48};

    static const char *ash[] = {"Pikachu", "Bulbassauro", "Squirtle", "Charizard"};
    static const int nv_ash[] = {48, 47, 47, 49};

    static const char *mewtwo[] = {"Mewtwo"};
    static const int nv_mewtwo[] = {50};

    if (fila == NULL || pokedex == NULL) {
        return 0;
    }

    if (!incluirTreinador(fila, pokedex, "Brock",
                          "Uma rocha não recua. Mostre que a sua vontade é mais dura!",
                          brock, nv_brock, 2)) {
        return 0;
    }
    if (!incluirTreinador(fila, pokedex, "Misty",
                          "A água abre caminho. Vamos, meu time!",
                          misty, nv_misty, 2)) {
        return 0;
    }
    if (!incluirTreinador(fila, pokedex, "Equipe Rocket",
                          "Equipe Rocket decolando na velocidade da luz! Entrega esses Pokémon!",
                          rocket, nv_rocket, 2)) {
        return 0;
    }
    if (!incluirTreinador(fila, pokedex, "Ash",
                          "Eu escolho você! A amizade é o que faz um treinador de verdade.",
                          ash, nv_ash, 4)) {
        return 0;
    }
    if (!incluirTreinador(fila, pokedex, "Mewtwo",
                          "Humanos... Pokémon... se querem me enfrentar, lutem.",
                          mewtwo, nv_mewtwo, 1)) {
        return 0;
    }
    return 1;
}
