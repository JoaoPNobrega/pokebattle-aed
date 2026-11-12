/*
 * historia.h
 * ----------
 * O texto do caso, sem desenhar e sem decidir regra.
 *
 * A janela só pede a frase e mostra na caixa. Quem avança a
 * história continua sendo a sala, o puzzle e a luta.
 */

#ifndef HISTORIA_H
#define HISTORIA_H

/*
 * historiaIntro / historiaDesfecho
 * Páginas da abertura e do fim. pagina começa em 0.
 *
 * Retorno: texto fixo, ou NULL quando as páginas acabaram.
 * Complexidade: O(1).
 */
const char *historiaIntro(int pagina);
const char *historiaDesfecho(int pagina);

/*
 * historiaAoEntrar / historiaAoResolver
 * Uma fala ao pisar na sala pela primeira vez, e outra quando
 * o puzzle daquela sala é resolvido. Sala desconhecida devolve NULL.
 *
 * Complexidade: O(k), k = tamanho do nome (a comparação).
 */
const char *historiaAoEntrar(const char *nome_sala);
const char *historiaAoResolver(const char *nome_sala);

#endif
