/* Falas do caso */

#include "historia.h"

#include <string.h>

const char *historiaIntro(int pagina) {
    switch (pagina) {
    case 0:
        return "Alerta. Contenção interrompida.";
    case 1:
        return "Detetive, perdemos contato com o laboratório. Vá com Pikachu.";
    case 2:
        return "Última imagem do tanque antes do apagão.";
    case 3:
        return "A porta fecha atrás de vocês. As luzes apagam.";
    default:
        return NULL;
    }
}

const char *historiaDesfecho(int pagina) {
    switch (pagina) {
    case 0:
        return "Giovanni recua. O laboratório deixa de ser da Equipe Rocket.";
    case 1:
        return "O tanque está vazio. Uma voz ecoa.";
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
        return "Está escuro. Aperte L para ligar a lanterna. Procure o painel e use o choque do Pikachu para devolver a luz.";
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
        return "O Alakazam de teste observa. Há marcas na mesa. Capture-o e investigue a área com a habilidade psíquica.";
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
        return "O gerador voltou! Agora use Pikachu nos dois refletores do setor. Eles liberam a porta do pátio.";
    }
    if (strcmp(nome_sala, "Câmara fria") == 0) {
        return "As travas derreteram. Religue os refletores e enfrente o Vigia Rocket para alcançar o tanque.";
    }
    if (strcmp(nome_sala, "Oficina") == 0) {
        return "A dosagem funcionou e o fogo apagou. A Engenheira Rocket ainda protege o acesso à contenção.";
    }
    if (strcmp(nome_sala, "Arquivo") == 0) {
        return "A estante desliza. Giovanni está logo adiante.";
    }
    return NULL;
}
