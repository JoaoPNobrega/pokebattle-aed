# PokéBattle: Arena do Mewtwo

Jogo de batalha por turnos em C, no clima de *Pokémon: O Filme — Mewtwo Contra-Ataca* (1998). Projeto da disciplina **Algoritmos e Estruturas de Dados** (CESAR School, 2026.2, Profa. Maira Lira), tema *Ode ao Cinema*.

O jogador monta um time de até 6 Pokémon da 1ª geração e enfrenta, nesta ordem, **Brock**, **Misty**, a **Equipe Rocket**, **Ash** e o **Mewtwo**. Quem age primeiro em cada turno sai de um *insertion sort* por prioridade do golpe e velocidade. Não é um quiz.

Esta etapa é só o núcleo no terminal, sem biblioteca gráfica. A tela está isolada em `src/ui_terminal.c` para a etapa do raylib não reescrever a luta.

## Como compilar e rodar

Precisa de um compilador C11 (`gcc` ou `clang`). Não há dependência externa.

### Linux e macOS

```bash
make
./bin/pokebattle
```

No macOS o comando `gcc` costuma ser o Clang. Os avisos `-Wall -Wextra -Wpedantic` funcionam do mesmo jeito. Se `make` não existir, instale as ferramentas de linha de comando (`xcode-select --install`).

### Windows (MinGW ou MSYS2)

No prompt do MinGW, na pasta do projeto:

```bash
mingw32-make
bin\pokebattle.exe
```

Se o `make` não estiver no PATH, o mesmo binário sai com:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude -o pokebattle.exe src/*.c
pokebattle.exe
```

O terminal precisa estar em UTF-8 para os acentos aparecerem certos (Windows Terminal, ou `chcp 65001` no prompt antigo).

### Testes

```bash
make teste
```

Isso faz três coisas:

1. `bin/testar_estruturas` — lista, fila, pilha, hash, AVL, ordenação, dano e a lista circular do turno.
2. `bin/pokebattle --simular` — uma arena inteira, os dois lados escolhendo o golpe de maior dano. Termina com `RESULTADO: VITORIA`.
3. `bin/pokebattle < tests/entrada_menu.txt` — o menu de verdade: Pokédex, ordenação, montagem do time, um turno contra o Brock e desistência.

Memória:

```bash
make sanitizar    # recompila com -fsanitize=address,undefined e repete os testes
make memcheck     # o mesmo percurso sob o valgrind, se ele estiver instalado
```

`make clean` apaga `build/` e `bin/`. O zip da entrega não deve incluir o executável.

## Como jogar

No menu:

1. **Nova jornada** — monta o time e entra na arena.
2. **Consultar a Pokédex** — nome (hash), número (AVL) ou a lista em ordem.
3. **Organizar a Pokédex** — merge sort por número, nome ou velocidade.
4. **Como jogar** — resumo na própria tela.
5. **Sair**.

Na montagem do time, o nível é sempre 50. Dá para digitar o número (`25`) ou o nome (`Pikachu`). Outros comandos: `LISTAR`, `TIME`, `REMOVER <nome>`, `TROCAR <i> <j>`, `PROX`, `ANT`, `ORDENAR`, `PRONTO`, `CANCELAR`.

A posição 1 da lista é quem entra no começo de cada batalha. `TROCAR` muda essa ordem. `ORDENAR` põe o mais rápido na frente com insertion sort.

Na luta, as opções são fixas:

- `1` a `4` — golpes (slot vazio avisa e pede de novo)
- `5` — trocar (gasta o turno; o oponente ainda age)
- `6` — histórico, do lance mais antigo ao mais novo
- `7` — desistir da arena

Se o Pokémon ativo desmaia e só há um substituto, ele entra sozinho. Se há escolha, o jogo pergunta. Entre um treinador e outro o HP volta. Dentro da luta, não.

Água contra Pedra/Terra é 4× ("extremamente efetivo"). Elétrico contra Terra não tem efeito. Ataque Rápido tem prioridade 1 e age antes mesmo de um Pokémon mais rápido.

## Estrutura de pastas

```text
include/    cabeçalhos, um por módulo
src/        implementação
    ui_terminal.c   única leitura de teclado e escrita na tela
    batalha.c       regra da luta, sem printf de menu
    simulacao.c     arena automática usada por --simular
tests/      testes das estruturas e um roteiro de teclado
docs/       material de estudo para a defesa oral
bin/        executável (gerado; não vai no zip)
build/      objetos (gerado)
```

## Onde está cada requisito do roteiro

| Requisito | Onde olhar |
| --- | --- |
| C11, `gcc -Wall -Wextra` sem aviso | `Makefile` (`-std=c11 -Wall -Wextra -Wpedantic`) |
| Menu interativo | `executarInterfaceTerminal` em `src/ui_terminal.c` |
| Lista dupla na lógica, não só como depósito | `src/lista_time.c`: o cursor `ativo` é quem está em campo; `avancarParaProximoVivo` manda o próximo quando o oponente desmaia; `removerPokemon` relêga `ant`/`prox` |
| Lista circular na lógica | `definirOrdemTurno` + `executarTurno` (`src/batalha.c`, `src/lista_turno.c`). Cada turno recria o ciclo na ordem do insertion sort e percorre cada nó uma vez |
| Fila na lógica | `conduzirArena` (`src/batalha.c`) só enfrenta quem `frenteDaFila` mostra e `proximoTreinador` tira. Não dá para pular o Brock |
| Pilha na lógica | Cada lance vai para `empilharLog`. A opção 6 chama `mostrarHistorico`, que desempilha numa pilha auxiliar e restaura a original |
| Tabela hash | `buscarPokemon` (`src/hash_pokedex.c`), usada ao digitar o nome na montagem do time |
| Árvore AVL | `buscarPorNumero` e `percorrerEmOrdem` (`src/arvore_pokedex.c`). O número da Pokédex cai aqui; a listagem em ordem é o percurso, sem sort |
| Insertion sort fora de ranking | `ordenarTurnoPorInsercao`, chamada todo turno por `definirOrdemTurno`. Também `ordenarTime` / `ORDENAR`, que muda quem entra primeiro |
| Merge sort | `ordenarPokedex` (`src/ordenacao.c`), menu "Organizar a Pokédex" |
| Pelo menos 6 operações de estrutura | Na lista do time: `adicionarPokemon`, `removerPokemon`, `trocarPokemon`, `avancarAtivo`, `retrocederAtivo`, `avancarParaProximoVivo`, `definirAtivoPorPosicao`, `ordenarTime` |
| Não é quiz | A partida é uma batalha; a Pokédex só consulta ficha |

A matriz de tipos (`src/tipos.c`) entra em `calcularDano`. Sem ela o golpe não sabe se é super efetivo, pouco efetivo ou imune.

## O que ficou para as próximas etapas

- Trocar `src/ui_terminal.c` por uma tela em raylib (barras, sprites, animações), sem mexer em `batalha.c`.
- Batalha dupla: `ordenarTurnoPorInsercao` já ordena N atores; hoje N vale no máximo 2.
- Status que alteram a velocidade (paralisia) antes do insertion sort.
- Itens, experiência e evolução.
- Gravar e carregar o time.
- Mais espécies lidas de arquivo, em vez do catálogo fixo de `src/dados_pokemon.c`.
- Variação de dano (o jogo hoje é determinístico de propósito, para o teste repetir).

O divisor `/2` no fim de `calcularDano` também é desta etapa: a fórmula clássica, com os níveis da arena, apagava o time adversário em um golpe e a luta deixava de mostrar troca e desmaio. A proporção entre os tipos não muda. Na defesa, vale dizer isso com essa frase.
