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

Rode o programa a partir da pasta do projeto, para achar `assets/`. Sem a fonte, a janela cai na fonte padrão da raylib. Os sprites já estão no repositório; `make assets` só baixa de novo.

```bash
make assets   # opcional: refaz assets/pokemon/ pela PokéAPI
```

## Controles

A cena é desenhada em 240×160 (o tamanho do GBA) e ampliada em pixels inteiros, com filtro de ponto.

| Onde | Teclado |
| --- | --- |
| Menu | Setas e Z, ou 1, 2, 3, 4. Mouse também serve |
| Mapa | Setas ou WASD, uma célula por vez. Z interage (painel, porta, Pokémon). N abre o caderno |
| Batalha | Setas escolhem LUTAR, BOLSA, POKÉMON ou FUGIR. Z confirma, X volta. A grade de golpes mostra tipo e PP |
| Texto | Z avança a digitação e a próxima página |
| Qualquer tela | Esc volta ao menu. No caderno, Esc ou Z fecha |

A bolsa começa com 2 Poções (20 de HP), 1 Super Poção (50) e 3 Poké Bolas. Usar um item gasta o turno e o oponente age. A Poké Bola só pega selvagem com no máximo metade do HP, sem sorteio. Fugir do selvagem depende da velocidade vezes o número da tentativa. Do chefe, a frase é "Não dá pra fugir de um chefe!".

O Pikachu anda atrás do detetive. A porta só abre depois do puzzle ou da luta. A barra de HP desce sozinha e fica amarela abaixo de 50% e vermelha abaixo de 20%. O acerto pisca o sprite e treme. O som é uma onda gerada no código; sem placa de áudio, a luta segue muda.

Quem age primeiro continua sendo prioridade do golpe e velocidade (insertion sort). Ataque Rápido fura a fila.

## Créditos e licenças

| Arquivo | O que é | Licença |
| --- | --- | --- |
| `assets/fontes/PressStart2P-Regular.ttf` | Fonte pixel Press Start 2P, Cody Boisclair | SIL OFL 1.1 (`assets/fontes/OFL-PressStart2P.txt`). O nome é reservado. [Google Fonts](https://fonts.google.com/specimen/Press+Start+2P) |
| `assets/pokemon/` | Sprites de frente, costas e ícone, mais `dados.txt` (nome, tipos e stats base) | Arte da Nintendo / Game Freak, obtida pela [PokéAPI](https://pokeapi.co) (`sprites.versions.generation-iii.firered-leafgreen`, com fallback para o sprite padrão). Uso acadêmico. O jogo não baixa nada ao rodar |
| `assets/tiles/frlg_lab.png` | Chão, parede, estante, máquina e porta do laboratório da FireRed | Nintendo / Game Freak, via a decompilação [pret/pokefirered](https://github.com/pret/pokefirered) (`data/tilesets/primary/building` e `data/tilesets/secondary/lab`, mapa do laboratório do Carvalho). Uso acadêmico |
| `assets/tiles/kenney_indoor.png` | Tileset de interior 16×16, reserva se o da FireRed faltar | CC0, Kenney (Roguelike Indoor). Texto em `assets/tiles/LICENSE-kenney-indoor.txt`. [kenney.nl](https://kenney.nl) |
| `assets/personagens/detetive.png` e `giovanni_overworld.png` | Gentleman e Giovanni no mapa, 4 direções e 3 passos | Nintendo / Game Freak, [pret/pokefirered](https://github.com/pret/pokefirered) `graphics/object_events/pics/people/`. Uso acadêmico |
| `assets/personagens/detetive_costas.png` | Red de costas na introdução da luta | O mesmo repositório, `graphics/trainers/back_pics/red_back_pic.png` |
| `assets/personagens/giovanni.png` | Giovanni treinador, geração III | [Pokémon Showdown](https://play.pokemonshowdown.com/sprites/trainers/giovanni-gen3.png). Uso acadêmico |
| `assets/personagens/pikachu_overworld.png` | Pikachu seguidor, 4 direções | HeartGold/SoulSilver, GIFs em [pokeamice.com/hgss-pokemon-walk](https://pokeamice.com/hgss-pokemon-walk/). Uso acadêmico |
| `assets/personagens/pokebola.png` | Poké Bola 16×16 | Pixel art deste projeto |
| Cliques e acerto | Onda quadrada gerada na hora | Sem arquivo externo |

Os stats oficiais estão em `assets/pokemon/dados.txt`. A batalha continua com os números de `src/dados_pokemon.c`, afinados para a luta durar mais de um golpe. A lista do que a tela aproveitou está em `docs/REFERENCIAS_VISUAIS.md`.

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

- Assinar `docs/DECLARACAO_USO_IA.md` com o que cada um realmente usou e o que reescreveu. O modelo e um rascunho da sessão de novembro já estão no arquivo.
- Zip do código, sem executável, e o vídeo da entrega (18/11/2026, 07:59).
- Balancear níveis se uma luta ficar longa ou curta demais na mão de vocês.
- Se o sobrenome do Leo for entrar nos créditos, trocar a linha "Leo" em `desenharCreditos`.

### Leo

A tela está em `src/ui_raylib.c`: mapa de 16×16, batalha no layout da FireRed, sprites locais. Se for mexer:

- Não mude a resolução virtual (240×160) nem troque o filtro point por bilinear.
- Não desenhe fora de `ui_raylib.c` e não mova regra para dentro do desenho.
- Não ponha HTTP no binário. Sprite novo entra em `assets/`, com licença neste README, e a tela precisa de um retângulo reserva se o arquivo faltar.
- Conferir a build no Windows (MSYS2) e no macOS se tiver a máquina.

## Legado

`src/ui_terminal.c` é o menu de texto da etapa anterior (arena até o Mewtwo). Não é a tela do jogo atual. A regra de dano, o insertion sort, a lista circular, a hash, a AVL e a matriz de tipos foram reaproveitados.
