/* Matriz de efetividade da 1ª geração */

#ifndef TIPOS_H
#define TIPOS_H

typedef enum {
    TIPO_NENHUM = -1,
    TIPO_NORMAL = 0,
    TIPO_FOGO,
    TIPO_AGUA,
    TIPO_ELETRICO,
    TIPO_GRAMA,
    TIPO_GELO,
    TIPO_LUTADOR,
    TIPO_VENENO,
    TIPO_TERRA,
    TIPO_VOADOR,
    TIPO_PSIQUICO,
    TIPO_INSETO,
    TIPO_PEDRA,
    TIPO_FANTASMA,
    TIPO_DRAGAO,
    TIPO_CONTAGEM
} Tipo;

/* Preenche a matriz: 0 imune, 50 fraco, 100 neutro, 200 forte */
void inicializarTipos(void);

/* Nome do tipo */
const char *nomeTipo(Tipo tipo);

/* Golpe contra um tipo só */
int efetividadePar(Tipo ataque, Tipo defesa);

/* Junta os dois tipos do defensor */
int multiplicadorEfetividade(Tipo ataque, Tipo def1, Tipo def2);

#endif
