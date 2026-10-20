/*
 * ui_terminal.c
 * -------------
 * Menu, montagem do time e a tela da batalha. Toda leitura de teclado
 * passa por lerLinha, para o fim de arquivo (Ctrl+D ou o fim de um
 * script) encerrar o programa sem entrar em laço.
 */

#include "ui_terminal.h"

#include "batalha.h"
#include "ordenacao.h"
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINHA_MAX 128

static int lerLinha(char *buffer, size_t capacidade) {
    if (buffer == NULL || capacidade == 0) {
        return 0;
    }
    if (fgets(buffer, (int)capacidade, stdin) == NULL) {
        return 0;
    }

    {
        size_t n = strlen(buffer);
        if (n > 0 && buffer[n - 1] == '\n') {
            buffer[n - 1] = '\0';
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
        }
    }

    apararEspacos(buffer);
    return 1;
}

static void prompt(void) {
    printf("> ");
    fflush(stdout);
}

static int interpretarInteiro(const char *texto, int *valor) {
    char *fim;
    long lido;

    if (texto == NULL || valor == NULL) {
        return 0;
    }
    while (*texto == ' ') {
        texto++;
    }
    if (*texto == '\0') {
        return 0;
    }

    lido = strtol(texto, &fim, 10);
    if (fim == texto) {
        return 0;
    }
    while (*fim == ' ') {
        fim++;
    }
    if (*fim != '\0') {
        return 0;
    }
    *valor = (int)lido;
    return 1;
}

static void separarComando(const char *linha, char *comando, size_t cap_comando,
                           char *resto, size_t cap_resto) {
    size_t i = 0;

    if (comando != NULL && cap_comando > 0) {
        comando[0] = '\0';
    }
    if (resto != NULL && cap_resto > 0) {
        resto[0] = '\0';
    }
    if (linha == NULL) {
        return;
    }

    while (*linha != '\0' && isspace((unsigned char)*linha)) {
        linha++;
    }
    while (*linha != '\0' && !isspace((unsigned char)*linha) && i + 1 < cap_comando) {
        comando[i] = (char)toupper((unsigned char)*linha);
        i++;
        linha++;
    }
    if (cap_comando > 0) {
        comando[i] = '\0';
    }
    while (*linha != '\0' && isspace((unsigned char)*linha)) {
        linha++;
    }
    copiarTexto(resto, cap_resto, linha);
}

static void imprimirBarra(int atual, int maximo) {
    int i;
    int cheios;

    if (maximo < 1) {
        maximo = 1;
    }
    if (atual < 0) {
        atual = 0;
    }
    if (atual > maximo) {
        atual = maximo;
    }

    cheios = (atual * 20) / maximo;
    printf("[");
    for (i = 0; i < 20; i++) {
        putchar(i < cheios ? '#' : '-');
    }
    printf("] %d/%d", atual, maximo);
}

static void imprimirTipos(const Pokemon *pokemon) {
    printf("%s", nomeTipo(pokemon->tipo1));
    if (pokemon->tipo2 != TIPO_NENHUM) {
        printf("/%s", nomeTipo(pokemon->tipo2));
    }
}

static void imprimirFicha(const Pokemon *pokemon) {
    int i;
    int hp50;

    if (pokemon == NULL) {
        printf("Não há ficha para mostrar.\n");
        return;
    }

    hp50 = calcularHpMax(pokemon->hp_base, NIVEL_DO_JOGADOR);
    printf("\n#%03d %s\n", pokemon->numero, pokemon->nome);
    printf("Tipos: ");
    imprimirTipos(pokemon);
    printf("\n");
    printf("Base  HP %d  Ataque %d  Defesa %d  Velocidade %d\n",
           pokemon->hp_base, pokemon->ataque, pokemon->defesa, pokemon->velocidade);
    printf("No nível %d: HP %d, ataque %d, defesa %d, velocidade %d\n",
           NIVEL_DO_JOGADOR, hp50,
           calcularAtributo(pokemon->ataque, NIVEL_DO_JOGADOR),
           calcularAtributo(pokemon->defesa, NIVEL_DO_JOGADOR),
           calcularAtributo(pokemon->velocidade, NIVEL_DO_JOGADOR));
    printf("Golpes:\n");
    for (i = 0; i < pokemon->qtd_golpes; i++) {
        const Golpe *golpe = &pokemon->golpes[i];
        printf("  - %s [%s, poder %d", golpe->nome, nomeTipo(golpe->tipo), golpe->poder);
        if (golpe->prioridade > 0) {
            printf(", prioridade %d", golpe->prioridade);
        }
        printf("]\n");
    }
    printf("\n");
}

typedef struct {
    int contagem;
} ContextoListagem;

static void imprimirResumoEspecie(const Pokemon *pokemon, void *contexto) {
    ContextoListagem *ctx = (ContextoListagem *)contexto;

    if (pokemon == NULL) {
        return;
    }
    if (ctx != NULL) {
        ctx->contagem++;
    }
    printf("%3d  %-14s ", pokemon->numero, pokemon->nome);
    imprimirTipos(pokemon);
    printf("  HP %3d  Atk %3d  Def %3d  Vel %3d\n",
           pokemon->hp_base, pokemon->ataque, pokemon->defesa, pokemon->velocidade);
}

static void mostrarTime(const ListaTime *time) {
    int i;
    int n = tamanhoTime(time);
    int cursor = posicaoDoAtivo(time);

    printf("\nTime (%d/%d). O cursor marca o nó atual da lista; a posição 1 entra primeiro.\n",
           n, MAX_TIME);
    if (n == 0) {
        printf("  (vazio)\n\n");
        return;
    }

    for (i = 0; i < n; i++) {
        const Pokemon *pokemon = pokemonNaPosicao(time, i);
        printf("  %d) %s %s  ", i + 1, (i == cursor) ? ">" : " ", pokemon->nome);
        imprimirTipos(pokemon);
        printf("  vel %d\n", pokemon->velocidade);
    }
    printf("\n");
}

static void mostrarAjudaTime(void) {
    printf("Comandos do time:\n");
    printf("  <número ou nome>   adiciona (nível %d, até %d)\n", NIVEL_DO_JOGADOR, MAX_TIME);
    printf("  LISTAR             Pokédex em ordem de número (percurso da AVL)\n");
    printf("  TIME               mostra a lista e o cursor\n");
    printf("  REMOVER <nome>     tira um Pokémon (religa a lista dupla)\n");
    printf("  TROCAR <i> <j>     troca duas posições (1 é o líder)\n");
    printf("  PROX / ANT         anda com o cursor (avancarAtivo / retrocederAtivo)\n");
    printf("  ORDENAR            insertion sort por velocidade; o mais rápido vira o líder\n");
    printf("  PRONTO             começa a arena\n");
    printf("  CANCELAR           volta ao menu sem lutar\n");
    printf("  AJUDA              mostra esta lista\n");
}

static int adicionarAoTime(Jogo *jogo, const Pokemon *modelo) {
    Pokemon preparado;

    if (modelo == NULL) {
        printf("Não encontrei esse Pokémon na Pokédex.\n");
        return 1;
    }
    if (buscarPokemonNoTime(&jogo->time, modelo->nome) != NULL) {
        printf("%s já está no time.\n", modelo->nome);
        return 1;
    }
    if (tamanhoTime(&jogo->time) >= MAX_TIME) {
        printf("O time já tem %d Pokémon. Remova algum antes.\n", MAX_TIME);
        return 1;
    }

    preparado = pokemonPreparar(modelo, NIVEL_DO_JOGADOR);
    if (!adicionarPokemon(&jogo->time, &preparado)) {
        printf("Não foi possível adicionar (memória ou time cheio).\n");
        return 1;
    }
    printf("%s entrou no time (%d/%d).\n", modelo->nome, tamanhoTime(&jogo->time), MAX_TIME);
    return 1;
}

/*
 * Retorno: 1 pronto para lutar, 0 cancelou, -1 fim da entrada.
 */
static int montarTime(Jogo *jogo) {
    printf("\n--- Montar o time (até %d Pokémon, nível %d) ---\n", MAX_TIME, NIVEL_DO_JOGADOR);
    mostrarAjudaTime();

    for (;;) {
        char linha[LINHA_MAX];
        char comando[LINHA_MAX];
        char resto[LINHA_MAX];

        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            return -1;
        }
        if (linha[0] == '\0') {
            continue;
        }

        separarComando(linha, comando, sizeof comando, resto, sizeof resto);

        if (strcmp(comando, "AJUDA") == 0) {
            mostrarAjudaTime();
        } else if (strcmp(comando, "LISTAR") == 0) {
            ContextoListagem ctx;
            ctx.contagem = 0;
            printf("\nPokédex (em ordem, pela árvore):\n");
            percorrerEmOrdem(&jogo->arvore, imprimirResumoEspecie, &ctx);
            printf("\n");
        } else if (strcmp(comando, "TIME") == 0) {
            mostrarTime(&jogo->time);
        } else if (strcmp(comando, "PROX") == 0) {
            if (!avancarAtivo(&jogo->time)) {
                printf("O time está vazio.\n");
            } else {
                mostrarTime(&jogo->time);
            }
        } else if (strcmp(comando, "ANT") == 0) {
            if (!retrocederAtivo(&jogo->time)) {
                printf("O time está vazio.\n");
            } else {
                mostrarTime(&jogo->time);
            }
        } else if (strcmp(comando, "ORDENAR") == 0) {
            ordenarTime(&jogo->time);
            printf("Time ordenado por velocidade (insertion sort).\n");
            mostrarTime(&jogo->time);
        } else if (strcmp(comando, "REMOVER") == 0) {
            if (resto[0] == '\0') {
                printf("Use: REMOVER <nome>\n");
            } else if (removerPokemon(&jogo->time, resto)) {
                printf("%s saiu do time.\n", resto);
                mostrarTime(&jogo->time);
            } else {
                printf("Não há \"%s\" no time.\n", resto);
            }
        } else if (strcmp(comando, "TROCAR") == 0) {
            int i = 0;
            int j = 0;
            if (sscanf(resto, "%d %d", &i, &j) != 2) {
                printf("Use: TROCAR <posição> <posição>  (exemplo: TROCAR 1 3)\n");
            } else if (trocarPokemon(&jogo->time, i - 1, j - 1)) {
                printf("Posições %d e %d trocadas.\n", i, j);
                mostrarTime(&jogo->time);
            } else {
                printf("Posição inválida. O time tem %d Pokémon.\n", tamanhoTime(&jogo->time));
            }
        } else if (strcmp(comando, "PRONTO") == 0) {
            if (tamanhoTime(&jogo->time) < 1) {
                printf("Escolha pelo menos um Pokémon.\n");
            } else {
                printf("Time fechado. A posição 1 entra primeiro em cada batalha.\n");
                return 1;
            }
        } else if (strcmp(comando, "CANCELAR") == 0) {
            printf("Jornada cancelada.\n");
            return 0;
        } else if (textoEhNumero(comando)) {
            int numero = atoi(comando);
            adicionarAoTime(jogo, buscarPorNumero(&jogo->arvore, numero));
        } else {
            /* O comando foi posto em maiúsculas; a busca ignora a caixa. */
            adicionarAoTime(jogo, buscarPokemon(&jogo->pokedex, comando));
        }
    }
}

static void narrarTerminal(void *contexto, const char *mensagem) {
    (void)contexto;
    printf("%s\n", mensagem);
    fflush(stdout);
}

static void mostrarStatusBatalha(const VisaoBatalha *visao) {
    const Pokemon *eu = obterAtivo(visao->time_jogador);
    const Pokemon *opp = obterAtivo(visao->time_oponente);
    int i;

    printf("\n----- Turno %d vs %s -----\n", visao->numero_turno, visao->nome_treinador);
    if (eu != NULL) {
        printf("Você: %s (", eu->nome);
        imprimirTipos(eu);
        printf(") ");
        imprimirBarra(eu->hp_atual, eu->hp_max);
        printf("  vel %d\n", velocidadeEmBatalha(eu));
    }
    if (opp != NULL) {
        printf("Rival: %s (", opp->nome);
        imprimirTipos(opp);
        printf(") ");
        imprimirBarra(opp->hp_atual, opp->hp_max);
        printf("  vel %d\n", velocidadeEmBatalha(opp));
    }

    printf("\nGolpes:\n");
    if (eu != NULL) {
        for (i = 0; i < MAX_GOLPES; i++) {
            if (i < eu->qtd_golpes) {
                const Golpe *golpe = &eu->golpes[i];
                printf("  %d) %s [%s, poder %d", i + 1, golpe->nome, nomeTipo(golpe->tipo), golpe->poder);
                if (golpe->prioridade > 0) {
                    printf(", prioridade %d", golpe->prioridade);
                }
                printf("]\n");
            } else {
                printf("  %d) (vazio)\n", i + 1);
            }
        }
    }
    printf("  5) Trocar Pokémon\n");
    printf("  6) Ver histórico\n");
    printf("  7) Desistir\n");
}

static int escolherTroca(const VisaoBatalha *visao, int obrigatorio, int *posicao) {
    for (;;) {
        char linha[LINHA_MAX];
        int escolha = 0;
        int i;
        int n = tamanhoTime(visao->time_jogador);
        const Pokemon *escolhido;

        printf("\nQuem entra? Posições do time");
        if (!obrigatorio) {
            printf(" (0 cancela)");
        }
        printf(":\n");
        for (i = 0; i < n; i++) {
            const Pokemon *pokemon = pokemonNaPosicao(visao->time_jogador, i);
            printf("  %d) %s ", i + 1, pokemon->nome);
            imprimirBarra(pokemon->hp_atual, pokemon->hp_max);
            if (pokemon->hp_atual <= 0) {
                printf(" desmaiado");
            }
            if (i == posicaoDoAtivo(visao->time_jogador)) {
                printf("  (em campo)");
            }
            printf("\n");
        }
        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            return 0;
        }
        if (!interpretarInteiro(linha, &escolha)) {
            printf("Digite o número da posição.\n");
            continue;
        }
        if (!obrigatorio && escolha == 0) {
            return -1;
        }
        escolhido = pokemonNaPosicao(visao->time_jogador, escolha - 1);
        if (escolhido == NULL) {
            printf("Posição inválida.\n");
            continue;
        }
        if (escolhido->hp_atual <= 0) {
            printf("%s está desmaiado.\n", escolhido->nome);
            continue;
        }
        if (escolha - 1 == posicaoDoAtivo(visao->time_jogador)) {
            printf("%s já está em campo.\n", escolhido->nome);
            continue;
        }
        *posicao = escolha - 1;
        return 1;
    }
}

static int pedirAcaoTerminal(void *contexto, const VisaoBatalha *visao, AcaoJogador *acao) {
    (void)contexto;

    for (;;) {
        char linha[LINHA_MAX];
        int escolha = 0;
        const Pokemon *eu;

        mostrarStatusBatalha(visao);
        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            return 0;
        }
        if (!interpretarInteiro(linha, &escolha)) {
            printf("Digite um número da lista.\n");
            continue;
        }

        eu = obterAtivo(visao->time_jogador);
        if (escolha >= 1 && escolha <= MAX_GOLPES) {
            if (eu == NULL || escolha > eu->qtd_golpes) {
                printf("Esse golpe não existe.\n");
                continue;
            }
            acao->tipo = ACAO_GOLPE;
            acao->indice = escolha - 1;
            return 1;
        }
        if (escolha == 5) {
            int posicao = 0;
            int vivos = 0;
            int i;
            int n = tamanhoTime(visao->time_jogador);

            for (i = 0; i < n; i++) {
                const Pokemon *pokemon = pokemonNaPosicao(visao->time_jogador, i);
                if (pokemon != NULL && pokemon->hp_atual > 0 && i != posicaoDoAtivo(visao->time_jogador)) {
                    vivos++;
                }
            }
            if (vivos == 0) {
                printf("Não há outro Pokémon consciente para trocar.\n");
                continue;
            }

            {
                int troca = escolherTroca(visao, 0, &posicao);
                if (troca == 0) {
                    return 0;
                }
                if (troca < 0) {
                    continue;
                }
            }
            acao->tipo = ACAO_TROCAR;
            acao->indice = posicao;
            return 1;
        }
        if (escolha == 6) {
            printf("\n--- Histórico (do mais antigo ao mais novo) ---\n");
            mostrarHistorico(visao->log, stdout);
            printf("--- fim do histórico ---\n");
            continue;
        }
        if (escolha == 7) {
            char confirma[LINHA_MAX];
            printf("Tem certeza que quer desistir? (s/n)\n");
            prompt();
            if (!lerLinha(confirma, sizeof confirma)) {
                return 0;
            }
            if (confirma[0] == 's' || confirma[0] == 'S') {
                acao->tipo = ACAO_DESISTIR;
                acao->indice = 0;
                return 1;
            }
            continue;
        }
        printf("Opção inválida.\n");
    }
}

static int pedirTrocaTerminal(void *contexto, const VisaoBatalha *visao, int *posicao) {
    int i;
    int n;
    int vivos = 0;
    int unica = -1;

    (void)contexto;
    n = tamanhoTime(visao->time_jogador);
    for (i = 0; i < n; i++) {
        const Pokemon *pokemon = pokemonNaPosicao(visao->time_jogador, i);
        if (pokemon != NULL && pokemon->hp_atual > 0) {
            vivos++;
            unica = i;
        }
    }

    if (vivos == 1) {
        *posicao = unica;
        return 1;
    }

    printf("\nSeu Pokémon desmaiou!");
    for (;;) {
        int troca = escolherTroca(visao, 1, posicao);
        if (troca == 0) {
            return 0;
        }
        if (troca > 0) {
            return 1;
        }
    }
}

static void conduzirJornada(Jogo *jogo) {
    int montado;
    int resultado;

    if (!reiniciarDesafio(jogo)) {
        printf("Não foi possível montar a arena.\n");
        return;
    }

    printf("\nA arena tem %d desafiantes, do Brock ao Mewtwo.\n", tamanhoFila(&jogo->fila));
    printf("Entre um treinador e outro o time recupera o HP.\n");

    montado = montarTime(jogo);
    if (montado < 0) {
        /* EOF: o menu principal também deve parar. Sinalizado pelo time vazio e um flag? */
        return;
    }
    if (montado == 0) {
        return;
    }

    resultado = conduzirArena(&jogo->time, &jogo->fila, &jogo->log,
                              pedirAcaoTerminal, pedirTrocaTerminal, narrarTerminal, NULL);

    if (resultado == 1) {
        printf("\n*** Vitória! A arena é sua. Mewtwo foi derrotado. ***\n\n");
    } else if (resultado == 0) {
        printf("\n*** Derrota. Seu time não aguentou a arena. ***\n\n");
    } else if (resultado == -1) {
        printf("\nVocê deixou a arena.\n\n");
    }
}

/*
 * O montarTime devolve -1 no EOF, mas conduzirJornada engole isso.
 * Para encerrar o menu quando o script acaba no meio da montagem,
 * a jornada avisa por um campo. Mais simples: se stdin já acabou,
 * a próxima leitura do menu falha e o laço sai. Se o EOF aconteceu
 * dentro da jornada, a próxima lerLinha do menu também falha. Bom.
 * Se o EOF aconteceu e ainda há um prompt, fgets devolve NULL. OK.
 */

static void consultarPokedex(Jogo *jogo) {
    for (;;) {
        char linha[LINHA_MAX];
        int opcao = 0;

        printf("\n--- Pokédex ---\n");
        printf("1) Buscar por nome (tabela hash)\n");
        printf("2) Buscar por número (árvore AVL)\n");
        printf("3) Listar por número (percurso em ordem)\n");
        printf("4) Voltar\n");
        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            return;
        }
        if (!interpretarInteiro(linha, &opcao)) {
            printf("Digite um número de 1 a 4.\n");
            continue;
        }

        if (opcao == 1) {
            char nome[LINHA_MAX];
            printf("Nome: ");
            fflush(stdout);
            if (!lerLinha(nome, sizeof nome)) {
                return;
            }
            imprimirFicha(buscarPokemon(&jogo->pokedex, nome));
        } else if (opcao == 2) {
            char numero_texto[LINHA_MAX];
            int numero = 0;
            printf("Número: ");
            fflush(stdout);
            if (!lerLinha(numero_texto, sizeof numero_texto)) {
                return;
            }
            if (!interpretarInteiro(numero_texto, &numero)) {
                printf("Isso não é um número.\n");
            } else {
                imprimirFicha(buscarPorNumero(&jogo->arvore, numero));
            }
        } else if (opcao == 3) {
            ContextoListagem ctx;
            ctx.contagem = 0;
            printf("\n");
            percorrerEmOrdem(&jogo->arvore, imprimirResumoEspecie, &ctx);
            printf("Total: %d espécies.\n", ctx.contagem);
        } else if (opcao == 4) {
            return;
        } else {
            printf("Opção inválida.\n");
        }
    }
}

static const char *nomeCriterio(CriterioOrdenacao criterio) {
    switch (criterio) {
    case ORDENAR_POR_NUMERO:
        return "número";
    case ORDENAR_POR_NOME:
        return "nome";
    case ORDENAR_POR_VELOCIDADE:
        return "velocidade";
    case ORDENAR_POR_NIVEL:
        return "nível";
    default:
        return "?";
    }
}

static void organizarPokedex(Jogo *jogo) {
    for (;;) {
        char linha[LINHA_MAX];
        int opcao = 0;
        CriterioOrdenacao criterio;
        Pokemon vetor[64];
        int n;
        int i;

        printf("\n--- Organizar a Pokédex (merge sort) ---\n");
        printf("A hash não guarda ordem. O vetor sai dos baldes e o merge sort rearruma.\n");
        printf("1) Por número\n");
        printf("2) Por nome\n");
        printf("3) Por velocidade\n");
        printf("4) Voltar\n");
        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            return;
        }
        if (!interpretarInteiro(linha, &opcao)) {
            printf("Digite um número de 1 a 4.\n");
            continue;
        }
        if (opcao == 4) {
            return;
        }
        if (opcao < 1 || opcao > 3) {
            printf("Opção inválida.\n");
            continue;
        }

        if (opcao == 1) {
            criterio = ORDENAR_POR_NUMERO;
        } else if (opcao == 2) {
            criterio = ORDENAR_POR_NOME;
        } else {
            criterio = ORDENAR_POR_VELOCIDADE;
        }

        n = listarPokedex(&jogo->pokedex, vetor, 64);
        if (!ordenarPokedex(vetor, n, criterio)) {
            printf("Sem memória para ordenar.\n");
            continue;
        }

        printf("\nPokédex por %s:\n", nomeCriterio(criterio));
        for (i = 0; i < n; i++) {
            printf("%3d  %-14s  vel %3d\n", vetor[i].numero, vetor[i].nome, vetor[i].velocidade);
        }
    }
}

static void mostrarComoJogar(void) {
    printf("\nPokéBattle: Arena do Mewtwo\n");
    printf("Você monta um time de até 6 Pokémon da 1ª geração e enfrenta,\n");
    printf("nesta ordem, Brock, Misty, a Equipe Rocket, Ash e o Mewtwo.\n\n");
    printf("Em cada turno os dois escolhem um golpe. Quem age primeiro sai\n");
    printf("de um insertion sort: prioridade do golpe, depois velocidade.\n");
    printf("Ataque Rápido tem prioridade e fura a fila da velocidade.\n");
    printf("Trocar gasta o turno. Se o seu Pokémon desmaiar, outro entra.\n");
    printf("Entre treinadores o HP volta. Dentro da luta, não.\n\n");
    printf("Tipos importam: água contra pedra é super efetivo, elétrico\n");
    printf("contra terra não tem efeito. O histórico (opção 6) usa uma pilha.\n\n");
}

static void imprimirBanner(void) {
    printf("========================================\n");
    printf("  PokéBattle: Arena do Mewtwo\n");
    printf("  Ode ao Cinema — CESAR School AED\n");
    printf("========================================\n");
    printf("Inspirado em Pokémon: O Filme — Mewtwo Contra-Ataca (1998).\n");
}

int executarInterfaceTerminal(Jogo *jogo) {
    imprimirBanner();

    for (;;) {
        char linha[LINHA_MAX];
        int opcao = 0;

        printf("\n1) Nova jornada\n");
        printf("2) Consultar a Pokédex\n");
        printf("3) Organizar a Pokédex\n");
        printf("4) Como jogar\n");
        printf("5) Sair\n");
        prompt();
        if (!lerLinha(linha, sizeof linha)) {
            printf("\n");
            break;
        }
        if (!interpretarInteiro(linha, &opcao)) {
            printf("Digite um número de 1 a 5.\n");
            continue;
        }

        switch (opcao) {
        case 1:
            conduzirJornada(jogo);
            if (feof(stdin)) {
                return 0;
            }
            break;
        case 2:
            consultarPokedex(jogo);
            if (feof(stdin)) {
                return 0;
            }
            break;
        case 3:
            organizarPokedex(jogo);
            if (feof(stdin)) {
                return 0;
            }
            break;
        case 4:
            mostrarComoJogar();
            break;
        case 5:
            printf("Até a próxima batalha!\n");
            return 0;
        default:
            printf("Opção inválida.\n");
            break;
        }
    }

    printf("Até a próxima batalha!\n");
    return 0;
}
