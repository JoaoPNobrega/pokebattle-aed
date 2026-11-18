/* Estado do caso: salas, fila e batalha */

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

/* Itens da bolsa */
#define ITEM_POCAO 0
#define ITEM_SUPER_POCAO 1
#define ITEM_POKE_BOLA 2
#define ITEM_MASTER_BOLA 3
#define ITEM_CONTAGEM 4

int iniciarCaso(Caso *caso);
void encerrarCaso(Caso *caso);

/* Recomeça a investigação */
int comecarCaso(Caso *caso);

/* Habilidade e passos na sala atual */
int casoUsarHabilidade(Caso *caso);
int casoAvancar(Caso *caso);
int casoVoltar(Caso *caso);

/* Manda uma ação para o puzzle da sala */
int casoComandarPuzzle(Caso *caso, AcaoDesafio acao, int alvo);
/* Pikachu acende um refletor */
int casoAcenderPonto(Caso *caso, int ponto);
/* Área secreta e selos */
int casoAbrirAreaSecreta(Caso *caso);
void casoSairAreaSecreta(Caso *caso);
int casoAtivarSeloSecreto(Caso *caso, int selo);

int casoObterChaveSecreta(Caso *caso);
int casoTemChaveSecreta(const Caso *caso);

/* Altar do santuário, pelo tipo do time */
int casoAtivarAltarElemental(Caso *caso, int altar);
/* Guarda da sala, depois do puzzle */
int casoIniciarDuelo(Caso *caso);
/* Texto da próxima tarefa */
const char *casoObjetivoAtual(const Caso *caso);

/* Captura: 0 cima, 1 direita, 2 baixo, 3 esquerda, 4 Z */
int casoComandoCaptura(const Caso *caso);
int casoPrazoCaptura(const Caso *caso);
int casoResponderCaptura(Caso *caso, int comando, int tempo_ms);

/* Página do documento da Jaula */
const char *casoPaginaDocumento(const Caso *caso, int pagina);

/* Encontro opcional, sem tirar ninguém da fila */
int casoIniciarTreino(Caso *caso);

/* Recupera HP fora da luta */
int casoDescansar(Caso *caso);

/* Tira o próximo treinador da fila e começa a luta */
int casoIniciarBatalha(Caso *caso);

/* Um turno: insertion sort e lista circular */
int casoExecutarGolpe(Caso *caso, int indice_golpe, SaidaBatalha *saida);

/* Copia o selvagem para o fim do time */
int casoCapturar(Caso *caso);

const char *casoNomeItem(int item);
int casoQuantidadeItem(const Caso *caso, int item);

/* Usa item. Poké Bola não tem sorteio. */
int casoUsarItem(Caso *caso, int item, SaidaBatalha *saida);

/* Fuga sem sorteio. Chefe recusa. */
int casoTentarFugir(Caso *caso, SaidaBatalha *saida);

void casoMarcarVisto(Caso *caso, int numero);
int casoFoiVisto(const Caso *caso, int numero);

/* Itens espalhados pelo mapa */
int casoColetarItemMapa(Caso *caso, int id_item, char *mensagem, size_t tamanho);
int casoItemJaColetado(const Caso *caso, int id_item);

/* Computador: depósito, retirada e líder */
int casoDepositarPokemon(Caso *caso, int pos_time);
int casoRetirarPokemon(Caso *caso, int pos_reserva);
int casoTrocarOrdemTime(Caso *caso, int i, int j);
int casoDefinirLider(Caso *caso, int pos);
int casoTamanhoReserva(const Caso *caso);
const Pokemon *casoPokemonReserva(const Caso *caso, int pos);

/* Mewtwo entra no time ou no PC */
int casoConvidarMewtwo(Caso *caso);

/* Luta contra um lendário */
int casoIniciarEncontroLendario(Caso *caso, int numero_lendario);

#endif
