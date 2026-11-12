# PokéBattle: O Caso do Laboratório

Jogo em C para a disciplina **Algoritmos e Estruturas de Dados** (CESAR School, 2026.2, Profa. Maira Lira), tema *Ode ao Cinema*.

O jogador é um detetive. O Pikachu é o parceiro, não o protagonista. A história se passa no laboratório onde o Mewtwo foi criado e bebe de *Mewtwo Contra-Ataca* e de *Detetive Pikachu*, sem seguir nenhum dos dois à risca. Há salas com puzzle, batalha por turnos e, no plano do jogo, um chefão no estilo Giovanni. Não é um quiz.

O caso jogável atravessa oito salas: o painel elétrico, um Charmander selvagem, a câmara fria, um Squirtle, a oficina em chamas, um Alakazam de teste, a estante do arquivo e a luta contra o Giovanni. Capturar o selvagem é o que abre a sala seguinte. O que ainda falta para a entrega está na lista de TODO.

Quem for alterar o código com IA deve ler o `AGENTS.md` antes. Ele também está apontado em `CLAUDE.md`, `.cursor/rules/projeto.mdc` e `.github/copilot-instructions.md`.

## Como compilar e rodar

Precisa de gcc (ou Clang) com C11 e da raylib 5. Não há executável no repositório.

### Linux

Pacotes de compilação da raylib (X11 e OpenGL):

```bash
sudo apt install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
```

Se a distribuição já tiver a raylib no `pkg-config` (`pkg-config --libs raylib`), basta:

```bash
make
./bin/pokebattle
```

Se não tiver, o Makefile chama o script, que baixa a raylib 5.5 para `third_party/` (essa pasta não entra no git) e compila a biblioteca estática:

```bash
./scripts/baixar_raylib.sh
make
./bin/pokebattle
```

Teste sem abrir janela:

```bash
make teste
```

Isso roda os testes das estruturas e `./bin/pokebattle --simular`, que resolve o puzzle, entra no corredor, vence o capanga e imprime `RESULTADO: VITORIA`.

### macOS

```bash
brew install raylib
make
./bin/pokebattle
```

O `gcc` do Mac costuma ser o Clang. As flags `-Wall -Wextra -Wpedantic` são as mesmas. Se o `pkg-config` não achar a raylib, use `./scripts/baixar_raylib.sh` (precisa do Xcode command line tools).

### Windows (MSYS2 / MinGW)

No terminal MSYS2 UCRT64 ou MINGW64:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib mingw-w64-x86_64-pkg-config make
mingw32-make
bin/pokebattle.exe
```

O `pkg-config` deve enxergar a raylib. Sem ele, o script `scripts/baixar_raylib.sh` também funciona dentro do MSYS2, desde que os headers de OpenGL estejam instalados.

Rode o programa a partir da pasta do projeto, para achar `assets/fontes/`. Sem a fonte, a janela cai na fonte padrão da raylib.

## Controles

A cena é desenhada em 384×216 e ampliada em pixels inteiros.

| Onde | Mouse | Teclado |
| --- | --- | --- |
| Menu | Novo jogo, Ajuda, Pokédex, Sair | 1, 2, 3, 4 |
| Sala | Habilidade, Avançar, Voltar, Capturar, Lutar | E, D, A, C, B |
| Caderno de pistas | — | N abre e fecha |
| Batalha | Grade 2×2 de golpes | 1 a 4. A barra desce sozinha e fica amarela abaixo de 50% e vermelha abaixo de 20% |
| Qualquer tela, fora do menu | — | Esc volta ao menu. No caderno, Esc só fecha o caderno |

Quem age primeiro continua sendo prioridade do golpe e velocidade (insertion sort). Ataque Rápido fura a fila.

## Créditos e licenças

| Arquivo | O que é | Licença |
| --- | --- | --- |
| `assets/fontes/PressStart2P-Regular.ttf` | Fonte pixel Press Start 2P, Cody Boisclair | SIL OFL 1.1 (`assets/fontes/OFL-PressStart2P.txt`). O nome é reservado. [Google Fonts](https://fonts.google.com/specimen/Press+Start+2P) |
| Sprites dos Pokémon em `src/ui_raylib.c` | Grades de pixel feitas no código | Originais deste projeto. Não são sprites oficiais nem da PokéAPI |
| Cliques e acerto | Onda quadrada gerada na hora | Sem arquivo externo |

A lista do que foi olhado e do que entrou na tela está em `docs/REFERENCIAS_VISUAIS.md`. Não há arte do Kenney empacotada; a licença CC0 dele foi só o critério para o que poderia entrar.

## Onde está cada requisito

| Requisito | Onde |
| --- | --- |
| C11, sem warning em `-Wall -Wextra` | `Makefile` |
| Menu interativo | `executarInterfaceRaylib` em `src/ui_raylib.c` |
| Lista dupla na lógica | `src/sala.c` (salas: avançar, voltar, desbloquear) e `src/lista_time.c` (time e parceiro ativo) |
| Fila na lógica | `casoIniciarBatalha` tira o próximo oponente com `proximoTreinador` |
| Pilha na lógica | Cada lance vai para `empilharLog`. O topo aparece na sala |
| Lista circular + insertion sort no turno | `definirOrdemTurno` e `casoExecutarGolpe` |
| Hash e AVL | Pokédex: nome na hash, número e listagem em ordem na AVL |
| Merge sort | `ordenarPokedex` (menu de organizar ainda é da etapa anterior; a função continua no código) |
| Matriz de tipos | `src/tipos.c`, usada por `calcularDano` |
| Seis ou mais operações na estrutura | Em `sala.c`: criar, liberar, adicionar, avançar, voltar, resolver, tamanho |
| Não é quiz | Exploração e batalha |
| Interface gráfica | raylib, só em `src/ui_raylib.c` |

## O que falta

O caso até o Giovanni já roda na janela e no `--simular`. Ainda não é a entrega final.

### João

- Um ou dois diálogos curtos, só em momento pontual (a fala do Giovanni hoje é uma linha na luta). Se virar árvore de falas, que seja pequena.
- Preencher `docs/DECLARACAO_USO_IA.md` com os prompts reais de cada um. O arquivo já tem o modelo.
- Zip do código, sem executável, e o vídeo da entrega (18/11/2026, 07:59).
- Balancear níveis se uma luta ficar longa ou curta demais na mão de vocês.

### Leo

A base visual já está em `src/ui_raylib.c` (pixel, barra, typewriter, caderno, chefe). Se for mexer:

- Não mude a resolução virtual nem troque o filtro point por bilinear.
- Não desenhe fora de `ui_raylib.c` e não mova regra para dentro do desenho.
- Sprite novo: ou grade no código, ou PNG livre em `assets/` com licença neste README e um desenho reserva se o arquivo faltar.
- Conferir a build no Windows (MSYS2) e no macOS se tiver a máquina.

## Legado

`src/ui_terminal.c` é o menu de texto da etapa anterior (arena até o Mewtwo). Não é a tela do jogo atual. A regra de dano, o insertion sort, a lista circular, a hash, a AVL e a matriz de tipos foram reaproveitados.
