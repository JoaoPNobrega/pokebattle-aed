/* Puzzles das salas: circuito, gelo, água, rastro e estantes */

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

/* Monta a grade do puzzle */
void iniciarDesafio(Desafio *desafio, TipoDesafio tipo);
/* Portas da peça: cima, direita, baixo, esquerda */
int mascaraCircuito(const Desafio *desafio, int celula);
/* Percurso no circuito, a partir da fonte */
unsigned int circuitoAlcancado(const Desafio *desafio);
/* Parede na grade das estantes */
int paredeEstantes(int celula);
/* Distância até o próximo rastro */
int distanciaRastro(const Desafio *desafio, int celula);
/* Aplica a ação do puzzle */
int comandarDesafio(Desafio *desafio, AcaoDesafio acao, int alvo);
#endif
