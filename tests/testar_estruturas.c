/*
 * testar_estruturas.c
 * -------------------
 * Testa as estruturas e as regras sem passar pelo menu.
 * Não substitui o --simular: aqui o foco é ponteiro, ordem e dano.
 */

#include "arvore_pokedex.h"
#include "batalha.h"
#include "caso.h"
#include "simulacao.h"
#include "fila_treinadores.h"
#include "hash_pokedex.h"
#include "lista_time.h"
#include "lista_turno.h"
#include "ordenacao.h"
#include "pilha_log.h"
#include "sala.h"
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

static void testarSalas(void) {
    ListaSalas salas;
    ListaTime time;
    PilhaLog log;
    NoSala gelo;
    NoSala porta;
    Pokemon fogo;
    char topo[LOG_MAX];

    memset(&gelo, 0, sizeof gelo);
    memset(&porta, 0, sizeof porta);
    copiarTexto(gelo.nome, sizeof gelo.nome, "Gelo");
    gelo.habilidade = HAB_FOGO;
    copiarTexto(gelo.texto_resolvido, sizeof gelo.texto_resolvido, "derreteu");
    copiarTexto(porta.nome, sizeof porta.nome, "Porta");
    porta.habilidade = HAB_NENHUMA;
    porta.resolvido = 1;

    criarListaSalas(&salas);
    criarListaTime(&time);
    criarPilhaLog(&log);
    checar(adicionarSala(&salas, &gelo), "sala do gelo entra");
    checar(adicionarSala(&salas, &porta), "porta entra bloqueada");
    checar(!avancarSala(&salas, &log), "não avança com puzzle fechado");

    fogo = especieSimples(4, "Charmander", 65);
    fogo.tipo1 = TIPO_FOGO;
    checar(adicionarPokemon(&time, &fogo), "charmander no time");
    checar(resolverPuzzleDaSala(&salas, &time, &log), "chama resolve o gelo");
    checar(salaAtual(&salas)->resolvido, "sala ficou resolvida");
    checar(avancarSala(&salas, &log), "avança depois do puzzle");
    checar(strcmp(salaAtual(&salas)->nome, "Porta") == 0, "cursor na porta");
    checar(voltarSala(&salas, &log), "volta para o gelo");
    checar(topoDoLog(&log, topo, sizeof topo), "histórico do caminho");
    checar(tamanhoSalas(&salas) == 2, "duas salas");

    liberarPilhaLog(&log);
    liberarListaTime(&time);
    liberarListaSalas(&salas);
}

/* Cada família tem operações e falhas próprias, verificadas sem relógio ou janela. */
static void testarDesafiosEspaciais(void) {
    Desafio d;
    int i;
    iniciarDesafio(&d, DES_CIRCUITO);
    checar(!d.concluido && circuitoAlcancado(&d) != 511U, "rede começa desconectada");
    checar(comandarDesafio(&d, ACAO_USAR, 9) == -1, "circuito recusa célula fora da grade");
    for (i = 0; i < 9 && !d.concluido; i++) {
        int giros = 0;
        while (d.rotacao[i] && giros++ < 4 && !d.concluido) comandarDesafio(&d, ACAO_USAR, i);
    }
    checar(d.concluido && circuitoAlcancado(&d) == 511U, "percurso real liga todos os terminais");
    iniciarDesafio(&d, DES_GELO);
    for (i = 0; i < 3; i++) comandarDesafio(&d, ACAO_USAR, 4);
    checar(d.gelo[4] == 0 && d.calor == 66, "chama remove camadas e acumula calor");
    comandarDesafio(&d, ACAO_USAR, 5);
    checar(comandarDesafio(&d, ACAO_USAR, 5) == 0 && d.gelo[4] > 0,
           "superaquecimento recompõe parte do gelo");
    comandarDesafio(&d, ACAO_RESFRIAR, 0);
    checar(d.calor == 40, "resfriar tem efeito na regra");
    checar(comandarDesafio(&d, ACAO_USAR, 1) == 0, "atingir tubo é um erro espacial");
    iniciarDesafio(&d, DES_AGUA);
    checar(comandarDesafio(&d, ACAO_USAR, 3) == 0 && !d.concluido, "mangueira sem dosagem não libera porta");
    {
        static const int passos[] = {0,1,2,1,0,1};
        for (i = 0; i < 6; i++) comandarDesafio(&d, ACAO_USAR, passos[i]);
    }
    checar(d.tanque_a == 4 && d.tanque_b == 3 && !d.concluido, "conserva volumes ao transferir água");
    checar(comandarDesafio(&d, ACAO_USAR, 3) == 2, "mangueira usa a dosagem correta");
    iniciarDesafio(&d, DES_RASTRO);
    checar(distanciaRastro(&d, 2) == 0 && distanciaRastro(&d, 0) == 2, "scanner mede distância no espaço");
    checar(comandarDesafio(&d, ACAO_USAR, 0) == 0 && d.rastros == 0, "examinar área errada não encontra pista");
    comandarDesafio(&d, ACAO_USAR, 2);
    checar(comandarDesafio(&d, ACAO_USAR, 2) == 0 && d.rastros == 1, "mesma pista não conta duas vezes");
    comandarDesafio(&d, ACAO_USAR, 14);
    checar(comandarDesafio(&d, ACAO_USAR, 17) == 2, "três rastros revelam documento");
    iniciarDesafio(&d, DES_ESTANTES);
    checar(comandarDesafio(&d, ACAO_DIREITA, 0) == 0 && d.operador == 13, "estante não atravessa parede");
    comandarDesafio(&d, ACAO_BAIXO, 0);
    checar(d.operador == 19, "operador anda sem empurrar uma estante distante");
    checar(comandarDesafio(&d, ACAO_DESFAZER, 0) == 1 && d.operador == 13,
           "desfazer restaura operador e caixas");
    checar(comandarDesafio(&d, ACAO_DESFAZER, 0) == 0, "não reaplica desfazer sem estado anterior");
    comandarDesafio(&d, ACAO_REINICIAR, 0);
    checar(d.movimentos == 0 && d.caixas == ((1UL << 14) | (1UL << 26)), "reinício recupera configuração resolúvel");
}

static int vencerEncontroTeste(Caso *caso) {
    int turnos = 0;
    while (caso->em_batalha && turnos++ < 60) {
        int golpe = escolherGolpeAdversario(obterAtivo(&caso->nucleo.time), obterAtivo(&caso->oponente_atual.time));
        if (casoExecutarGolpe(caso, golpe, NULL) < 0) return 0;
    }
    return !caso->em_batalha && !caso->oponente_carregado;
}

static void testarProgressaoECaptura(void) {
    Caso caso;
    int setor = 0, guardas = 0, selvagens = 0;
    if (!iniciarCaso(&caso)) { checar(0, "inicia progressão"); return; }
    if (!comecarCaso(&caso)) { checar(0, "monta progressão"); encerrarCaso(&caso); return; }
    checar(caso.nucleo.fila.tamanho == 6, "fila inclui quatro confrontos e dois guardas");
    checar(obterAtivo(&caso.nucleo.time)->qtd_golpes == 2, "Pikachu começa com arsenal limitado");
    checar(!casoAcenderPonto(&caso, 0), "refletor exige gerador");
    checar(!casoUsarHabilidade(&caso), "habilidade não ignora circuito");
    checar(casoComandarPuzzle(&caso, ACAO_USAR, -1) == -1, "recusa alvo inválido");
    checar(casoPaginaDocumento(&caso, 0) == NULL, "documento ainda está escondido");
    while (!caso.caso_encerrado && setor < 8) {
        NoSala *sala = caso.salas.atual;
        if (sala->inicia_batalha && !sala->batalha_concluida) {
            checar(casoDescansar(&caso), "recupera time antes de confronto");
            checar(casoIniciarBatalha(&caso), "fila inicia encontro do setor");
            checar(casoComandarPuzzle(&caso, ACAO_USAR, 0) == -1, "não resolve puzzle em batalha");
            checar(vencerEncontroTeste(&caso), "vence confronto obrigatório");
            if (caso.captura_disponivel) {
                selvagens++;
                checar(!casoCapturar(&caso), "animação não captura sozinha");
                checar(!casoAvancar(&caso), "captura pendente bloqueia avanço");
                checar(casoResponderCaptura(&caso, 0, 50) == 0 && caso.captura_acertos == 0,
                       "comando errado reinicia contenção sem perder selvagem");
                checar(casoResponderCaptura(&caso, 4, 0) == 1, "Z lança a bola");
                checar(casoResponderCaptura(&caso, casoComandoCaptura(&caso), casoPrazoCaptura(&caso) + 1) == 0,
                       "captura rejeita reação fora do prazo");
                checar(simularCaptura(&caso), "cinco comandos corretos completam captura");
            }
        }
        if (caso.caso_encerrado) break;
        if (sala->duelo_obrigatorio) checar(!casoIniciarDuelo(&caso), "guarda exige puzzle e refletores");
        checar(simularPuzzleAtual(&caso), "cada desafio possui solução pelas ações públicas");
        if (sala->luzes_necessarias) {
            checar(!casoAvancar(&caso), "puzzle sozinho não libera setor escuro");
            checar(casoAcenderPonto(&caso, 0), "religa primeiro refletor");
            checar(!casoAvancar(&caso), "um refletor ainda não completa setor");
            checar(casoAcenderPonto(&caso, 1), "religa segundo refletor");
        }
        if (sala->duelo_obrigatorio) {
            int bolas = caso.pokebolas;
            checar(!casoAvancar(&caso), "guarda bloqueia saída depois do puzzle");
            casoDescansar(&caso);
            checar(casoIniciarDuelo(&caso), "guarda sai na ordem certa da fila");
            checar(casoTentarFugir(&caso, NULL) == -2, "não pula guarda fugindo");
            checar(casoUsarItem(&caso, ITEM_POKE_BOLA, NULL) == -2 && caso.pokebolas == bolas,
                   "não captura Pokémon de NPC nem gasta bola");
            checar(vencerEncontroTeste(&caso) && sala->duelo_concluido, "vitória libera guarda sem alterar puzzle");
            guardas++;
            checar(pokemonNaPosicao(&caso.nucleo.time, 0)->qtd_golpes == 2 + guardas, "guarda desbloqueia novo golpe");
        }
        if (strcmp(sala->nome, "Pátio") == 0) {
            NoFila *frente = caso.nucleo.fila.frente;
            int nivel = pokemonNaPosicao(&caso.nucleo.time, 0)->nivel;
            int i;
            for (i = 0; i < 2; i++) {
                casoDescansar(&caso);
                checar(casoIniciarTreino(&caso) && vencerEncontroTeste(&caso), "treino opcional continua funcionando");
                if (caso.captura_disponivel) checar(simularCaptura(&caso), "captura de treino também é interativa");
            }
            checar(pokemonNaPosicao(&caso.nucleo.time, 0)->nivel == nivel + 1, "treinar gera progressão real de nível");
            checar(caso.nucleo.fila.frente == frente, "treino não consome NPC da fila");
        }
        if (strcmp(sala->nome, "Jaula") == 0) {
            checar(casoPaginaDocumento(&caso, 0) && casoPaginaDocumento(&caso, 1) &&
                   !casoPaginaDocumento(&caso, 2), "documento libera duas páginas completas");
        }
        if (!casoAvancar(&caso)) { checar(0, "avança ao concluir objetivos do setor"); break; }
        setor++;
    }
    checar(caso.caso_encerrado && guardas == 2 && selvagens == 3 && caso.nucleo.fila.tamanho == 0,
           "progressão completa passa por três capturas, dois guardas e Giovanni");
    checar(comecarCaso(&caso), "reinicia caso após vitória");
    checar(!caso.salas.inicio->luzes && !caso.salas.inicio->desafio.concluido &&
           !caso.captura_acertos && !caso.batalha_npc && caso.nucleo.fila.tamanho == 6,
           "reinício limpa desafios, luzes, guardas e captura");
    encerrarCaso(&caso);
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
    testarSalas();
    testarDesafiosEspaciais();
    testarProgressaoECaptura();

    if (falhas == 0) {
        printf("Estruturas: todos os testes passaram.\n");
        return 0;
    }
    fprintf(stderr, "Estruturas: %d teste(s) falharam.\n", falhas);
    return 1;
}
