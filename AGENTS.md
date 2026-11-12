# AGENTS.md — PokéBattle: O Caso do Laboratório

Leia este arquivo antes de alterar qualquer coisa. Ele vale para Cursor, Claude, Copilot e qualquer outra IA. Se uma instrução daqui conflitar com um palpite seu, siga este arquivo.

## O que é o jogo

Projeto da disciplina **Algoritmos e Estruturas de Dados** (CESAR School, 2026.2, Profa. Maira Lira), tema *Ode ao Cinema*. Entrega em **18/11/2026 07:59**: código-fonte em zip (sem executável), README de como rodar, e um vídeo. Depois há defesa oral individual: o código tem que ser explicável por quem o escreveu.

Nome: **PokéBattle: O Caso do Laboratório**.

Inspirado em *Pokémon: Mewtwo Contra-Ataca* e *Detetive Pikachu*, sem seguir nenhum dos dois à risca.

- Mini mistério no estilo escape room.
- O jogador é um **detetive** (não é o Pikachu). Começa só com o Pikachu, parceiro de investigação.
- Explora salas (o laboratório onde o Mewtwo foi criado, e o que vier depois), acha pistas e captura Pokémon.
- Cada Pokémon capturado tem uma **habilidade de tipo** que resolve um puzzle: elétrico liga máquina / acende sala, fogo derrete gelo ou acende, água apaga fogo, psíquico move objeto.
- Puzzle resolvido desbloqueia a próxima sala.
- No caminho há batalha por turnos (selvagem ou capanga). No fim, uma boss battle contra um vilão no estilo Giovanni (Equipe Rocket). Vencer zera o caso.
- Diálogo com personagem só em momento pontual. Poucos. Não é visual novel e não é quiz.

## Requisitos obrigatórios do roteiro

1. C (C99/C11). `gcc -Wall -Wextra` sem warning relevante. O Makefile usa também `-Wpedantic`.
2. Menu de opções interativo. O menu atual está na janela raylib: Novo jogo, Como jogar, Pokédex, Sair.
3. Pelo menos uma estrutura de sala na **lógica central**, não só como depósito. Foco em listas e árvores.
4. Pelo menos um algoritmo de ordenação feito à mão (insertion, bubble, quick, merge ou heap) que **não** seja só ranking. Aqui é o insertion sort da ordem do turno.
5. Pelo menos 6 funções operando sobre a estrutura. A lista de salas e a lista do time passam disso.
6. Não pode ser quiz.
7. Interface gráfica vale +0,5. A professora sugere raylib. **Não remover a janela.**

## Decisões já tomadas (não reabrir)

- Linguagem: só C. Sem C++, sem Python no jogo, sem framework de jogo além da raylib.
- Lógica e apresentação separadas. `src/ui_raylib.c` desenha e lê mouse/teclado. `src/caso.c`, `src/sala.c` e `src/batalha.c` decidem.
- Dá para jogar o caso sem janela: `./bin/pokebattle --simular`. `make teste` não abre janela.
- Estruturas feitas à mão. Proibido usar a STL (não é C), `glib`, e qualquer biblioteca pronta de lista, fila, pilha, hash ou árvore.
- Ordenação feita à mão em `src/ordenacao.c`. Não trocar por `qsort` o insertion sort do turno nem o merge sort da Pokédex.
- Nomes de função, tipo e variável em português. Comentário em português do Brasil.
- Todo arquivo de módulo explica no topo o que faz e por que a estrutura foi escolhida. Toda função pública diz o que faz, parâmetros, retorno e Big-O.
- Sem vazamento. Quem aloca, libera. A lista circular do turno não é dona do Pokémon.
- O dano tem um `/2` no fim de `calcularDano` para a luta durar mais de um golpe. A proporção entre os tipos não muda. Não apague esse divisor sem avisar no README.
- Sprites, se existirem, só com licença livre e crédito no README. Forma e texto bastam.
- Não commitar executável, `bin/`, `build/` nem a raylib baixada (`third_party/`).

## Onde cada estrutura trabalha

| Peça | Arquivo | Papel na regra |
| --- | --- | --- |
| Lista dupla das salas | `src/sala.c` | Cursor `atual` é onde o detetive está. `avancarSala` só anda se o puzzle foi resolvido e a próxima sala foi desbloqueada. `resolverPuzzleDaSala` exige a habilidade no time. |
| Lista dupla do time | `src/lista_time.c` | Pokémon capturados / parceiros. O `ativo` é quem luta. |
| Fila de lutas | `src/fila_treinadores.c` | Confrontos em ordem. O capanga sai primeiro. O chefão, quando entrar no jogo, entra no fim da fila. |
| Pilha de histórico | `src/pilha_log.c` | Caminho e lances. `mostrarHistorico` inverte com uma segunda pilha. |
| Lista circular do turno | `src/lista_turno.c` | Quem age neste turno. `fim` aponta de volta para o `inicio`. |
| Insertion sort | `ordenarTurnoPorInsercao` em `src/ordenacao.c` | Prioridade do golpe, depois velocidade. Chamado por `definirOrdemTurno` todo turno. Também reordena o time (`ordenarTime`). Não é ranking. |
| Merge sort | `ordenarPokedex` | Reorganiza uma cópia tirada da hash (que não tem ordem) por número, nome ou velocidade. |
| Hash | `src/hash_pokedex.c` | Busca da espécie pelo nome. |
| AVL | `src/arvore_pokedex.c` | Busca pelo número e listagem em ordem (percurso). |
| Matriz de tipos | `src/tipos.c` | Efetividade usada em `calcularDano`. |

## Mapa das pastas

```text
include/     cabeçalhos
src/         implementação
  ui_raylib.c    única chamada à raylib
  caso.c         estado do mistério (salas + luta), sem desenhar
  sala.c         lista dupla e habilidade de puzzle
  batalha.c      dano, ordem do turno, um turno
  simulacao.c    --simular, sem janela
src/ui_terminal.c   legado da etapa da arena. Não é o jogo atual. Não reativar como tela principal.
tests/       testes das estruturas (não abrem janela)
scripts/baixar_raylib.sh
docs/        material da defesa. ARQUITETURA.md ainda mistura o texto antigo: reescrever, não apagar o requisito.
third_party/ raylib compilada localmente. Não versionar.
```

## Como compilar

Linux, se não houver `pkg-config --libs raylib`:

```bash
sudo apt install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
./scripts/baixar_raylib.sh
make
./bin/pokebattle
make teste
```

macOS: `brew install raylib` e `make`. Windows (MSYS2): `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib mingw-w64-x86_64-pkg-config` e `mingw32-make`. Detalhe no README.

## O que já está jogável

Oito salas, nesta ordem: Laboratório (choque), Pátio (Charmander capturável), Câmara fria (chama), Tanque (Squirtle capturável), Oficina (jato), Jaula (Alakazam de teste capturável), Arquivo (mente) e Sala do chefe (Giovanni, Meowth e Geodude). Vencer o Giovanni encerra o caso.

- Menu, ajuda, Pokédex, exploração, captura e batalha com barra de HP estão em `src/ui_raylib.c`.
- `--simular` joga esse percurso sozinho e só imprime `RESULTADO: VITORIA` se o chefe cair.
- `docs/ARQUITETURA.md` descreve este fluxo. `docs/DECLARACAO_USO_IA.md` é o modelo para os alunos preencherem.

## O que ainda falta

Diálogos pontuais além da fala de uma linha, polimento visual (tarefa do Leo, só em `ui_raylib.c`), declaração de IA preenchida, zip sem executável e o vídeo da entrega. A divisão está no README. Não reimplemente salas, captura nem o chefão: eles já estão na lógica.

## O que NÃO fazer

- Não transformar o jogo em quiz, formulário ou prova.
- Não trocar de linguagem e não tirar a interface gráfica.
- Não substituir lista, fila, pilha, hash ou AVL por biblioteca pronta.
- Não tirar o insertion sort da ordem do turno nem deixar a ordenação só para um ranking.
- Não colocar `printf` de regra dentro de `ui_raylib.c` como se fosse a lógica, e não colocar `DrawText` dentro de `caso.c` / `batalha.c` / `sala.c`.
- Não apagar o modo `--simular` nem o `make teste`.
- Não commitar binário, zip com executável, ou a pasta `third_party/`.
- Ao mudar regra ou pasta, atualize este arquivo, o README e (quando ele for reescrito) `docs/ARQUITETURA.md`.

## Convenção de comentário

Topo do `.c` e do `.h`: o módulo e, se houver estrutura, por que ela foi escolhida. Em cada função pública: o que faz, parâmetros, retorno, Big-O. Comente o trecho que mexe em ponteiro (remover da lista dupla, ciclo da lista do turno, rotação da AVL, inversão da pilha). Não comente o óbvio (`i++`).
