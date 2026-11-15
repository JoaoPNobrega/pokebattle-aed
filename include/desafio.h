/* desafio.h — Puzzles espaciais do laboratório, sem raylib.
 * Grades pequenas têm tamanho fixo; o estado vive no nó da lista de salas.
 * Circuitos usam percurso sobre as conexões, não uma resposta de quiz. */
#ifndef DESAFIO_H
#define DESAFIO_H

typedef enum { DES_NENHUM, DES_CIRCUITO, DES_GELO, DES_AGUA, DES_RASTRO, DES_ESTANTES } TipoDesafio;
typedef enum { ACAO_USAR, ACAO_RESFRIAR, ACAO_CIMA, ACAO_DIREITA, ACAO_BAIXO,
               ACAO_ESQUERDA, ACAO_REINICIAR, ACAO_DESFAZER } AcaoDesafio;
typedef struct {
    TipoDesafio tipo;
    int rotacao[9];
    int gelo[12];
    int calor;
    int tanque_a, tanque_b;
    int rastros;
    unsigned int examinados;
    int operador, operador_anterior;
    unsigned long caixas, caixas_anteriores;
    int pode_desfazer;
    int concluido;
    int movimentos;
} Desafio;

/* iniciarDesafio: zera estado e monta a grade do tipo informado. O(1).
 * Parâmetros: destino e tipo. Retorno: nenhum; NULL é ignorado. */
void iniciarDesafio(Desafio *desafio, TipoDesafio tipo);
/* mascaraCircuito: portas da peça 0..8: cima=1, direita=2, baixo=4, esquerda=8.
 * Parâmetros: estado e célula. Retorno: máscara ou 0 se inválido. O(1). */
int mascaraCircuito(const Desafio *desafio, int celula);
/* circuitoAlcancado: percorre ligações recíprocas a partir da fonte.
 * Parâmetro: estado. Retorno: bits das células energizadas. O(v+e), v <= 9. */
unsigned int circuitoAlcancado(const Desafio *desafio);
/* paredeEstantes: informa colisão na grade 6x6. O(1).
 * Parâmetro: célula. Retorno: 1 parede/fora da grade, 0 livre. */
int paredeEstantes(int celula);
/* distanciaRastro: distância Manhattan ao próximo eco (busca 5x4). O(1).
 * Parâmetros: estado e célula. Retorno: distância ou -1 se inválido/concluído. */
int distanciaRastro(const Desafio *desafio, int celula);
/* comandarDesafio: gira peça, aquece célula, opera válvula, examina ou empurra.
 * Parâmetros: estado, ação e alvo (célula/botão, conforme tipo).
 * Retorno: -1 inválido, 0 bloqueio/erro, 1 ação, 2 conclusão. O(v+e), v <= 36.
 * Reinício e desfazer não concluem nem liberam sala por conta própria. */
int comandarDesafio(Desafio *desafio, AcaoDesafio acao, int alvo);
#endif
