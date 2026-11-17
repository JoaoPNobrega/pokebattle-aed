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
| Mapa | Setas ou WASD, uma célula por vez. Z interage (painel, porta, itens, Pokémon). HUD minimalista com ícones rápidos [P], [M], [N], [T] e chave secreta. N abre o caderno. L liga/desliga a lanterna |
| Itens no Mapa | Cápsulas com Pokébolas flutuantes ('*') espalhadas pelos setores guardam Rare Candies (que aumentam o nível instantaneamente até 60 e curam o parceiro) e suprimentos (Super Poções, Poké Bolas) |
| Pokédex Dinâmica | Tecla P abre a Pokédex LCD. Pokémon não encontrados aparecem como silhuetas misteriosas com dados ocultos (???). Conforme você explora e batalha, novos registros são desbloqueados |
| Scanner Holográfico | Antes de cada duelo, o visor Lock-On mira no Pokémon adversário com retículos neon cibernéticos, animação de varredura e leitura de atributos em tempo real |
| Minimapa | Tecla M a qualquer instante abre o mapa tático da instalação, status dos 8 setores, passagem secreta e radar local com detetive e Pikachu |
| Documento | Após a busca na Jaula, abre em destaque com duas páginas completas. Z/seta direita avança; seta esquerda volta; X fecha. R permite reler em qualquer sala ou no caderno |
| Puzzles | Setas/WASD escolhem ou movem. Z gira fio/aquece/examina/opera válvula. E resfria o gelo. U desfaz uma estante. R reinicia o desafio; X fecha |
| Santuário Secreto | No Arquivo, examine o cofre lateral ('k') para resgatar a Chave Secreta. Abra a passagem lateral leste ('S') para o Santuário e ative os 3 altares elementais (Fogo/Charmander, Psíquico/Alakazam, Choque/Pikachu) para liberar a Master Ball |
| Treino | Matos do Pátio/Tanque e escombros da Oficina/Jaula. Depois da captura obrigatória, ande quatro passos na área ou use Z nela para encontrar selvagens. Vitória rende 30 XP para cada parceiro; 60 XP sobem um nível (limite 60) |
| Time e descanso | T escolhe o Pokémon ativo e mostra HP/XP. Z diante de bancos recupera HP e PP. V retorna à sala anterior |
| Batalha | Comandos com ícones dedicados (LUTAR, BOLSA, POKÉMON, FUGIR). Sprites com respiração viva ("squash & stretch"), sombras realistas e alinhamento em plataformas ovais. Oponentes Shiny possuem partículas reluzentes |
| Texto | Z avança a digitação e a próxima página |
| Qualquer tela | Esc volta ao menu. No caderno, minimapa ou Pokédex, Esc, P, M ou X fecha o modal |

A bolsa começa com 2 Poções (20 de HP), 1 Super Poção (50) e 3 Poké Bolas. Encontrar cápsulas com Rare Candy no mapa permite subir de nível de forma instantânea. Usar um item gasta o turno e o oponente age. Na batalha, a Poké Bola exige no máximo metade do HP. Depois, a captura exige cinco comandos de lançamento/contenção, também após vencer um selvagem. Siga o PNG destacado; erro de tecla ou prazo reinicia a contenção, sem perder o parceiro obrigatório. A Master Ball do santuário secreto captura Mew/Mewtwo selvagens sem falha. Fugir do selvagem depende da velocidade vezes o número da tentativa. Do chefe, a frase é "Não dá pra fugir de um chefe!".

O Pikachu anda atrás do detetive. A porta exige os objetivos do setor: puzzle, dois refletores nos ambientes escuros, captura e guarda quando houver. A barra de HP desce sozinha e fica amarela abaixo de 50% e vermelha abaixo de 20%. O acerto pisca o sprite e treme. O som é uma onda gerada no código; sem placa de áudio, a luta segue muda.

Quem age primeiro continua sendo prioridade do golpe e velocidade (insertion sort). Ataque Rápido fura a fila.

## Créditos e licenças

| Arquivo | O que é | Licença |
| --- | --- | --- |
| `assets/fontes/PressStart2P-Regular.ttf` | Fonte pixel Press Start 2P, Cody Boisclair | SIL OFL 1.1 (`assets/fontes/OFL-PressStart2P.txt`). O nome é reservado. [Google Fonts](https://fonts.google.com/specimen/Press+Start+2P) |
| `assets/pokemon/` | Sprites de frente, costas e ícone, mais `dados.txt` (nome, tipos e stats base) | Arte da Nintendo / Game Freak, obtida pela [PokéAPI](https://pokeapi.co) (`sprites.versions.generation-iii.firered-leafgreen`, com fallback para o sprite padrão). Uso acadêmico. O jogo não baixa nada ao rodar |
| `assets/pokemon/145_*.png` | Zapdos em pixel art local, usado quando o asset da PokéAPI não está disponível | Arte própria do projeto; gerador opcional em `tools/gerar_zapdos.ps1` |
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

O laboratório começa escuro. L alterna uma lanterna que acompanha o detetive e a direção em que olha; o painel resolvido pelo Pikachu restaura a iluminação normal. Na Jaula, capture Alakazam e use Z diante da mesa para examinar a área com a habilidade psíquica. Um fundo falso revela um documento com dicas sobre o Arquivo e Geodude; a pista fica no caderno (N), e o Arquivo só destrava após a busca. Não há inventário paralelo: o documento usa o estado do puzzle e a pilha de histórico.

Capturas opcionais preservam vagas para os parceiros dos puzzles. Espécies já capturadas continuam disponíveis para treinar, sem duplicar o time. Os encontros opcionais não consomem a fila dos confrontos obrigatórios. No Arquivo, a porta azul marcada no mapa leva ao Santuário secreto, uma área própria com mata e três selos; resolva-os na ordem do mural para receber a Master Ball. Só no santuário podem aparecer Mew (1/64), Mewtwo (1/256) ou uma das aves lendárias (1/64 no total). Qualquer encontro de treino pode ser shiny (1/128). O sorteio usa semente de relógio e não afeta a fila do caso.


## Exploração e progressão dos setores

Pikachu começa no nível 26 com dois golpes, sem Trovão disponível de início. O Vigia Rocket da Câmara fria e a Engenheira Rocket da Oficina desbloqueiam os golpes seguintes, restauram o time e dão Poções/Poké Bolas. Há seis confrontos obrigatórios na fila até Giovanni, além dos treinos opcionais.

Os desafios agora são espaciais e diferentes por habilidade, no estado `Desafio` de cada nó de sala (`include/desafio.h`, `src/desafio.c`). Pikachu gira uma rede 3×3 com percurso de conexões; Charmander derrete quatro travas de três camadas, gerenciando calor e evitando tubos; Squirtle opera tanques de capacidade 5 e 3 até obter A=4/B=3 antes de liberar a mangueira; Alakazam segue três ecos numa busca 5×4 na Jaula e empurra estantes numa grade 6×6 no Arquivo, com U para desfazer e R para reiniciar. `casoComandarPuzzle` valida parceiro e aplica ações; a janela só lê entradas e desenha. As barras de acertar no verde foram removidas. Não transformar esses desafios em quiz.

O circuito restaura o gerador; dez refletores, dois por setor escuro, precisam ser religados no mapa com Pikachu. L alterna a lanterna. A caixa de seleção marca o objeto à frente, mostra a tecla Z em PNG e informa o objetivo restante. No topo, veja o setor e Luz 0/2, 1/2 ou 2/2. A porta só libera após completar esses objetivos.

A captura não é automática: Z lança, siga as três direções e Z sela a bola. Os prazos diminuem ao longo da sequência. Um erro reinicia a contenção; você pode tentar novamente. Capturas opcionais continuam reservando vagas para os parceiros do caso.

Os PNGs de teclas e guardas em `assets/ui/` são arte própria do projeto. O gerador opcional é `tools/gerar_interface.ps1`; os PNGs já acompanham o código e não precisam ser gerados para jogar. Nenhum asset de Ben 10 foi incorporado.

Antes do duelo, a ficha da Pokédex permite consultar os membros do treinador e os dados de combate. Giovanni agora leva Meowth, Geodude e Mewtwo. A Pokédex junta as 23 espécies embutidas em `src/dados_pokemon.c` às linhas extras de `assets/pokemon/dados.txt`; quando os assets contêm a primeira geração completa, os encontros de treino sorteiam entre as 151 espécies. O script `tools/baixar_assets.py` foi ampliado para gerar esse catálogo completo. Neste checkout, `dados.txt` ainda tem 20 linhas de espécie, então o jogo atual carrega 23 até os assets completos serem baixados. Os encontros raros usam Mew, Mewtwo e as três aves lendárias, todas dentro da primeira geração.
