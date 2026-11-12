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
} Caso;

/* Itens da bolsa. A quantidade mora no Caso, não na janela. */
#define ITEM_POCAO 0
#define ITEM_SUPER_POCAO 1
#define ITEM_POKE_BOLA 2
#define ITEM_CONTAGEM 3

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
 * Retorno: 1 se a ação aconteceu, 0 se foi recusada.
 * Complexidade: O(t) na habilidade (percorre o time); O(1) ao andar.
 */
int casoUsarHabilidade(Caso *caso);
int casoAvancar(Caso *caso);
int casoVoltar(Caso *caso);

/*
 * casoIniciarBatalha
 * Se a sala atual pede luta e ela ainda não foi vencida, tira o
 * próximo treinador da fila (só na primeira tentativa) e entra em batalha.
 * Retorno: 1 se a luta começou, 0 se não havia luta.
 * Complexidade: O(1).
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
 * Complexidade: O(g + n^2) com g <= 4 golpes e n <= 2 atores no turno.
 */
int casoExecutarGolpe(Caso *caso, int indice_golpe, SaidaBatalha *saida);

/*
 * casoCapturar
 * Copia o selvagem pendente para o fim do time, com HP cheio.
 * Só existe pendência depois de vencer uma sala marcada como capturável.
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
 * Poké Bola só serve em sala capturável: pega se o HP restante
 * for no máximo a metade. Não há sorteio, para a defesa repetir
 * o mesmo resultado.
 *
 * Usar de verdade gasta o turno: o oponente age em seguida, pela
 * mesma lista circular (o jogador entra com golpe -1 e fica de fora).
 * Item inválido ou sala de chefe não gasta a unidade nem o turno.
 *
 * Retorno: o mesmo de casoExecutarGolpe (1 continua, 0 vitória,
 * -1 derrota, -2 não usou).
 * Complexidade: O(g + n^2), g <= 4 e n <= 2, quando o oponente age.
 */
int casoUsarItem(Caso *caso, int item, SaidaBatalha *saida);

/*
 * casoTentarFugir
 * Chefe recusa, sem turno gasto. No selvagem, a fuga sai sem
 * sorteio: a tentativa i (1, 2, 3...) funciona quando
 * velocidade_do_jogador * i >= velocidade_do_oponente.
 * Se falhar, o oponente age.
 *
 * Retorno: 3 se fugiu, ou o código de casoExecutarGolpe.
 * Complexidade: O(g + n^2) quando o oponente chega a agir.
 */
int casoTentarFugir(Caso *caso, SaidaBatalha *saida);

#endif
