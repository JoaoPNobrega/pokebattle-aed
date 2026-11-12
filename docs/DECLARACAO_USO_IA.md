# Declaração de uso de IA

Disciplina: Algoritmos e Estruturas de Dados — CESAR School, 2026.2.
Projeto: PokéBattle: O Caso do Laboratório.
Alunos: João e Leo. Cada um preenche as linhas do que usou. A defesa é individual: a pessoa que assina precisa saber explicar o código que ficou no projeto.

A ferramenta não substitui a autoria. O que entra no zip é o que vocês conseguem defender.

## Como preencher

Para cada conversa relevante, copie o bloco abaixo. Pode juntar vários prompts da mesma sessão se o assunto for o mesmo. Não precisa colar resposta inteira da IA: basta o que ela mudou no código e o que vocês revisaram.

---

### Sessão

- Aluno:
- Data:
- Ferramenta (Cursor, ChatGPT, Claude, Copilot, outra):
- Arquivos mexidos:

Prompt (cole o pedido, ou o trecho que define a tarefa):

```text
(cole aqui)
```

O que a IA entregou (em uma frase, sem colar o código todo):

O que eu revisei, apaguei ou reescrevi antes de aceitar:

Consigo explicar na defesa? (sim ou não; se não, não deixa no projeto):

---

## Sessões já ocorridas (completar e corrigir)

### Base do núcleo e do caso

- Aluno: João (revisar e assinar se for o caso)
- Data: outubro e novembro de 2026
- Ferramenta: Cursor, com agente de código
- Arquivos: o repositório em geral (`src/`, `include/`, `Makefile`, `README.md`, `AGENTS.md`, `docs/`)

Pedido resumido: primeiro um núcleo de batalha em C com as estruturas da disciplina; depois a mudança de conceito para o detetive no laboratório, com raylib, salas, puzzles, captura e chefão, mantendo a lógica separada da janela.

O que revisar antes da entrega: ler `docs/ARQUITETURA.md`, abrir `sala.c`, `caso.c` e `batalha.c`, e confirmar que cada um explica o próprio módulo. Apagar da declaração o que não for verdade.

### Bolsa, fuga, falas e créditos

- Aluno: João e Leo (os dois precisam ler e assinar se aceitarem)
- Data: 12/11/2026
- Ferramenta: Cursor, agente de código
- Arquivos: `src/caso.c`, `src/historia.c`, `src/ui_raylib.c`, `include/caso.h`, `include/historia.h`, README e `docs/`

Pedido resumido: diálogos pontuais da história, bolsa com Poção, Super Poção e Poké Bola gastando o turno, fuga de selvagem sem sorteio e recusa no chefe, tela de créditos e de game over. Manter `make teste` e `--simular` em vitória, sem warnings, e sem merge na main.

O que revisar antes de assinar: jogar a abertura, usar uma Poção, fugir de um selvagem, perder de propósito e ler os créditos. Apagar desta declaração qualquer frase que não for verdade.

## O que não entra como "a IA fez sozinha"

- Ideia do detetive, do Pikachu parceiro, dos puzzles por tipo e do Giovanni: decisão de João e Leo, comunicada à professora.
- A escolha das estruturas (lista dupla, fila, pilha, hash, AVL, insertion sort, merge sort) segue o roteiro da disciplina e o desenho combinado no `AGENTS.md`.
