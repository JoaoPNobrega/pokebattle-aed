/*
 * testar_estruturas.c
 * -------------------
 * Testa as estruturas e as regras sem passar pelo menu.
 * Não substitui o --simular: aqui o foco é ponteiro, ordem e dano.
 */

#include "arvore_pokedex.h"
#include "batalha.h"
#include "fila_treinadores.h"
#include "hash_pokedex.h"
#include "lista_time.h"
#include "lista_turno.h"
#include "ordenacao.h"
#include "pilha_log.h"
#include "pokemon.h"
#include "tipos.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

static int falhas = 0;

static void checar(int condicao, const char *texto) {
    if (!condicao) {
        fprintf(stderr, "FALHOU: %s\n", texto);
        falhas++;
    }
}

static Pokemon especieSimples(int numero, const char *nome, int velocidade) {
    Pokemon pokemon;
    memset(&pokemon, 0, sizeof pokemon);
    pokemon.numero = numero;
    copiarTexto(pokemon.nome, sizeof pokemon.nome, nome);
    pokemon.tipo1 = TIPO_NORMAL;
    pokemon.tipo2 = TIPO_NENHUM;
    pokemon.hp_base = 50;
    pokemon.ataque = 50;
    pokemon.defesa = 50;
    pokemon.velocidade = velocidade;
    pokemon.nivel = 50;
    pokemon.hp_max = 50;
    pokemon.hp_atual = 50;
    return pokemon;
}

static void testarLista(void) {
    ListaTime time;
    Pokemon a = especieSimples(1, "A", 10);
    Pokemon b = especieSimples(2, "B", 30);
    Pokemon c = especieSimples(3, "C", 20);
    NoTime *no;

    criarListaTime(&time);
    checar(adicionarPokemon(&time, &a), "adicionar A");
    checar(adicionarPokemon(&time, &b), "adicionar B");
    checar(adicionarPokemon(&time, &c), "adicionar C");
    checar(tamanhoTime(&time) == 3, "tamanho 3");
    checar(time.inicio->prox->ant == time.inicio, "elo ant do meio");
    checar(strcmp(time.fim->pokemon.nome, "C") == 0, "fim é C");

    checar(removerPokemon(&time, "b"), "remover o do meio");
    checar(time.inicio->prox == time.fim, "cabeça liga na cauda");
    checar(time.fim->ant == time.inicio, "cauda liga na cabeça");
    checar(tamanhoTime(&time) == 2, "tamanho 2 depois de remover");

    checar(adicionarPokemon(&time, &b), "repor B");
    checar(trocarPokemon(&time, 0, 2), "trocar pontas");
    checar(strcmp(pokemonNaPosicao(&time, 0)->nome, "B") == 0, "líder virou B");
    checar(strcmp(pokemonNaPosicao(&time, 2)->nome, "A") == 0, "último virou A");

    definirAtivoPorPosicao(&time, 0);
    checar(avancarAtivo(&time), "avancar");
    checar(posicaoDoAtivo(&time) == 1, "cursor no meio");
    checar(retrocederAtivo(&time), "retroceder");
    checar(posicaoDoAtivo(&time) == 0, "cursor voltou");

    /* Do fim, avançar circula para o início. */
    definirAtivoPorPosicao(&time, 2);
    avancarAtivo(&time);
    checar(posicaoDoAtivo(&time) == 0, "avanco circular");

    ordenarTime(&time);
    checar(pokemonNaPosicao(&time, 0)->velocidade == 30, "mais rápido na frente");
    checar(pokemonNaPosicao(&time, 1)->velocidade == 20, "velocidade do meio");
    checar(pokemonNaPosicao(&time, 2)->velocidade == 10, "mais lento no fim");

    no = time.inicio;
    while (no->prox != NULL) {
        checar(no->prox->ant == no, "todos os elos ant/prox");
        no = no->prox;
    }

    liberarListaTime(&time);
    checar(tamanhoTime(&time) == 0, "lista liberada");
}

static void testarFila(void) {
    FilaTreinadores fila;
    Treinador a;
    Treinador b;
    Treinador saiu;

    criarFilaTreinadores(&fila);
    criarTreinador(&a, "Brock", "pedra");
    criarTreinador(&b, "Misty", "agua");
    checar(enfileirarTreinador(&fila, &a), "enfileirar Brock");
    checar(enfileirarTreinador(&fila, &b), "enfileirar Misty");
    checar(strcmp(frenteDaFila(&fila)->nome, "Brock") == 0, "frente é o primeiro");
    checar(proximoTreinador(&fila, &saiu), "sair da fila");
    checar(strcmp(saiu.nome, "Brock") == 0, "FIFO entrega Brock");
    liberarListaTime(&saiu.time);
    checar(strcmp(frenteDaFila(&fila)->nome, "Misty") == 0, "Misty ficou na frente");
    liberarFilaTreinadores(&fila);
    checar(filaVazia(&fila), "fila vazia");
}

static void testarPilha(void) {
    PilhaLog pilha;
    char topo[LOG_MAX];
    char bloco[LOG_MAX * 4];
    FILE *arquivo;
    size_t lidos;

    criarPilhaLog(&pilha);
    checar(empilharLog(&pilha, "um"), "empilha um");
    checar(empilharLog(&pilha, "dois"), "empilha dois");
    checar(empilharLog(&pilha, "tres"), "empilha tres");
    checar(topoDoLog(&pilha, topo, sizeof topo) && strcmp(topo, "tres") == 0, "topo é o mais novo");

    arquivo = tmpfile();
    checar(arquivo != NULL, "tmpfile");
    if (arquivo != NULL) {
        mostrarHistorico(&pilha, arquivo);
        rewind(arquivo);
        lidos = fread(bloco, 1, sizeof bloco - 1, arquivo);
        bloco[lidos] = '\0';
        fclose(arquivo);
        checar(strcmp(bloco, "um\ndois\ntres\n") == 0, "histórico cronológico");
    }
    checar(topoDoLog(&pilha, topo, sizeof topo) && strcmp(topo, "tres") == 0, "pilha restaurada");
    checar(desempilharLog(&pilha, topo, sizeof topo) && strcmp(topo, "tres") == 0, "desempilha o topo");
    liberarPilhaLog(&pilha);
}

static void testarHash(void) {
    HashPokedex pokedex;
    Pokemon a = especieSimples(25, "Pikachu", 90);
    Pokemon b = especieSimples(6, "Charizard", 100);
    const Pokemon *achado;

    criarHashPokedex(&pokedex);
    checar(inserirNaPokedex(&pokedex, &a), "insere Pikachu");
    checar(inserirNaPokedex(&pokedex, &b), "insere Charizard");
    checar(!inserirNaPokedex(&pokedex, &a), "recusa duplicata");
    achado = buscarPokemon(&pokedex, "pikachu");
    checar(achado != NULL && achado->numero == 25, "busca ignora maiúsculas");
    checar(buscarPokemon(&pokedex, "Mewtwo") == NULL, "ausente devolve NULL");
    checar(removerDaPokedex(&pokedex, "Charizard"), "remove Charizard");
    checar(buscarPokemon(&pokedex, "Charizard") == NULL, "removido some");
    checar(quantidadePokedex(&pokedex) == 1, "sobrou um");
    liberarHashPokedex(&pokedex);
}

typedef struct {
    int ultimo;
    int quantidade;
    int ordenado;
} OrdemVisitada;

static void visitarOrdem(const Pokemon *pokemon, void *contexto) {
    OrdemVisitada *ordem = (OrdemVisitada *)contexto;
    if (ordem->quantidade > 0 && pokemon->numero < ordem->ultimo) {
        ordem->ordenado = 0;
    }
    ordem->ultimo = pokemon->numero;
    ordem->quantidade++;
}

static void testarArvore(void) {
    ArvorePokedex arvore;
    OrdemVisitada ordem;
    int i;

    criarArvorePokedex(&arvore);
    for (i = 1; i <= 3; i++) {
        Pokemon p = especieSimples(i, "N", i);
        checar(inserirNaArvore(&arvore, &p), "insere sequencial");
    }
    /* 1, 2, 3 numa BST pura teria altura 3. A AVL gira e fica com 2. */
    checar(alturaDaArvore(&arvore) == 2, "AVL rebalanceou 1-2-3");

    for (i = 4; i <= 15; i++) {
        Pokemon p = especieSimples(i, "N", i);
        inserirNaArvore(&arvore, &p);
    }
    checar(quantidadeDaArvore(&arvore) == 15, "15 nós");
    checar(alturaDaArvore(&arvore) <= 6, "altura continua logarítmica");

    ordem.ultimo = -1;
    ordem.quantidade = 0;
    ordem.ordenado = 1;
    percorrerEmOrdem(&arvore, visitarOrdem, &ordem);
    checar(ordem.ordenado && ordem.quantidade == 15, "em ordem crescente");
    checar(buscarPorNumero(&arvore, 15) != NULL, "acha o 15");
    checar(removerDaArvore(&arvore, 8), "remove o 8");
    checar(buscarPorNumero(&arvore, 8) == NULL, "8 saiu");
    checar(buscarPorNumero(&arvore, 7) != NULL && buscarPorNumero(&arvore, 9) != NULL, "vizinhos ficaram");
    checar(quantidadeDaArvore(&arvore) == 14, "quantidade após remoção");
    checar(alturaDaArvore(&arvore) <= 6, "altura após remoção");
    liberarArvorePokedex(&arvore);
    checar(alturaDaArvore(&arvore) == 0, "árvore liberada");
}

static void testarOrdenacaoETurno(void) {
    AtorTurno atores[4];
    Pokemon lentos[2];
    Pokemon pokedex[4];
    ListaTurno ciclo;
    int i;

    memset(atores, 0, sizeof atores);
    lentos[0] = especieSimples(1, "Lento", 10);
    lentos[1] = especieSimples(2, "Rapido", 200);
    lentos[0].golpes[0].prioridade = 1;
    lentos[0].golpes[0].poder = 40;
    lentos[0].qtd_golpes = 1;
    copiarTexto(lentos[0].golpes[0].nome, sizeof lentos[0].golpes[0].nome, "Ataque Rápido");
    lentos[1].golpes[0].prioridade = 0;
    lentos[1].golpes[0].poder = 80;
    lentos[1].qtd_golpes = 1;

    atores[0].pokemon = &lentos[0];
    atores[0].velocidade = 10;
    atores[0].prioridade = 1;
    atores[0].lado = LADO_JOGADOR;
    atores[1].pokemon = &lentos[1];
    atores[1].velocidade = 200;
    atores[1].prioridade = 0;
    atores[1].lado = LADO_OPONENTE;
    atores[2].pokemon = &lentos[1];
    atores[2].velocidade = 50;
    atores[2].prioridade = 0;
    atores[3].pokemon = &lentos[0];
    atores[3].velocidade = 50;
    atores[3].prioridade = 0;
    /* O terceiro e o quarto empatam em velocidade. A estabilidade mantém a ordem. */
    atores[2].lado = 2;
    atores[3].lado = 3;

    ordenarTurnoPorInsercao(atores, 4);
    checar(atores[0].prioridade == 1, "prioridade fura a velocidade");
    checar(atores[1].velocidade == 200, "mais rápido em seguida");
    checar(atores[2].lado == 2 && atores[3].lado == 3, "empate estável");

    criarListaTurno(&ciclo);
    definirOrdemTurno(&ciclo, &lentos[0], 0, &lentos[1], 0);
    checar(tamanhoListaTurno(&ciclo) == 2, "ciclo com dois");
    checar(inicioDoCiclo(&ciclo)->ator.lado == LADO_JOGADOR, "Ataque Rápido age primeiro");
    checar(proximoDoCiclo(ciclo.fim) == ciclo.inicio, "fim aponta para o início");
    checar(proximoDoCiclo(proximoDoCiclo(ciclo.inicio)) == ciclo.inicio, "volta ao início");
    liberarListaTurno(&ciclo);

    pokedex[0] = especieSimples(25, "Pikachu", 90);
    pokedex[1] = especieSimples(1, "Bulbassauro", 45);
    pokedex[2] = especieSimples(6, "Charizard", 100);
    pokedex[3] = especieSimples(6, "Charizard", 10);
    pokedex[3].nivel = 10;
    checar(ordenarPokedex(pokedex, 4, ORDENAR_POR_NOME), "merge sort");
    checar(strcmp(pokedex[0].nome, "Bulbassauro") == 0, "nome A primeiro");
    checar(strcmp(pokedex[1].nome, "Charizard") == 0, "Charizard no meio");
    /* Estável: os dois Charizard mantêm a ordem original (vel 100 antes de vel 10). */
    checar(pokedex[1].velocidade == 100 && pokedex[2].velocidade == 10, "merge estável");
    checar(strcmp(pokedex[3].nome, "Pikachu") == 0, "Pikachu no fim");

    for (i = 0; i < 4; i++) {
        pokedex[i].nivel = 50 - i;
    }
    ordenarPokemonPorInsercao(pokedex, 4, ORDENAR_POR_NIVEL);
    checar(pokedex[0].nivel >= pokedex[1].nivel && pokedex[1].nivel >= pokedex[2].nivel, "nível decrescente");
}

static void testarDano(void) {
    Pokemon atacante = especieSimples(1, "Fogo", 50);
    Pokemon grama = especieSimples(2, "Grama", 50);
    Pokemon agua = especieSimples(3, "Agua", 50);
    Pokemon terra = especieSimples(4, "Terra", 50);
    Golpe brasa;
    Golpe choque;
    int dano_grama;
    int dano_agua;
    int dano_imune;

    atacante.tipo1 = TIPO_FOGO;
    atacante.nivel = 50;
    atacante.ataque = 80;
    grama.tipo1 = TIPO_GRAMA;
    agua.tipo1 = TIPO_AGUA;
    terra.tipo1 = TIPO_TERRA;

    memset(&brasa, 0, sizeof brasa);
    brasa.tipo = TIPO_FOGO;
    brasa.poder = 40;
    memset(&choque, 0, sizeof choque);
    choque.tipo = TIPO_ELETRICO;
    choque.poder = 90;

    dano_grama = calcularDano(&atacante, &brasa, &grama);
    dano_agua = calcularDano(&atacante, &brasa, &agua);
    dano_imune = calcularDano(&atacante, &choque, &terra);

    checar(dano_grama > dano_agua, "fogo dói mais em grama do que em água");
    checar(dano_agua > 0, "pouco efetivo ainda causa dano");
    checar(dano_imune == 0, "elétrico não afeta terra");
    checar(multiplicadorEfetividade(TIPO_AGUA, TIPO_PEDRA, TIPO_TERRA) == 400, "água 4x em pedra/terra");
    checar(verificarDesmaio(NULL), "NULL conta como desmaiado");
}

int main(void) {
    inicializarTipos();
    testarLista();
    testarFila();
    testarPilha();
    testarHash();
    testarArvore();
    testarOrdenacaoETurno();
    testarDano();

    if (falhas == 0) {
        printf("Estruturas: todos os testes passaram.\n");
        return 0;
    }
    fprintf(stderr, "Estruturas: %d teste(s) falharam.\n", falhas);
    return 1;
}
