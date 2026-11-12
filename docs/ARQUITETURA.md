# Arquitetura — PokéBattle: O Caso do Laboratório

> O conceito mudou. Este texto ainda descreve a arena antiga e vai ser
> reescrito. Até lá, a fonte da verdade para quem for alterar o jogo
> (pessoa ou IA) é o `AGENTS.md` na raiz: detetive, salas em lista
> dupla, puzzle por habilidade, batalha reusando insertion sort e
> lista circular, interface em raylib.

# Arquitetura antiga — Arena do Mewtwo

Material para ler antes da defesa. A ideia é conseguir apontar, no código, qual estrutura toma qual decisão.

## O que o programa faz

Há dois modos e os dois usam a mesma regra:

- `./bin/pokebattle` abre o menu no terminal.
- `./bin/pokebattle --simular` joga a arena sozinho, para teste.

A tela não decide dano, ordem de turno nem quem entra na fila. Ela só pergunta e mostra. Quem decide está em `src/batalha.c` e nas estruturas.

```text
main.c
  |-- menu (ui_terminal.c) ---- lê o teclado, imprime
  |         |
  |         +-- Jogo (jogo.c)
  |               |-- HashPokedex     busca por nome
  |               |-- ArvorePokedex   busca por número, lista em ordem
  |               |-- ListaTime       time do jogador
  |               |-- FilaTreinadores Brock -> Misty -> Rocket -> Ash -> Mewtwo
  |               \-- PilhaLog        histórico
  |
  \-- --simular (simulacao.c) -- mesma arena, escolhe o golpe sozinho
```

`iniciarJogo` só carrega a Pokédex. A fila da arena nasce em `reiniciarDesafio`, quando o jogador escolhe "Nova jornada". Assim dá para consultar a Pokédex sem começar uma luta, e dá para tentar de novo sem vazar a tentativa anterior.

## Fluxo de uma jornada

1. `reiniciarDesafio` esvazia time, fila e log, e `montarArena` enfileira os cinco desafiantes. Cada um já traz o próprio `ListaTime`, com Pokémon copiados da Pokédex e preparados no nível da luta (`pokemonPreparar`).
2. O jogador monta o time. Número vai para a AVL (`buscarPorNumero`). Nome vai para a hash (`buscarPokemon`). Os dois devolvem o modelo; o time guarda uma **cópia** no nível 50. A Pokédex não é alterada quando alguém perde HP.
3. `conduzirArena` olha `frenteDaFila` (sem remover), copia o nome, e só então chama `proximoTreinador`. A cópia do nome importa: o ponteiro da frente aponta para dentro do nó, e o desenfileirar dá `free` nesse nó.
4. `conduzirBatalha` trava o líder (`definirAtivoPorPosicao` na posição 0) dos dois lados e repete o turno.
5. Se o jogador vence e ainda há alguém na fila, `restaurarTime` enche o HP e a arena segue. Se perde ou desiste, a função devolve e a interface mostra o fim. O time do treinador que saiu da fila é liberado ali mesmo.

Códigos de `conduzirBatalha` / `conduzirArena`:

| Valor | Significado |
| --- | --- |
| 1 | Jogador venceu (o treinador, ou a arena inteira) |
| 0 | Jogador perdeu |
| -1 | Desistiu |
| -2 | A entrada acabou (fim de arquivo) |

## Um turno, na ordem em que o código roda

1. Se o ativo do oponente está com HP 0, `avancarParaProximoVivo` anda na lista dupla até achar alguém vivo. Isso é o "próximo Pokémon" do jogo, não um `if` solto num vetor.
2. Se o ativo do jogador desmaiou, a interface escolhe o substituto e `definirAtivoPorPosicao` move o cursor.
3. O jogador escolhe golpe, troca ou desistência. Troca gasta o turno: o oponente ainda age.
4. `escolherGolpeAdversario` testa cada golpe com `calcularDano` e fica com o maior. É a regra do oponente nesta etapa. Não há árvore de decisão escondida.
5. `definirOrdemTurno` monta um vetor de no máximo 2 `AtorTurno` (ponteiro para o Pokémon, lado, índice do golpe, prioridade, velocidade já escalada pelo nível). Pokémon desmaiado ou golpe inválido fica de fora — é assim que a troca, em que só um lado age, passa pela mesma função.
6. `ordenarTurnoPorInsercao` ordena esse vetor. Chave: maior prioridade, depois maior velocidade. Empate não troca, e o jogador foi colocado primeiro no vetor, então o empate fica com ele. Ataque Rápido tem prioridade 1 e por isso sai na frente de um Alakazam mais rápido.
7. Os atores já ordenados entram numa **lista circular**. `fim->prox` aponta para `inicio`. `executarTurno` anda exatamente N passos. Se o primeiro derruba o segundo, `verificarDesmaio` vê HP 0 e o segundo não age. A lista guarda ponteiro: o HP mudado é o HP do time, não uma cópia.
8. A lista do turno é liberada. Os Pokémon não: eles vivem nos times. No turno seguinte a lista nasce de novo, porque a velocidade de quem entrou em campo pode ser outra.

A frase que a tela imprime ("Ordem de ação: X age antes de Y") é o resultado desse sort. Na defesa, vale rodar uma luta e mostrar essa linha.

## Por que cada estrutura

**Lista dupla do time.** Inserir no fim é O(1) porque existe ponteiro `fim`. Remover do meio precisa de `ant` e de `prox`: senão a lista parte. O caso da cabeça (`ant == NULL`, atualiza `inicio`) e o da cauda (`prox == NULL`, atualiza `fim`) estão comentados em `removerPokemon`. O ponteiro `ativo` é o Pokémon em campo. `PROX` e `ANT` no menu chamam `avancarAtivo` e `retrocederAtivo`, que circulam nas pontas.

`trocarPokemon` troca o conteúdo dos nós, não os elos. Religar dois nós vizinhos é fácil de errar e o jogador não vê diferença. A cirurgia de ponteiro fica na remoção, que realmente tira um nó.

**Fila dos treinadores.** A regra é FIFO. `enfileirarTreinador` é O(1) pelo ponteiro `tras`. A arena não reordena a fila. Quem está na frente é o próximo desafio, e só sai quando a batalha começa.

**Lista circular do turno.** O ciclo deixa explícito que a ação volta ao primeiro. Hoje cabem 2 atores. A função de ordenar não sabe disso: ela recebe N. Uma batalha dupla, na etapa seguinte, colocaria 4 no mesmo vetor e na mesma lista.

**Pilha do histórico.** O lance novo fica no topo, que é como a gente lembra o combate. `mostrarHistorico` usa duas pilhas: desempilha tudo para uma auxiliar (isso inverte) e desempilha a auxiliar imprimindo, devolvendo cada texto à pilha original. No fim o log está igual. Vale desenhar três lances no quadro e mostrar a inversão.

**Hash da Pokédex.** djb2 no nome, ignorando maiúsculas, módulo 31. Colisão vai para uma lista simples no balde (encadeamento separado). Digitar "pikachu" ou "Pikachu" acha o mesmo nó. A ordem dos baldes não é a ordem da Pokédex — por isso o menu de organizar copia para um vetor e ordena.

**AVL por número.** Inserir 1, 3, 4, 6… numa BST comum vira uma lista inclinada, busca O(n). A AVL gira (simples ou dupla) e a busca fica O(log n). O percurso em ordem já entrega a Pokédex do menor número ao maior. O merge sort por número existe para mostrar o algoritmo em cima do vetor tirado da hash, que nasce sem ordem. As duas coisas respondem perguntas diferentes: a árvore mantém a ordem do número o tempo todo; o merge sort rearruma uma cópia por qualquer chave (número, nome, velocidade).

**Matriz de tipos.** Linha = tipo do golpe, coluna = tipo do defensor. Os valores são 0, 50, 100 e 200 (porcentagem), para a conta ficar inteira. Dois tipos multiplicam as duas casas: Água contra Pedra/Terra é 200 × 200 / 100 = 400, o "extremamente efetivo".

## Fórmula de dano

Em `calcularDano`, com inteiros, arredondando para baixo como o jogo clássico:

```text
base = ((2 * nível / 5 + 2) * poder * ataque / defesa) / 50 + 2
base = base * efetividade / 100
se o golpe é do tipo do atacante, base = base * 3 / 2    (STAB)
base = base / 2
```

Ataque, defesa e velocidade de batalha são `(base * nível) / 50 + 5`. HP é `(2 * base * nível) / 100 + nível + 10`. Há um atributo de ataque só: atacante especial (Mewtwo, Alakazam, Gengar) guarda nesse campo a força que ele realmente usa, senão Psíquico sairia fraco.

O `/2` é ajuste desta etapa, não da fórmula de Game Boy. Sem ele a luta acabava no primeiro golpe e o resto da mecânica não aparecia. 2× continua o dobro de 1×, e imunidade continua 0.

O dano não é sorteado. A mesma situação sempre dá o mesmo número. Isso deixa o teste e a conta da defesa reproduzíveis.

## Memória: quem é dono do quê

| Dado | Dono | Liberado em |
| --- | --- | --- |
| Espécie na hash e na AVL | as duas estruturas, cada uma com sua cópia | `encerrarJogo` |
| Pokémon do time do jogador | `ListaTime` dentro de `Jogo` | `reiniciarDesafio` e `encerrarJogo` |
| Pokémon do treinador | `ListaTime` dentro do `Treinador` | quem desenfileirou, ao fim da batalha; ou `liberarFilaTreinadores` se a jornada for cancelada |
| Nó da lista circular | a lista do turno | fim de `executarTurno` (não libera o Pokémon) |
| Texto do log | `PilhaLog` | `reiniciarDesafio` e `encerrarJogo` |
| Vetor auxiliar do merge sort | `ordenarPokedex` | a própria função, antes de voltar |

`enfileirarTreinador` copia o struct, inclusive os ponteiros do time. Depois disso a fila é a dona dos nós. O chamador não pode dar `liberarListaTime` no original. Isso está comentado em `incluirTreinador`.

Não há `free` de ponteiro que a lista circular tenha "emprestado". O ponteiro do ator aponta para o Pokémon que continua no time.

## Roteiro curto para falar na defesa

1. Abrir `lista_time.h` e mostrar `inicio`, `fim`, `ativo`. Explicar que `ativo` é o estado da batalha.
2. Em `removerPokemon`, desenhar `A <-> B <-> C` virando `A <-> C`.
3. Em `conduzirArena`, mostrar a fila: olha a frente, copia o nome, desenfileira. Dizer por que o ponteiro não pode ser usado depois do `free`.
4. Em `definirOrdemTurno`, mostrar o vetor, o insertion sort e a lista circular. Citar Ataque Rápido como caso em que a prioridade vence a velocidade.
5. Em `mostrarHistorico`, fazer a inversão de três strings no quadro.
6. Contrastar hash (nome, O(1) médio) com AVL (número, O(log n), percurso já ordenado) e com o merge sort (rearruma uma cópia por outra chave).
7. Se perguntarem do `/2` no dano, explicar que a proporção dos tipos foi preservada e que o divisor existe para a luta ter vários turnos.

## O que esta etapa não faz

Não desenha sprite, não separa golpe físico de especial, não aplica paralisia, queimadura ou item, e não grava arquivo. O ponto de extensão da tela é `src/ui_terminal.c`: os callbacks `PedirAcaoJogador`, `PedirTrocaJogador` e `Narrador` são o contrato. Uma interface raylib preencheria os mesmos callbacks e chamaria `conduzirArena` igual.
