/*
 * caso.h
 * ------
 * O estado do mistério, sem desenhar nada.
 *
 * Junta o que já existia (Pokédex, time, fila de confrontos, pilha
 * de histórico) com a lista de salas. A fila deixa de ser a arena
 * do filme e passa a ser a fila de lutas do caso: o capanga sai
 * primeiro; o chefão, quando existir, entra atrás dele.
 */

#ifndef CASO_H
#define CASO_H

#include "batalha.h"
#include "jogo.h"
#include "sala.h"

typedef struct {
    Jogo nucleo;
    ListaSalas salas;
    Treinador oponente_atual;
    int oponente_carregado; /* 1 = oponente_atual é dono de um time */
    int em_batalha;
    int captura_disponivel; /* 1 = há um Pokémon selvagem esperando captura */
    Pokemon captura_pendente;
    int caso_encerrado; /* 1 = o chefão foi derrotado */
    int pocoes;
    int super_pocoes;
    int pokebolas;
    int tentativas_fuga; /* conta as fugas desta luta, sem sorteio */
    int treino_ativo; /* encontro opcional, não consome a fila do caso */
    int encontros_treino;
    int batalha_npc;
    int captura_acertos;
    int captura_erros;
    unsigned int semente_sorteio; /* encontros opcionais, lendários e shiny */
    int area_secreta;
    int selos_secretos;
    int master_bolas;
    int chave_secreta;
    int altares_secretos;
    unsigned char pokedex_vistos[152];
    unsigned int itens_mapa;
    ListaTime reserva_pc;
} Caso;

/* Itens da bolsa. A quantidade mora no Caso, não na janela. */
#define ITEM_POCAO 0
#define ITEM_SUPER_POCAO 1
#define ITEM_POKE_BOLA 2
#define ITEM_MASTER_BOLA 3
#define ITEM_CONTAGEM 4

/*
 * iniciarCaso
 * Carrega a Pokédex e deixa salas, time e fila vazios.
 * Retorno: 1 se a Pokédex entrou, 0 se faltou memória.
 * Complexidade: O(n log n), n = espécies.
 */
int iniciarCaso(Caso *caso);

/*
 * encerrarCaso
 * Libera Pokédex, time, fila, log, salas e o oponente carregado.
 * Complexidade: O(n) somando essas estruturas.
 */
void encerrarCaso(Caso *caso);

/*
 * comecarCaso
 * Recomeça a investigação: time só com o Pikachu, a sequência de
 * salas do laboratório e a fila de lutas até o chefão.
 * Retorno: 1 se montou, 0 se faltou dado ou memória.
 * Complexidade: O(1) além das liberações do estado anterior.
 */
int comecarCaso(Caso *caso);

/*
 * casoUsarHabilidade / casoAvancar / casoVoltar
 * Atalhos que operam na sala atual e anotam o histórico.
 * Habilidade é recusada durante a luta ou antes de vencer o encontro.
 * Na Jaula, a busca psíquica revela um documento e registra suas dicas no log.
 * Habilidade só conclui depois do desafio espacial da sala.
 * Retorno: 1 se a ação aconteceu, 0 se foi recusada.
 * Complexidade: O(t) na habilidade (percorre o time); O(1) ao andar.
 */
int casoUsarHabilidade(Caso *caso);
int casoAvancar(Caso *caso);
int casoVoltar(Caso *caso);

/* casoComandarPuzzle: valida parceiro e aplica ação/alvo ao desafio da sala.
 * Retorno: -1 inválido, 0 erro/bloqueio, 1 ação, 2 sala resolvida. O(t+v+e).
 * Parâmetros: caso, ação do desafio e célula/válvula. t = time; v <= 36. */
int casoComandarPuzzle(Caso *caso, AcaoDesafio acao, int alvo);
/* casoAcenderPonto: Pikachu religa refletor 0 ou 1 após restaurar o gerador.
 * Parâmetros: caso e ponto. Retorno: 1 aceso, 0 indisponível. O(t).
 * O estado das luzes pertence à sala e participa da trava de avanço. */
int casoAcenderPonto(Caso *caso, int ponto);
/* casoAbrirAreaSecreta / casoAtivarSeloSecreto
 * Abre o santuário após resolver o Arquivo e ativa os selos na sequência.
 * Parâmetros: caso; selo 0..2. Retorno: 1 abertura/ação, 2 prêmio, -1 erro de ordem, 0 bloqueio.
 * Complexidade: O(s) ao buscar a sala; O(1) por selo, s = salas.
 */
int casoAbrirAreaSecreta(Caso *caso);
/* Marca a saída da área. Parâmetro: caso; retorno: nenhum; O(1). */
void casoSairAreaSecreta(Caso *caso);
int casoAtivarSeloSecreto(Caso *caso, int selo);

/* casoObterChaveSecreta / casoTemChaveSecreta
 * Coleta ou consulta a Chave do Santuário encontrada no Arquivo.
 * Parâmetro: caso. Retorno: 1 se coletou/possui, 0 se indisponível. O(1).
 */
int casoObterChaveSecreta(Caso *caso);
int casoTemChaveSecreta(const Caso *caso);

/* casoAtivarAltarElemental
 * Ativa altar elemental no santuário exigindo o tipo/habilidade correto:
 * Altar 0: Fogo (HAB_FOGO), Altar 1: Psíquico (HAB_PSIQUICA), Altar 2: Trovão (HAB_ELETRICA).
 * Retorno: 1 ativado, 2 todos ativados (Master Ball liberada), -1 tipo incorreto, 0 já ativado/erro. O(t).
 */
int casoAtivarAltarElemental(Caso *caso, int altar);
/* casoIniciarDuelo: inicia o guarda da sala depois do puzzle e das luzes.
 * Parâmetro: caso. Retorno: 1 iniciou, 0 recusado. O(t), retira da fila do caso.
 * t = tamanho do time, se precisar escolher outro parceiro vivo. */
int casoIniciarDuelo(Caso *caso);
/* casoObjetivoAtual: texto curto da próxima tarefa. Não altera o estado.
 * Parâmetro: caso. Retorno: texto estático (nunca NULL). O(1). */
const char *casoObjetivoAtual(const Caso *caso);
/* Captura: comandos 0=cima,1=direita,2=baixo,3=esquerda,4=Z.
 * casoComandoCaptura/casoPrazoCaptura leem próximo comando e prazo em ms. O(1).
 * Parâmetro: caso. Retorno: comando ou -1; prazo ou 0 se indisponível.
 * casoResponderCaptura valida comando e tempo desde o anterior. Um erro
 * reinicia a contenção sem perder o parceiro obrigatório. O(1).
 * Parâmetros: caso, comando, tempo_ms. Retorno: -1 inválido,0 erro,1 acerto,2 pronto.
 * Só cinco acertos liberam casoCapturar. Não há sorteio. */
int casoComandoCaptura(const Caso *caso);
int casoPrazoCaptura(const Caso *caso);
int casoResponderCaptura(Caso *caso, int comando, int tempo_ms);

/* casoPaginaDocumento: lê página 0 ou 1 do documento revelado na Jaula.
 * Parâmetros: caso e índice da página. Retorno: texto ou NULL se indisponível.
 * O(s), s = salas. A descoberta é o puzzle resolvido, sem inventário paralelo. */
const char *casoPaginaDocumento(const Caso *caso, int pagina);

/* casoIniciarTreino: cria encontro opcional em Pátio/Tanque/Oficina/Jaula.
 * Exige encontro obrigatório concluído e capturado; nunca tira nós da fila.
 * Retorno: 1 se começou, 0 se indisponível. O(t), t = tamanho do time.
 * Grama e escombros são apenas a apresentação dessas áreas na janela. */
int casoIniciarTreino(Caso *caso);

/* casoDescansar: restaura HP fora da luta; a janela repõe os PP visuais.
 * Parâmetro: caso. Retorno: 1 se descansou, 0 durante luta ou captura pendente.
 * O(t), t = tamanho do time. */
int casoDescansar(Caso *caso);

/*
 * casoIniciarBatalha
 * Se a sala atual pede luta e ela ainda não foi vencida, tira o
 * próximo treinador da fila (só na primeira tentativa) e entra em batalha.
 * Retorno: 1 se a luta começou, 0 se não havia luta.
 * Complexidade: O(t), t = soma dos times quando restaura uma tentativa anterior.
 */
int casoIniciarBatalha(Caso *caso);

/*
 * casoExecutarGolpe
 * Um turno da luta, com o golpe escolhido pelo jogador (índice no
 * Pokémon ativo). O oponente escolhe o dele por maior dano. A ordem
 * sai do insertion sort e da lista circular, como na batalha antiga.
 *
 * Retorno:
 *   1  a luta continua
 *   0  o jogador venceu
 *  -1  o jogador perdeu
 *  -2  não havia luta, ou o índice do golpe é inválido
 * Complexidade: O(g + n^2 + t), g <= 4, n <= 2 e t = tamanho dos times.
 */
int casoExecutarGolpe(Caso *caso, int indice_golpe, SaidaBatalha *saida);

/*
 * casoCapturar
 * Copia o selvagem pendente para o fim do time, com HP cheio.
 * Existe pendência após vitória capturável do caso ou treino com vaga permitida.
 * Exige cinco comandos aceitos por casoResponderCaptura antes de copiar.
 *
 * Retorno: 1 se entrou no time, 0 se não havia ninguém ou o time está cheio.
 * Complexidade: O(1) (o time tem no máximo 6).
 */
int casoCapturar(Caso *caso);

/*
 * casoNomeItem / casoQuantidadeItem
 * Nome em português e quantas unidades restam. item fora da faixa
 * devolve NULL ou 0.
 *
 * Complexidade: O(1).
 */
const char *casoNomeItem(int item);
int casoQuantidadeItem(const Caso *caso, int item);

/*
 * casoUsarItem
 * Gasta um item na luta. Poção cura 20 de HP. Super Poção cura 50.
 * Poké Bola serve em sala capturável ou treino com vaga permitida: pega se o HP restante
 * for no máximo a metade. Não há sorteio, para a defesa repetir
 * o mesmo resultado.
 *
 * Usar de verdade gasta o turno: o oponente age em seguida, pela
 * mesma lista circular (o jogador entra com golpe -1 e fica de fora).
 * Item inválido ou sala de chefe não gasta a unidade nem o turno.
 *
 * Retorno: o mesmo de casoExecutarGolpe (1 continua, 0 vitória,
 * -1 derrota, -2 não usou).
 * Complexidade: O(g + n^2 + t), g <= 4, n <= 2, t = tamanho dos times.
 */
int casoUsarItem(Caso *caso, int item, SaidaBatalha *saida);

/*
 * casoTentarFugir
 * Chefe recusa, sem turno gasto. No selvagem, a fuga sai sem
 * sorteio: a tentativa i (1, 2, 3...) funciona quando
 * velocidade_do_jogador * i >= velocidade_do_oponente.
 * Se falhar, o oponente age.
 * Fugir de treino libera o oponente opcional sem alterar a fila do caso.
 *
 * Retorno: 3 se fugiu, ou o código de casoExecutarGolpe.
 * Complexidade: O(g + n^2 + t) quando o oponente chega a agir.
 */
int casoTentarFugir(Caso *caso, SaidaBatalha *saida);

/*
 * casoMarcarVisto / casoFoiVisto
 * Registra ou consulta se uma espécie foi encontrada/enfrentada pelo detetive.
 *
 * Complexidade: O(1).
 */
void casoMarcarVisto(Caso *caso, int numero);
int casoFoiVisto(const Caso *caso, int numero);

/*
 * casoColetarItemMapa / casoItemJaColetado
 * Coleta itens espalhados pelos setores (Rare Candies, poções e bolas).
 *
 * Complexidade: O(1).
 */
int casoColetarItemMapa(Caso *caso, int id_item, char *mensagem, size_t tamanho);
int casoItemJaColetado(const Caso *caso, int id_item);

/*
 * Computador da Equipe (Terminal de Armazenamento Pokémon):
 * Gerenciamento de depósito, retirada, troca de ordem e definição do líder ativo.
 * Complexidade: O(n) na lista duplamente encadeada.
 */
int casoDepositarPokemon(Caso *caso, int pos_time);
int casoRetirarPokemon(Caso *caso, int pos_reserva);
int casoTrocarOrdemTime(Caso *caso, int i, int j);
int casoDefinirLider(Caso *caso, int pos);
int casoTamanhoReserva(const Caso *caso);
const Pokemon *casoPokemonReserva(const Caso *caso, int pos);

/*
 * casoConvidarMewtwo
 * Prepara o Mewtwo (espécie #150, nível 50) e adiciona à equipe ou PC do detetive.
 *
 * Parâmetro: caso.
 * Retorno: 1 se integrado com sucesso, 0 caso contrário.
 * Complexidade: O(log n) na AVL da Pokédex + O(1) na inserção da lista duplamente encadeada.
 */
int casoConvidarMewtwo(Caso *caso);

/*
 * casoIniciarEncontroLendario
 * Inicia confronto direto com lendário (#144 Articuno, #145 Zapdos, #146 Moltres, #150 Mewtwo, #151 Mew).
 * Complexidade: O(log n) na busca da AVL + O(1) na inserção.
 */
int casoIniciarEncontroLendario(Caso *caso, int numero_lendario);

#endif

