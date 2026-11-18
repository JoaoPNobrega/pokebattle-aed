/*
 * main.c
 * ------
 * PokéBattle: O Caso do Laboratório.
 *
 * Sem argumentos, abre em tela cheia (raylib); --janela abre em janela.
 * Com --simular, joga o caso sozinho, sem janela, e devolve
 * 0 se o detetive vence o capanga, 1 se perde, 2 se nem monta o caso.
 */

#include "simulacao.h"
#include "ui_raylib.h"

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <stdint.h>
#include <unistd.h>

/* O Finder pode abrir o app a partir de outro diretório. */
static void usarRecursosDoApp(void) {
    static const char sufixo[] = "/MacOS/pokebattle";
    static const char recursos[] = "/Resources";
    char executavel[4096];
    uint32_t capacidade = sizeof executavel;
    size_t tamanho;
    size_t inicio;

    if (_NSGetExecutablePath(executavel, &capacidade) != 0) return;
    tamanho = strlen(executavel);
    if (tamanho < sizeof sufixo - 1) return;
    inicio = tamanho - (sizeof sufixo - 1);
    if (strcmp(executavel + inicio, sufixo) != 0) return;
    memcpy(executavel + inicio, recursos, sizeof recursos);
    chdir(executavel);
}
#endif

int main(int argc, char **argv) {
#ifdef __APPLE__
    usarRecursosDoApp();
#endif
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc > 1 && strcmp(argv[1], "--simular") == 0) {
        int resultado = simularArenaAutomatica();
        if (resultado < 0) {
            return 2;
        }
        return resultado == 1 ? 0 : 1;
    }
    if (argc > 1 && strcmp(argv[1], "--capturas") == 0) {
        return capturarTelasDemonstracao();
    }

    return executarInterfaceRaylib(!(argc > 1 && strcmp(argv[1], "--janela") == 0));
}
