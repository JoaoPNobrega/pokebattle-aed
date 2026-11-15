# Arquitetura — PokéBattle: O Caso do Laboratório

Material para a defesa oral. A ideia é apontar no código qual estrutura toma qual decisão.

## O jogo em uma frase

Um detetive, com o Pikachu de parceiro, atravessa as salas do laboratório onde o Mewtwo foi criado. Cada sala trancada pede a habilidade de um tipo. Pokémon selvagens entram no time depois da luta. A última sala é a luta contra o Giovanni.

A janela (raylib) só desenha e lê o mouse. Ela não calcula dano, não destrava porta e não decide quem age primeiro.

```text
main.c
  |-- sem argumento --> ui_raylib.c  (desenha)
  |                         |
  |                         +-- Caso (caso.c)
  |                               |-- ListaSalas     onde o detetive está
  |                               |-- ListaTime      parceiros capturados
  |                               |-- FilaTreinadores lutas, da primeira à do chefe
  |                               |-- PilhaLog       caminho e lances
  |                               \-- Pokédex        hash por nome, AVL por número
  |
  \-- --simular --> simulacao.c   (o mesmo Caso, sem janela)
```

## Fluxo de uma partida

1. `comecarCaso` monta oito salas e seis confrontos na fila: Charmander (18), Vigia Rocket (Meowth 24), Squirtle (18), Engenheira Rocket (Geodude 16 e Meowth 22), Alakazam (18), Giovanni. Pikachu começa no nível 26 com dois golpes. Cada guarda vencido desbloqueia mais um golpe, restaura o time e entrega suprimentos.
2. O laboratório começa apagado. `Desafio` guarda a rede de fios da sala; `casoComandarPuzzle` gira peças e o percurso `circuitoAlcancado` verifica conexões recíprocas. Só uma rede completa permite usar a habilidade. Gelo, reservatórios, rastros e estantes têm regras próprias no mesmo módulo, sem raylib. A simulação executa ações públicas reproduzíveis, sem burlar as travas.
3. `avancarSala` exige puzzle concluído, ambos os refletores do setor religados, encontro obrigatório vencido, guarda derrotado quando houver e próxima sala desbloqueada. `casoAvancar` também recusa captura pendente. `voltarSala` permite revisitar setores, preservando seus estados.
4. No pátio, no tanque e na jaula a sala pede luta. `casoIniciarBatalha` tira o próximo treinador da fila com `proximoTreinador`. Na derrota o mesmo treinador continua carregado e o HP dos dois lados volta. Na vitória a sala marca `batalha_concluida`, a próxima destrava somente se o puzzle já estiver resolvido e, se a sala é capturável, o selvagem derrotado fica pendente.
5. `casoComandoCaptura` e `casoPrazoCaptura` informam a próxima tecla e o prazo. `casoResponderCaptura` valida lançamento e contenção: Z, três direções e Z final. Erro reinicia a sequência sem perder o parceiro necessário. Só cinco acertos liberam `casoCapturar`, que copia o Pokémon para a lista do time. O estado mora no Caso, sem temporizador ou tecla na regra.
6. A sala do chefe tem `e_chefe`. Vencer ali liga `caso_encerrado`. A janela mostra a tela de fim. A simulação imprime `RESULTADO: VITORIA`.

## Um turno de batalha

`casoExecutarGolpe` não desenha. Ele:

1. Pede ao oponente o golpe de maior dano (`escolherGolpeAdversario`).
2. Monta no máximo dois atores e chama `definirOrdemTurno`.
3. `ordenarTurnoPorInsercao` ordena por prioridade do golpe e, no empate, por velocidade. O insertion sort é estável: empate total fica com quem já estava antes no vetor, e o jogador entra primeiro nesse vetor.
4. Os atores já ordenados entram numa lista circular. `executarTurno` percorre cada nó uma vez. Se o primeiro derruba o segundo, o segundo não age.
5. Se o Pokémon ativo desmaiou e ainda há outro vivo na lista dupla do time, `avancarParaProximoVivo` manda o próximo. O mesmo vale para o time do chefe. Se não há mais ninguém vivo, a luta acaba.

A lista circular não é dona do Pokémon. O ponteiro aponta para o nó do time. Liberar a lista do turno não dá `free` no Pikachu.

## Por que cada estrutura

**Lista dupla das salas.** O prédio é uma sequência com volta. Inserir no fim é O(1) por causa de `fim`. O cursor `atual` é o lugar do detetive. Avançar e voltar são um passo de ponteiro, não um índice solto. A trava do puzzle está na função de avançar: sem isso a lista seria só um vetor com outro nome.

**Lista dupla do time.** Os capturados entram no fim. O `ativo` é quem está em campo. Quando desmaia, o cursor anda até o próximo vivo. `removerPokemon` (da etapa anterior, ainda no módulo) é onde se vê a religação de `ant` e `prox`.

**Fila de lutas.** O Charmander selvagem foi enfileirado antes do Giovanni, então ele sai antes. Não dá para enfrentar o chefe pulando a fila, porque `casoIniciarBatalha` só conhece a frente.

**Pilha do histórico.** Cada puzzle, cada troca de sala e cada lance empilha um texto. O topo é o lance mais recente, que a sala mostra. `mostrarHistorico` ainda inverte com uma segunda pilha quando alguém pede a ordem cronológica.

**Hash e AVL.** O nome ("Charmander") cai na hash. O número da Pokédex cai na AVL, cujo percurso em ordem já lista do menor para o maior. O merge sort (`ordenarPokedex`) rearruma uma cópia tirada da hash, que não tem ordem, quando a chave é outra (nome ou velocidade).

**Matriz de tipos.** Linha é o golpe, coluna é o defensor. Os valores são 0, 50, 100 e 200. Dois tipos multiplicam as casas. Elétrico contra Terra é 0, e é por isso que o Choque do Trovão não machuca o Geodude do Giovanni: o Pikachu usa outro golpe.

## Fórmula de dano

Em `calcularDano`, com inteiros:

```text
base = ((2 * nível / 5 + 2) * poder * ataque / defesa) / 50 + 2
base = base * efetividade / 100
se o golpe é do tipo do atacante, base = base * 3 / 2
base = base / 2
```

O `/2` não existe no Game Boy. Sem ele a luta acaba no primeiro golpe e a troca de Pokémon não aparece. 2× continua o dobro de 1×. Imunidade continua 0. O dano não é sorteado.

HP de batalha: `(2 * base * nível) / 100 + nível + 10`. Ataque, defesa e velocidade: `(base * nível) / 50 + 5`.

## Memória

| Dado | Dono | Liberado em |
| --- | --- | --- |
| Espécie na hash e na AVL | as duas estruturas, cada uma com a sua cópia | `encerrarCaso` |
| Pokémon do time | `ListaTime` dentro de `Jogo` | `comecarCaso` e `encerrarCaso` |
| Treinador ainda na fila | a fila | `liberarFilaTreinadores` |
| Treinador da luta atual | `Caso`, depois de sair da fila | `soltarOponente` na vitória, ou `encerrarCaso` |
| Nó da lista circular | a lista do turno | fim de `casoExecutarGolpe` |
| Texto do log | `PilhaLog` | `comecarCaso` e `encerrarCaso` |
| Cópia da captura | o struct `captura_pendente`, por valor | não aloca nó próprio; `casoCapturar` copia de novo para o time |

`enfileirarTreinador` copia o struct, inclusive os ponteiros do time. Depois disso a fila é a dona daqueles nós. Quem enfileira não pode dar `liberarListaTime` no original.

## Roteiro curto para falar

1. Abrir `sala.h` e mostrar `inicio`, `fim`, `atual`. Dizer que avançar recusa a porta se o puzzle não foi resolvido.
2. Em `resolverPuzzleDaSala`, mostrar a busca da habilidade no time e o `prox->desbloqueada = 1`.
3. Em `casoIniciarBatalha`, mostrar a fila: o ponteiro da frente não pode ser usado depois do `free` do nó. O código copia o treinador para `oponente_atual` antes disso.
4. Em `definirOrdemTurno`, mostrar o vetor, o insertion sort e a lista circular. Citar Ataque Rápido, que tem prioridade 1.
5. Em `casoCapturar`, mostrar que o selvagem entra no fim da lista dupla do time e que a sala seguinte só abre por causa desse tipo.
6. Contrastar hash (nome), AVL (número, percurso já ordenado) e merge sort (outra chave, em uma cópia).

## O que a janela não decide

`src/ui_raylib.c` escolhe o retângulo e a cor. O texto do puzzle, o HP e o nome do golpe vêm do `Caso`. As falas da história vêm de `historia.c`. A bolsa e a fuga estão em `casoUsarItem` e `casoTentarFugir`: a janela só mostra a quantidade e o resultado. Usar item ou falhar a fuga chama o mesmo turno em que o jogador entra com golpe -1 e só o oponente age. Não há `rand`. A trava da porta não entra na janela.

O laboratório começa escuro. L alterna uma lanterna que acompanha o detetive e a direção em que olha; o painel resolvido pelo Pikachu restaura a iluminação normal. Na Jaula, capture Alakazam e use Z diante da mesa para examinar a área com a habilidade psíquica. Um fundo falso revela um documento com dicas sobre o Arquivo e Geodude; a pista fica no caderno (N), e o Arquivo só destrava após a busca. Não há inventário paralelo: o documento usa o estado do puzzle e a pilha de histórico.

Os desafios agora são espaciais e diferentes por habilidade, no estado `Desafio` de cada nó de sala (`include/desafio.h`, `src/desafio.c`). Pikachu gira uma rede 3×3 com percurso de conexões; Charmander derrete quatro travas de três camadas, gerenciando calor e evitando tubos; Squirtle opera tanques de capacidade 5 e 3 até obter A=4/B=3 antes de liberar a mangueira; Alakazam segue três ecos numa busca 5×4 na Jaula e empurra estantes numa grade 6×6 no Arquivo, com U para desfazer e R para reiniciar. `casoComandarPuzzle` valida parceiro e aplica ações; a janela só lê entradas e desenha. As barras de acertar no verde foram removidas. Não transformar esses desafios em quiz.

Documento: `casoPaginaDocumento` só libera as duas páginas após resolver a Jaula. A janela abre o papel em destaque automaticamente, com páginas completas e navegação; R relê, Z avança e X fecha. O documento continua baseado no nó da Jaula e na pilha, sem inventário separado.

Treino opcional: matos (g) no Pátio/Tanque e escombros (r) na Oficina/Jaula são pisáveis. A cada quatro passos na área ou ao usar Z, `casoIniciarTreino` cria um selvagem fora da fila obrigatória. Encontros obrigatórios e capturas pendentes têm prioridade. Bulbassauro, Psyduck e Geodude usam os sprites/dados já existentes. Vencer rende 30 XP a cada parceiro; 60 XP sobem um nível até 60, recalculando HP sem reviver desmaiados. Repetições dão XP, mas não duplicam espécie. Capturas opcionais reservam vagas para Charmander, Squirtle e Alakazam: nunca lotar o time impedindo o caso. Treino não muda puzzle, sala, fila nem vitória do chefe; fuga libera seu oponente. Bancos (b) restauram HP/PP; T escolhe ativo e mostra experiência; V permite voltar à sala anterior. Mesas, armários e cadeiras mantêm caminhos acessíveis.


Há dez refletores (dois no Laboratório, Câmara fria, Oficina, Jaula e Arquivo). O gerador libera energia; `casoAcenderPonto` mantém os bits de iluminação em cada sala. Cada refletor cria uma área de luz; o setor fica plenamente iluminado quando ambos estão ligados. O HUD mostra setor, próximo objetivo, refletor selecionado e PNG da tecla. A lanterna continua em L.

Os guardas da Câmara fria e da Oficina saem da mesma fila obrigatória por `casoIniciarDuelo`. Eles exigem luzes e puzzle, recusam fuga/captura e bloqueiam a saída até a vitória. Oponentes de treino continuam fora da fila e não alteram esses objetivos. Não recriar salas, capturas ou Giovanni: estender o Caso existente.

`assets/ui/teclas.png` e `guardas.png` são pixel art própria, gerada por `tools/gerar_interface.ps1` (ferramenta opcional de desenvolvimento; o jogo C só lê os PNGs). Sem assets de Ben 10. Referências de mecânica em `docs/REFERENCIAS_VISUAIS.md`.
