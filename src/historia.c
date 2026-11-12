/*
 * historia.c
 * ----------
 * Falas curtas do mistério. Não seguem Mewtwo Contra-Ataca nem
 * Detetive Pikachu à risca: o laboratório e a Equipe Rocket são
 * o cenário, e o jogador é o detetive.
 *
 * Nada aqui desenha. A caixa de texto mora na interface.
 */

#include "historia.h"

#include <string.h>

const char *historiaIntro(int pagina) {
    switch (pagina) {
    case 0:
        return "O caso chegou de madrugada. Você é o detetive. O Pikachu é o parceiro.";
    case 1:
        return "Este laboratório criou o Mewtwo. As luzes morreram e as portas trancaram.";
    case 2:
        return "Não é o filme. É um escape: cada pista pede choque, chama, jato ou mente.";
    case 3:
        return "A Equipe Rocket ainda ronda o prédio. Vá até o fim do corredor.";
    default:
        return NULL;
    }
}

const char *historiaDesfecho(int pagina) {
    switch (pagina) {
    case 0:
        return "Giovanni recua. O laboratório deixa de ser da Equipe Rocket.";
    case 1:
        return "O Mewtwo não está nos tanques. Alguém o tirou daqui antes de você.";
    case 2:
        return "O Pikachu encosta na sua perna. O caso fecha. A história, não.";
    default:
        return NULL;
    }
}

const char *historiaAoEntrar(const char *nome_sala) {
    if (nome_sala == NULL) {
        return NULL;
    }
    if (strcmp(nome_sala, "Laboratório") == 0) {
        return "O painel está morto. Sem um choque, esta sala não devolve a luz.";
    }
    if (strcmp(nome_sala, "Pátio") == 0) {
        return "O Charmander não foge. Ele viu quem apagou o laboratório.";
    }
    if (strcmp(nome_sala, "Câmara fria") == 0) {
        return "Uma placa de gelo sela a próxima porta. Só uma chama cede.";
    }
    if (strcmp(nome_sala, "Tanque") == 0) {
        return "O Squirtle está preso no tanque. Água vai servir mais adiante.";
    }
    if (strcmp(nome_sala, "Oficina") == 0) {
        return "O incêndio não parece acidente. Água apaga. Mais fogo, não.";
    }
    if (strcmp(nome_sala, "Jaula") == 0) {
        return "O Alakazam de teste observa. A mente dele move o que a mão não move.";
    }
    if (strcmp(nome_sala, "Arquivo") == 0) {
        return "A estante cobre a sala do chefe. Só uma mente tira isso da frente.";
    }
    if (strcmp(nome_sala, "Sala do chefe") == 0) {
        return "Giovanni espera. Equipe Rocket. Este laboratório não sai sem luta.";
    }
    return NULL;
}

const char *historiaAoResolver(const char *nome_sala) {
    if (nome_sala == NULL) {
        return NULL;
    }
    if (strcmp(nome_sala, "Laboratório") == 0) {
        return "As luzes acendem. O Pikachu aponta a porta do pátio.";
    }
    if (strcmp(nome_sala, "Câmara fria") == 0) {
        return "O gelo derrete. Do outro lado, dá para ouvir o tanque.";
    }
    if (strcmp(nome_sala, "Oficina") == 0) {
        return "O fogo apaga. A fumaça sobe e o caminho da jaula abre.";
    }
    if (strcmp(nome_sala, "Arquivo") == 0) {
        return "A estante desliza. Giovanni está logo adiante.";
    }
    return NULL;
}
