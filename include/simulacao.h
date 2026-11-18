/* Partida automática, sem janela */

#ifndef SIMULACAO_H
#define SIMULACAO_H
#include "caso.h"

/* Resolve o puzzle da sala atual */
int simularPuzzleAtual(Caso *caso);

/* Manda os comandos da captura */
int simularCaptura(Caso *caso);

/* Joga o caso até o fim */
int simularArenaAutomatica(void);

#endif
