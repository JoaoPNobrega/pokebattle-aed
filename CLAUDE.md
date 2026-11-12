# Claude

Este repositório é o jogo **PokéBattle: O Caso do Laboratório** (AED, CESAR School, 2026.2).

Antes de editar, leia e obedeça o `AGENTS.md` na raiz. Ele é a fonte da verdade.

Regras que não podem escorregar:

- Só C. Sem tirar a janela raylib. Sem transformar o jogo em quiz.
- Lista, fila, pilha, hash, AVL e a ordenação são código nosso, na lógica do jogo. Não troque por biblioteca pronta nem por `qsort` no turno.
- Lógica em `caso.c`, `sala.c` e `batalha.c`. Desenho só em `src/ui_raylib.c`.
- Comentários e nomes em português do Brasil. Função pública com Big-O.
- Mantenha `./bin/pokebattle --simular` e `make teste`.
- Ao mudar regra ou pasta, atualize `AGENTS.md`, `README.md` e `docs/ARQUITETURA.md`.
