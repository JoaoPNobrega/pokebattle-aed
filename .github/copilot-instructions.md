# Copilot — PokéBattle: O Caso do Laboratório

A fonte da verdade é o `AGENTS.md` na raiz. Siga-o mesmo se o restante do contexto sugerir outro jogo (a arena antiga do terminal não é mais o produto).

- Jogo em C11 para a disciplina AED (CESAR School, 2026.2). Detetive, escape room, Pikachu como parceiro, puzzles por habilidade de tipo, batalha por turnos, chefão no fim.
- Não é quiz. Não troque a linguagem. Não remova a interface raylib.
- Estruturas e ordenação são implementadas no projeto e entram na regra: lista dupla (salas e time), fila (lutas), pilha (histórico), lista circular e insertion sort (turno), hash e AVL (Pokédex), merge sort (reorganizar a Pokédex).
- Não puxe `glib`, STL nem `qsort` para substituir isso.
- Desenho só em `src/ui_raylib.c`. Lógica sem `DrawText` e sem `printf` de menu.
- Comentários em português do Brasil. Toda função pública explica parâmetros, retorno e Big-O.
- Preserve `make teste` e `./bin/pokebattle --simular`.
- Ao mudar comportamento ou pastas, atualize `AGENTS.md`, `README.md` e `docs/ARQUITETURA.md`.
