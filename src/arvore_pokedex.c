/*
 * arvore_pokedex.c
 * ----------------
 * AVL indexada pelo número da Pokédex.
 *
 * Fator de balanceamento = altura(esquerda) - altura(direita).
 * Fora de {-1, 0, 1} a árvore gira:
 *   +2 com filho esquerdo pendendo à esquerda  -> rotação à direita
 *   +2 com filho esquerdo pendendo à direita   -> rotação dupla (esq-dir)
 *   -2 com filho direito pendendo à direita    -> rotação à esquerda
 *   -2 com filho direito pendendo à esquerda   -> rotação dupla (dir-esq)
 *
 * A altura guardada em cada nó deixa o fator O(1).
 */

#include "arvore_pokedex.h"

#include <stdlib.h>

static int alturaNo(const NoAvl *no) {
    if (no == NULL) {
        return 0;
    }
    return no->altura;
}

static int fator(const NoAvl *no) {
    if (no == NULL) {
        return 0;
    }
    return alturaNo(no->esq) - alturaNo(no->dir);
}

static void atualizarAltura(NoAvl *no) {
    int esquerda;
    int direita;

    if (no == NULL) {
        return;
    }
    esquerda = alturaNo(no->esq);
    direita = alturaNo(no->dir);
    no->altura = (esquerda > direita ? esquerda : direita) + 1;
}

/* y desce para a direita, x (filho esquerdo) sobe. */
static NoAvl *rotacionarDireita(NoAvl *y) {
    NoAvl *x = y->esq;
    NoAvl *meio = x->dir;

    x->dir = y;
    y->esq = meio;
    atualizarAltura(y);
    atualizarAltura(x);
    return x;
}

/* x desce para a esquerda, y (filho direito) sobe. */
static NoAvl *rotacionarEsquerda(NoAvl *x) {
    NoAvl *y = x->dir;
    NoAvl *meio = y->esq;

    y->esq = x;
    x->dir = meio;
    atualizarAltura(x);
    atualizarAltura(y);
    return y;
}

static NoAvl *balancear(NoAvl *no) {
    int fb;

    if (no == NULL) {
        return NULL;
    }

    atualizarAltura(no);
    fb = fator(no);

    if (fb > 1 && fator(no->esq) >= 0) {
        return rotacionarDireita(no);
    }
    if (fb > 1 && fator(no->esq) < 0) {
        no->esq = rotacionarEsquerda(no->esq);
        return rotacionarDireita(no);
    }
    if (fb < -1 && fator(no->dir) <= 0) {
        return rotacionarEsquerda(no);
    }
    if (fb < -1 && fator(no->dir) > 0) {
        no->dir = rotacionarDireita(no->dir);
        return rotacionarEsquerda(no);
    }
    return no;
}

static NoAvl *criarNo(const Pokemon *pokemon) {
    NoAvl *no = (NoAvl *)calloc(1, sizeof *no);
    if (no == NULL) {
        return NULL;
    }
    no->pokemon = *pokemon;
    no->altura = 1;
    return no;
}

/*
 * inseriu: 1 nó novo, 0 duplicata, -1 sem memória.
 * Em falta de memória a árvore fica como estava até o ponto da falha
 * (os nós já inseridos em chamadas anteriores continuam válidos).
 */
static NoAvl *inserirRec(NoAvl *no, const Pokemon *pokemon, int *inseriu) {
    if (no == NULL) {
        NoAvl *novo = criarNo(pokemon);
        if (novo == NULL) {
            *inseriu = -1;
            return NULL;
        }
        *inseriu = 1;
        return novo;
    }

    if (pokemon->numero < no->pokemon.numero) {
        no->esq = inserirRec(no->esq, pokemon, inseriu);
    } else if (pokemon->numero > no->pokemon.numero) {
        no->dir = inserirRec(no->dir, pokemon, inseriu);
    } else {
        *inseriu = 0;
        return no;
    }

    if (*inseriu < 0) {
        return no;
    }
    return balancear(no);
}

static const NoAvl *buscarRec(const NoAvl *no, int numero) {
    if (no == NULL) {
        return NULL;
    }
    if (numero < no->pokemon.numero) {
        return buscarRec(no->esq, numero);
    }
    if (numero > no->pokemon.numero) {
        return buscarRec(no->dir, numero);
    }
    return no;
}

static NoAvl *noMinimo(NoAvl *no) {
    while (no != NULL && no->esq != NULL) {
        no = no->esq;
    }
    return no;
}

static NoAvl *removerRec(NoAvl *no, int numero, int *achou) {
    if (no == NULL) {
        return NULL;
    }

    if (numero < no->pokemon.numero) {
        no->esq = removerRec(no->esq, numero, achou);
    } else if (numero > no->pokemon.numero) {
        no->dir = removerRec(no->dir, numero, achou);
    } else {
        NoAvl *filho;

        *achou = 1;
        if (no->esq == NULL || no->dir == NULL) {
            filho = (no->esq != NULL) ? no->esq : no->dir;
            free(no);
            /* O pai rebalanceia na volta da recursão. */
            return filho;
        }

        /*
         * Dois filhos: o sucessor (menor da subárvore direita) ocupa
         * o lugar, e a remoção de verdade acontece lá embaixo, onde
         * o sucessor tem no máximo um filho.
         */
        {
            NoAvl *sucessor = noMinimo(no->dir);
            no->pokemon = sucessor->pokemon;
            no->dir = removerRec(no->dir, sucessor->pokemon.numero, achou);
        }
    }

    return balancear(no);
}

static void emOrdemRec(const NoAvl *no, VisitaPokemon visita, void *contexto) {
    if (no == NULL || visita == NULL) {
        return;
    }
    emOrdemRec(no->esq, visita, contexto);
    visita(&no->pokemon, contexto);
    emOrdemRec(no->dir, visita, contexto);
}

static void liberarRec(NoAvl *no) {
    if (no == NULL) {
        return;
    }
    liberarRec(no->esq);
    liberarRec(no->dir);
    free(no);
}

void criarArvorePokedex(ArvorePokedex *arvore) {
    if (arvore == NULL) {
        return;
    }
    arvore->raiz = NULL;
    arvore->quantidade = 0;
}

void liberarArvorePokedex(ArvorePokedex *arvore) {
    if (arvore == NULL) {
        return;
    }
    liberarRec(arvore->raiz);
    arvore->raiz = NULL;
    arvore->quantidade = 0;
}

int inserirNaArvore(ArvorePokedex *arvore, const Pokemon *pokemon) {
    int inseriu = 0;

    if (arvore == NULL || pokemon == NULL) {
        return 0;
    }

    arvore->raiz = inserirRec(arvore->raiz, pokemon, &inseriu);
    if (inseriu == 1) {
        arvore->quantidade++;
        return 1;
    }
    return 0;
}

const Pokemon *buscarPorNumero(const ArvorePokedex *arvore, int numero) {
    const NoAvl *no;

    if (arvore == NULL) {
        return NULL;
    }
    no = buscarRec(arvore->raiz, numero);
    if (no == NULL) {
        return NULL;
    }
    return &no->pokemon;
}

int removerDaArvore(ArvorePokedex *arvore, int numero) {
    int achou = 0;

    if (arvore == NULL) {
        return 0;
    }

    arvore->raiz = removerRec(arvore->raiz, numero, &achou);
    if (achou) {
        arvore->quantidade--;
        return 1;
    }
    return 0;
}

void percorrerEmOrdem(const ArvorePokedex *arvore, VisitaPokemon visita, void *contexto) {
    if (arvore == NULL) {
        return;
    }
    emOrdemRec(arvore->raiz, visita, contexto);
}

int alturaDaArvore(const ArvorePokedex *arvore) {
    if (arvore == NULL) {
        return 0;
    }
    return alturaNo(arvore->raiz);
}

int quantidadeDaArvore(const ArvorePokedex *arvore) {
    if (arvore == NULL) {
        return 0;
    }
    return arvore->quantidade;
}
