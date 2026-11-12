# Referências visuais

Pesquisa feita antes de redesenhar a janela. Nada aqui entra na regra do jogo: dano, salas, fila e ordenação continuam onde estavam. A lista diz o que foi olhado e o que a tela aproveitou.

Os sprites dos Pokémon estão em `assets/pokemon/`. Foram baixados da [PokéAPI](https://pokeapi.co), geração III FireRed/LeafGreen (`sprites.versions['generation-iii']['firered-leafgreen']`, frente e costas), com o sprite padrão da API se esse arquivo faltar, mais o ícone da geração VII. A arte é da Nintendo e da Game Freak. O uso aqui é acadêmico, o jogo não acessa a rede, e o crédito está no README. `dados.txt` guarda nome, tipos e stats base oficiais; a batalha segue os números afinados de `src/dados_pokemon.c`.

O chão é o tileset **Roguelike Indoor** do Kenney, 16×16, CC0 (`assets/tiles/kenney_indoor.png`, licença em `LICENSE-kenney-indoor.txt`). O espelho usado no download foi o repositório [ETdoFresh/kenney.nl](https://github.com/ETdoFresh/kenney.nl), porque o site kenney.nl recusou o pedido automático. O detetive, o Giovanni e a Poké Bola em `assets/personagens/` foram desenhados para este projeto.

## Batalha no estilo GBA/DS

- [HP Bars & In-battle Menu — The Spriters Resource (FireRed / LeafGreen)](https://www.spriters-resource.com/game_boy_advance/pokemonfireredleafgreen/asset/33690/)
  Caixas de HUD da batalha e o menu de golpes. Aproveitado: caixa do oponente no alto à esquerda, caixa do jogador mais abaixo à direita, golpes em grade 2×2, moldura escura com miolo claro.
- [HP — Pokémon Wiki](https://pokemon.fandom.com/wiki/HP) e [How is the color of the Hit Point bar calculated? — Arqade](https://gaming.stackexchange.com/questions/331464/how-is-the-color-of-the-hit-point-bar-calculated)
  Verde acima de 50% do HP, amarelo de 50% até 20%, vermelho abaixo de 20%. A barra do jogo desce animada e troca de cor no meio do caminho, não no valor final.
- [How to change the Battle HUD in Pokemon Firered — r/PokemonROMhacks](https://www.reddit.com/r/PokemonROMhacks/comments/irckwe/how_to_change_the_battle_hud_in_pokemon_firered/) e [Pokemon fire red battle box — PokéCommunity](https://www.pokecommunity.com/threads/pokemon-fire-red-battle-box-help.466556/)
  A caixa de texto e a caixa de golpes são peças separadas da interface. Aproveitado: faixa de diálogo embaixo e grade de golpes no lugar do menu "Fight", já que esta etapa não tem bolsa nem fuga.
- [Pokemon Style Typewriter Dialogue — CodePen / repositório](https://codepen.io/scythianwizard/pen/oNVrGoy) e [old-pokemon-jrpg-style-message-box](https://github.com/scythianwizard/old-pokemon-jrpg-style-message-box)
  Letra por letra, estilo Gold/Silver. Aproveitado: a caixa revela o texto aos poucos; um clique completa a frase antes de deixar escolher outro golpe.
- [Pixel Dialogue — DVR Labs](https://dvrdesign.com/labs/retro-dialogue)
  Cena desenhada num buffer pequeno e ampliada em escala inteira, com suavização desligada. É o mesmo princípio da render texture virtual (240×160, o GBA) com `TEXTURE_FILTER_POINT`.
- [Battle Transition — ParticleFX Studio](https://particlefx.studio/effects/battle-transition)
  Flash e barras pretas no estilo Game Boy na entrada da luta. Aproveitado de forma simples: os Pokémon entram deslizando nas plataformas e a tela faz um fade, sem spritesheet externo.
- [GBC Modern UI + Battle Arena — Anima_nel, itch.io](https://anima-nel.itch.io/modern-rpg-ui)
  Referência de plataforma oval, variação da barra (cheia, no meio, baixa) e arena. As plataformas elípticas sob os Pokémon vêm dessa leitura. O pacote em si não foi copiado.

## Investigação, escape room e o laboratório

- [Detective Pikachu, PCL Lab — Territory Studio](https://territorystudio.com/project/detectivepikachu/)
  Telas de laboratório cheias de dado, DNA e mapa. Aproveitado: tubos verticais, brilho ciano/roxo e a sensação de equipamento ligado, não um corredor genérico.
- [Behind the scenes of Pokémon Detective Pikachu — Maxon](https://www.maxon.net/en/article/behind-the-scenes-of-pokemon-detective-pikachu)
  O mesmo laboratório em condição normal e em alerta vermelho. A sala do Giovanni usa o alerta: fundo quase preto, vermelho e tremor. O laboratório apagado usa vinheta, como uma lanterna fraca, e acende quando o Choque resolve o puzzle.
- [Detective Pikachu, capítulo 3, The PCL — Serebii](https://serebii.net/detective/chapter3.shtml)
  O caderno de caso (Case Notes) junta a pista ao cômodo. Aproveitado: a tecla N abre o caderno com o histórico da pilha e as salas já resolvidas. Não é um inventário de item novo; lê o estado que a lógica já guarda.
- [Case #1: Look For The Vial — SuperCheats](https://www.supercheats.com/detective-pikachu/walkthrough/case-1-look-for-the-vial)
  Objetos da sala (planta, livro, teclado, tanque) é que carregam a pista. Aproveitado: cada sala desenha um objeto que pisca enquanto o puzzle está aberto (painel, gelo, fogo, estante).

## Pixel art, fonte e raylib

- [Kenney — Roguelike Indoor](https://kenney.nl) (pacote CC0, espelho [ETdoFresh/kenney.nl](https://github.com/ETdoFresh/kenney.nl/tree/master/roguelikeindoor_updated))
  Tiles de 16×16 com margem de 1 px. Entrou o chão, o balcão, a estante, o monitor e a porta. A licença está em `assets/tiles/LICENSE-kenney-indoor.txt`.
- [Press Start 2P — Google Fonts / OFL](https://fonts.google.com/specimen/Press+Start+2P)
  Fonte pixel de Cody "codyan" Boisclair, licença OFL. Arquivo em `assets/fontes/PressStart2P-Regular.ttf`, texto da licença em `assets/fontes/OFL-PressStart2P.txt`. Crédito não é opcional: o nome "Press Start 2P" é reservado.
- [raylib — core_smooth_pixelperfect.c](https://github.com/raysan5/raylib/blob/master/examples/core/core_smooth_pixelperfect.c)
  Desenhar num `RenderTexture` e copiar para a janela. Aproveitado: resolução virtual 384×216, escala inteira, filtro point. A altura da textura vai invertida no retângulo de origem, como o exemplo, por causa do OpenGL.
- [raylib — text_font_filters.c](https://github.com/raysan5/raylib/blob/master/examples/text/text_font_filters.c)
  `TEXTURE_FILTER_POINT` na fonte para não borrar o pixel.
- [raylib issue 5678 — pixel font](https://github.com/raysan5/raylib/issues/5678)
  Fonte pixel carrega no tamanho em que é desenhada (aqui, 8 px). Escalar uma fonte grande com filtro bilinear apaga o pixel.

## O que ficou de fora de propósito

- Áudio do Kenney. O som de clique e de acerto é uma onda quadrada gerada no código, sem arquivo. Os sprites oficiais entraram só como PNG local, com o aviso de uso acadêmico no README.
- Pacotes pagos ou com cláusula que impeça o zip da disciplina.
- Tremor e vermelho fora da luta do Giovanni e do instante do dano. No resto da exploração a câmera fica quieta.
