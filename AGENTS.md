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
2. Menu de opções interativo. O menu está na pasta de investigação em raylib: Iniciar caso, Como jogar, Pokédex, Ajustes, Créditos e Sair.
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
- Os sprites dos Pokémon vêm da PokéAPI e ficam em `assets/pokemon/`. As pessoas e o laboratório vêm dos gráficos da FireRed (pret/pokefirered), o Giovanni da batalha do Pokémon Showdown e o Pikachu do mapa do HeartGold. O C não faz HTTP. Créditos no README. A Poké Bola é pixel art do projeto.
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
tools/baixar_assets.py   baixa sprites e dados.txt (make assets)
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

Declaração de IA assinada por quem vai defender, zip sem executável e o vídeo até 18/11/2026. Diálogos, bolsa, fuga, créditos e game over já estão no jogo. O mapa e a batalha no estilo GBA ficam em `ui_raylib.c`. Não reimplemente salas, captura nem o chefão. A bolsa e a fuga moram em `caso.c`; a janela só mostra.

## O que NÃO fazer

- Não transformar o jogo em quiz, formulário ou prova.
- Não trocar de linguagem e não tirar a interface gráfica.
- Não substituir lista, fila, pilha, hash ou AVL por biblioteca pronta.
- Não tirar o insertion sort da ordem do turno nem deixar a ordenação só para um ranking.
- Não colocar `printf` de regra dentro de `ui_raylib.c` como se fosse a lógica, e não colocar `DrawText` dentro de `caso.c` / `batalha.c` / `sala.c`.
- Não apagar o modo `--simular` nem o `make teste`.
- Não commitar binário, zip com executável, ou a pasta `third_party/`.
- Ao mudar regra ou pasta, atualize este arquivo, o README e (quando ele for reescrito) `docs/ARQUITETURA.md`.

## Convenção visual (não quebrar)

A janela desenha em 240×160 e amplia com a maior escala inteira que cabe no monitor, centralizada com margens pretas e `TEXTURE_FILTER_POINT`. Abre em tela cheia sem alterar a resolução do monitor; F11 ou Alt+Enter alterna janela/tela cheia e `--janela` inicia em janela redimensionável. O mouse desconta as margens e a escala do desenho. A fonte é Press Start 2P, 8 px, também com filtro point. Sem isso o pixel borra.

- `make assets` roda `tools/baixar_assets.sh`, que chama `baixar_assets.py` (PokéAPI) e `baixar_personagens.py` (overworld, costas do Red, Giovanni e o tileset do laboratório). Os arquivos já estão no git. O binário só lê disco.
- Mapa: tiles de 16×16 do Kenney (`assets/tiles/kenney_indoor.png`, CC0). O detetive anda célula a célula (setas ou WASD), com folha de 4 direções. O Pikachu segue atrás. Z age em painel, gelo, fogo, porta e encontro. A porta chama `casoAvancar`. O puzzle abre um desafio próprio: `casoComandarPuzzle` aplica ações à grade e só depois da conclusão chama `casoUsarHabilidade`.
- Batalha: sprite de costas embaixo à esquerda, de frente no alto à direita, plataformas ovais, caixas de HP, menu LUTAR/BOLSA/POKÉMON/FUGIR e grade de golpes com tipo e PP. BOLSA e FUGIR chamam as regras de itens/fuga em `caso.c`. NPCs e chefes recusam fuga e captura. A captura exige cinco comandos de lançamento/contenção antes de `casoCapturar`, com ícone PNG da tecla e prazo na tela.
- Barra de HP: verde acima de 50%, amarela até 20%, vermelha abaixo. A cor segue o valor animado, não o HP final.
- O texto da luta aparece letra por letra. Giovanni usa fundo vermelho. A entrada da luta fecha a tela com barras pretas.
- O caderno (tecla N) só lê a pilha e as salas já visitadas. Não crie outro inventário paralelo.
- Se um PNG faltar, a tela desenha um retângulo. Não tire esse reserva.
- Os números de `assets/pokemon/dados.txt` são os da PokéAPI. Não troque os de `dados_pokemon.c` por eles sem rodar `make teste`: o dano afinado é o que faz o `--simular` vencer.
- Som de clique e de acerto é onda gerada no código. Arquivo de áudio novo precisa de licença citada.
- Referências e o que foi aproveitado: `docs/REFERENCIAS_VISUAIS.md`.

## Convenção de comentário

Topo do `.c` e do `.h`: o módulo e, se houver estrutura, por que ela foi escolhida. Em cada função pública: o que faz, parâmetros, retorno, Big-O. Comente o trecho que mexe em ponteiro (remover da lista dupla, ciclo da lista do turno, rotação da AVL, inversão da pilha). Não comente o óbvio (`i++`).

O laboratório começa escuro. L alterna uma lanterna que acompanha o detetive e a direção em que olha; o painel resolvido pelo Pikachu restaura a iluminação normal. Na Jaula, capture Alakazam e use Z diante da mesa para examinar a área com a habilidade psíquica. Um fundo falso revela um documento com dicas sobre o Arquivo e Geodude; a pista fica no caderno (N), e o Arquivo só destrava após a busca. Não há inventário paralelo: o documento usa o estado do puzzle e a pilha de histórico.

Os desafios agora são espaciais e diferentes por habilidade, no estado `Desafio` de cada nó de sala (`include/desafio.h`, `src/desafio.c`). Pikachu gira uma rede 3×3 com percurso de conexões; Charmander derrete quatro travas de três camadas, gerenciando calor e evitando tubos; Squirtle opera tanques de capacidade 5 e 3 até obter A=4/B=3 antes de liberar a mangueira; Alakazam segue três ecos numa busca 5×4 na Jaula e empurra estantes numa grade 6×6 no Arquivo, com U para desfazer e R para reiniciar. `casoComandarPuzzle` valida parceiro e aplica ações; a janela só lê entradas e desenha. As barras de acertar no verde foram removidas. Não transformar esses desafios em quiz.

Documento: `casoPaginaDocumento` só libera as duas páginas após resolver a Jaula. A janela abre o papel em destaque automaticamente, com páginas completas e navegação; R relê, Z avança e X fecha. O documento continua baseado no nó da Jaula e na pilha, sem inventário separado.

Treino opcional: matos (g) no Pátio/Tanque e escombros (r) na Oficina/Jaula são pisáveis. A cada quatro passos na área ou ao usar Z, `casoIniciarTreino` cria um selvagem fora da fila obrigatória. Encontros obrigatórios e capturas pendentes têm prioridade. Bulbassauro, Psyduck e Geodude usam os sprites/dados já existentes. Vencer rende 30 XP a cada parceiro; 60 XP sobem um nível até 60, recalculando HP sem reviver desmaiados. Repetições dão XP, mas não duplicam espécie. Capturas opcionais reservam vagas para Charmander, Squirtle e Alakazam: nunca lotar o time impedindo o caso. Treino não muda puzzle, sala, fila nem vitória do chefe; fuga libera seu oponente. Bancos (b) restauram HP/PP; T escolhe ativo e mostra experiência; V permite voltar à sala anterior. Mesas, armários e cadeiras mantêm caminhos acessíveis.


Há dez refletores (dois no Laboratório, Câmara fria, Oficina, Jaula e Arquivo). O gerador libera energia; `casoAcenderPonto` mantém os bits de iluminação em cada sala. Cada refletor cria uma área de luz; o setor fica plenamente iluminado quando ambos estão ligados. O HUD mostra setor, próximo objetivo, refletor selecionado e PNG da tecla. A lanterna continua em L.

Os guardas da Câmara fria e da Oficina saem da mesma fila obrigatória por `casoIniciarDuelo`. Eles exigem luzes e puzzle, recusam fuga/captura e bloqueiam a saída até a vitória. Oponentes de treino continuam fora da fila e não alteram esses objetivos. Não recriar salas, capturas ou Giovanni: estender o Caso existente.

Antes de cada batalha, a tela abre a ficha da Pokédex do adversário ativo: tipos, nível, HP, atributos e golpes; setas percorrem o time de treinadores e Z confirma o duelo. Giovanni tem Mewtwo além de Meowth e Geodude. O Arquivo contém um cofre confidencial ('k') guardando a Chave Secreta; na parede lateral leste ('S'), a chave destrava a passagem para o Santuário Secreto. O Santuário possui três altares elementais interativos: Altar das Chamas (Fogo/Charmander), Altar da Mente (Psíquico/Alakazam) e Altar do Trovão (Choque/Pikachu). Ativar os três altares com os Pokémon corretos no time libera o pedestal central com a Master Ball. A Master Ball captura Mew, Mewtwo e as aves lendárias selvagens do santuário sem falha. Mew surge com chance 1/64, Mewtwo 1/256, as aves somadas 1/64 e qualquer encontro de treino pode ser shiny (1/128). A qualquer instante o jogador pode abrir a Pokédex com sprites oficiais (tecla P) e o Minimapa Tático da instalação com radar local (tecla M). `src/dados_pokemon.c` embute 23 espécies e completa a Pokédex lendo dados locais; `tools/baixar_assets.py` gera sprites e dados da 1ª geração completa para `assets/pokemon/`. Este checkout ainda tem só 20 linhas no `dados.txt`, portanto roda com 23 espécies até baixar os assets completos.

`assets/ui/teclas.png` e `guardas.png` são pixel art própria, gerada por `tools/gerar_interface.ps1` (ferramenta opcional de desenvolvimento; o jogo C só lê os PNGs). Sem assets de Ben 10. Referências de mecânica em `docs/REFERENCIAS_VISUAIS.md`.

Itens no mapa e exploração: cada setor conta com uma cápsula de suprimentos ('*') em pixel art com flutuação orgânica. O mapa contém Rare Candies (Laboratório, Câmara fria, Oficina, Arquivo e Santuário Secreto) que sobem o nível do parceiro ativo imediatamente até 60 e restauram seu HP, além de compartimentos com Super Poções e Poké Bolas extras no Pátio, Tanque e Jaula. O HUD de exploração é minimalista (pílula superior de 13px e barra inferior de 18px) com ícones dedicados [P], [M], [N], [T] e chave secreta dourada.

Pokédex Dinâmica e Scanner Holográfico: a Pokédex (tecla P) mantém controle de descobertas em `caso_jogo.pokedex_vistos`. Espécies ainda não encontradas são exibidas como silhuetas misteriosas com dados e atributos ocultos (???); o cabeçalho monitora o progresso de catalogação (VISTOS: XX/YY). Em batalhas, a análise tática do adversário (`BAT_ANALISE`) funciona como um Scanner Holográfico futurista com mira cibernética (lock-on nos 4 cantos em verde/ciano neon), animação de linha de varredura, sprite do Pokémon no visor e telemetria de combate (badges de tipo, barra de HP e atributos). Sprites de batalha contam com respiração viva ("squash & stretch"), sombras elípticas alinhadas nas plataformas e brilhos reluzentes para variantes Shiny.

Sprites das Aves Lendárias e Aliados em Batalha: Articuno (#144), Zapdos (#145) e Moltres (#146) possuem sprites oficiais autênticos de FireRed/LeafGreen (frente e costas em 64×64, ícones em 40×30 com transparência alfa). Em combate, os Pokémon aliados contam com barra de EXP clássica estilo GBA/FireRed, indicadores circulares de Poké Bolas em tempo real na caixa de status, animação acelerada de ofego em HP crítico (< 20%), estrelas de impacto elementar ao golpear, partículas cintilantes para variantes Shiny aliadas e tela de equipe de combate (`BAT_TIME`) com suporte aos 6 slots, ícones, badges de estado ([ATIVO] / [FNT]) e HP numérico detalhado.

Terminais de Equipe e Armazenamento (PC): Terminais de computador ('C') estão presentes no Laboratório, Pátio, Tanque, Oficina, Arquivo, Santuário Secreto e Sala do Chefe. Ao pressionar Z diante de um terminal 'C', abre-se o painel de gerenciamento cibernético com abas [EQUIPE] e [RESERVA] (alternadas com TAB ou setas laterais). O sistema reaproveita a lista duplamente encadeada (`ListaTime reserva_pc`) sem estruturas redundantes: permite reorganizar posições, definir o novo líder com [L], depositar e sacar monstros entre o time ativo e a reserva com [Z], e inspecionar telemetria completa (sprite frontal 64×64 animado, atributos ATQ/DEF/VEL, tipos e golpes). O fechamento do terminal por [ESC] ou [X] retorna à sala sem reiniciar a sessão.

Sprites de Equipe e Puzzle da Lupa de Investigação: As telas de visualização do time na exploração (tecla T) e de troca em combate (`BAT_TIME`) utilizam layout em split view com exibição do sprite oficial 64×64 animado (com respiração "squash & stretch" e brilhos para shinies), tipos, barra de vida e atributos completos do Pokémon focado, ao lado da lista navegável dos 6 membros. O desafio investigativo de rastros (`DES_RASTRO`) opera como o Puzzle da Lupa de Detetive: o cursor assume o formato de uma lupa detalhada com lente translúcida e mira; as células examinadas revelam marcas de pegadas em pixel art proporcionais à proximidade e telemetria analítica com gradiente de intensidade de rastro ("PEGADA AQUI!", "MUITO QUENTE", "RASTRO MORNO", "RASTRO FRIO").

Encontro Cinemático e Diálogo com Mewtwo pós-Giovanni: Ao derrotar o chefe Giovanni, o encerramento transiciona para a cena especial `TELA_MEWTWO`. Mewtwo surge levitando em uma névoa psíquica com animação ondulante e interpela o detetive por telepatia sobre o significado dos Pokémon. O jogador escolhe entre três respostas de diálogo: opções baseadas em empatia ou justiça convencem Mewtwo a juntar-se à equipe (`casoConvidarMewtwo` adiciona o Pokémon lendário ao time ou reserva), concluindo o caso e zerando o jogo com vitória e créditos finais.


HUD sem sobreposição: setor e pausa ficam no topo. Os atalhos P/M/N/T aparecem na pausa. Luzes e lanterna ficam na segunda linha, separadas da chave secreta. A barra inferior reserva altura para texto e ícone sem cruzar as bordas.
