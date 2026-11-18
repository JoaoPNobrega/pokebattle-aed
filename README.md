# PokéBattle: O Caso do Laboratório

João Pedro Araújo Nóbrega e Leonardo Gutzeit Borgmann Matos.
Algoritmos e Estruturas de Dados, CESAR School, 2026.2.

## Como executar

Precisa de gcc com C11 e da raylib 5. Os comandos abaixo são na pasta do projeto. Se rodar de outro lugar, o jogo não acha `assets/`.

O `make` usa a raylib do `pkg-config` quando ela existe. Se não existir, baixa a 5.5 para `third_party/` e compila a biblioteca estática. Essa pasta não entra no git.

### Linux

```bash
sudo apt install build-essential curl libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libxfixes-dev
make
./bin/pokebattle
```

### macOS

Precisa do Xcode Command Line Tools (`xcode-select --install`).

```bash
brew install raylib pkgconf
make
./bin/pokebattle
```

Se o `pkg-config` não achar a raylib, rode `./scripts/baixar_raylib.sh` e depois o `make` de novo.

### Windows (MSYS2, terminal UCRT64)

```bash
pacman -S make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib mingw-w64-ucrt-x86_64-pkgconf
make
./bin/pokebattle.exe
```

É o mesmo conjunto que o workflow de empacotar usa. O `jogar.bat` abre `bin\pokebattle.exe` depois da compilação.

O jogo abre em tela cheia. `./bin/pokebattle --janela` (no Windows, `bin\pokebattle.exe --janela`) começa em janela. F11 ou Alt+Enter troca durante a partida.

### Sem abrir a janela

```bash
make teste
```

Compila, roda os testes das estruturas e `./bin/pokebattle --simular`. A simulação joga o caso sozinho, do laboratório até o Giovanni, e termina com `RESULTADO: VITORIA`. No Windows o executável é `bin\pokebattle.exe --simular`.

Quem não for compilar pode pegar o ZIP na aba Releases: no Windows, extrair e abrir `PokeBattle.exe`; no Mac, extrair e abrir `PokéBattle.app`.

## O jogo

Você é o detetive. O Pikachu é o parceiro. A história passa pelo laboratório em que o Mewtwo foi criado, com cara de *Mewtwo Contra-Ataca* e de *Detetive Pikachu*, sem copiar o roteiro dos dois.

São oito salas, nesta ordem: Laboratório, Pátio, Câmara fria, Tanque, Oficina, Jaula, Arquivo e Sala do chefe. Em cada uma tem puzzle, captura ou guarda. O Pikachu liga o circuito, o Charmander derrete o gelo, o Squirtle apaga o fogo e o Alakazam segue os rastros e empurra as estantes. No Arquivo dá para abrir o Santuário, uma sala extra com três altares e a Master Ball. A Pokédex (P) registra quem já apareceu. O PC, nos computadores do mapa, guarda Pokémon fora do time. O Giovanni entra com Meowth, Geodude e Mewtwo.

A tela é a raylib, em `src/ui_raylib.c`. `src/ui_terminal.c` é o menu de texto da etapa anterior e não é o que o executável abre.

## Controles

Setas ou WASD andam e escolhem. Z confirma (Enter e espaço também). X volta. Esc pausa.

| Tecla | O que faz |
| --- | --- |
| Z | Interage, gira o fio, ataca, avança o texto |
| L | Liga e desliga a lanterna |
| N | Caderno, com o histórico da pilha |
| P | Pokédex |
| M | Mapa das salas |
| T | Time |
| V | Volta para a sala anterior |
| E | Resfria o gelo |
| U | Desfaz um empurrão de estante |
| R | Reinicia o puzzle, ou reabre o documento |

Na batalha o menu é lutar, bolsa, trocar e fugir. Item gasta o turno. Do chefe não dá para fugir. No PC, Tab troca entre equipe e reserva, Z move o Pokémon e L escolhe o líder.

## Estruturas de dados

| Estrutura | Onde |
| --- | --- |
| Lista dupla das salas | `src/sala.c` |
| Lista dupla do time e do PC | `src/lista_time.c` |
| Fila de treinadores | `src/fila_treinadores.c` (`casoIniciarBatalha` tira o próximo) |
| Pilha do histórico | `src/pilha_log.c` |
| Lista circular do turno | `src/lista_turno.c` |
| Insertion sort (quem age primeiro) | `ordenarTurnoPorInsercao` em `src/ordenacao.c` |
| Merge sort da Pokédex | `ordenarPokedex` em `src/ordenacao.c` |
| Hash por nome | `src/hash_pokedex.c` |
| AVL por número | `src/arvore_pokedex.c` |
| Matriz de tipos | `src/tipos.c`, usada em `calcularDano` |
| Fila do circuito (9 posições, sem `malloc`) | `circuitoAlcancado` em `src/desafio.c` |

O estado do caso fica em `src/caso.c`. A regra da luta fica em `src/batalha.c`.

## Créditos dos sprites

Uso acadêmico. O jogo não baixa nada enquanto roda.

| Arquivo | Origem |
| --- | --- |
| `assets/pokemon/` | Sprites e `dados.txt` da [PokéAPI](https://pokeapi.co), arte Nintendo / Game Freak (FireRed/LeafGreen, com o sprite padrão se faltar) |
| `assets/pokemon/145_*.png` | Zapdos em pixel art do projeto, se o arquivo da API não estiver |
| `assets/tiles/frlg_lab.png` | Laboratório do Carvalho, decompilação [pret/pokefirered](https://github.com/pret/pokefirered) |
| `assets/tiles/kenney_indoor.png` | Reserva de chão, CC0, [Kenney](https://kenney.nl). Licença em `assets/tiles/LICENSE-kenney-indoor.txt` |
| `assets/personagens/detetive.png`, `giovanni_overworld.png`, `detetive_costas.png` | FireRed, [pret/pokefirered](https://github.com/pret/pokefirered) |
| `assets/personagens/giovanni.png` | Giovanni da geração III, [Pokémon Showdown](https://play.pokemonshowdown.com/sprites/trainers/giovanni-gen3.png) |
| `assets/personagens/pikachu_overworld.png` | Seguidor de HeartGold/SoulSilver, [pokeamice.com/hgss-pokemon-walk](https://pokeamice.com/hgss-pokemon-walk/) |
| `assets/personagens/pokebola.png` e `assets/ui/` | Pixel art deste projeto |
| `assets/fontes/PressStart2P-Regular.ttf` | Press Start 2P, Cody Boisclair, SIL OFL 1.1 (`assets/fontes/OFL-PressStart2P.txt`) |

O som de clique e de acerto é uma onda gerada no código, sem arquivo. A lista do que a tela usou como referência está em `docs/REFERENCIAS_VISUAIS.md`.
