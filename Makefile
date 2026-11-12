# PokéBattle: O Caso do Laboratório
# C11, sem aviso em -Wall -Wextra -Wpedantic.
# A raylib entra por pkg-config ou por third_party/ (scripts/baixar_raylib.sh).

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
LDFLAGS :=

PKG_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)
UNAME_S    := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(OS),Windows_NT)
  SYS_LIBS := -lopengl32 -lgdi32 -lwinmm
else ifeq ($(UNAME_S),Darwin)
  SYS_LIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
  SYS_LIBS := -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXinerama -lXi -lXcursor
endif

ifeq ($(strip $(PKG_LIBS)),)
  CFLAGS += -Ithird_party/raylib/src
  LDFLAGS += third_party/raylib/src/libraylib.a $(SYS_LIBS)
  RAYLIB_DEP := third_party/raylib/src/libraylib.a
else
  CFLAGS += $(PKG_CFLAGS)
  LDFLAGS += $(PKG_LIBS)
  RAYLIB_DEP :=
endif

SRC       := $(sort $(wildcard src/*.c))
SRC_MAIN  := $(filter %/main.c,$(SRC))
SRC_LIB   := $(filter-out $(SRC_MAIN),$(SRC))

OBJ_DIR   := build
BIN_DIR   := bin
OBJS      := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))
OBJS_TEST := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC_LIB)) $(OBJ_DIR)/testar_estruturas.o
DEPS      := $(OBJS:.o=.d) $(OBJ_DIR)/testar_estruturas.d

TARGET    := $(BIN_DIR)/pokebattle
TEST_BIN  := $(BIN_DIR)/testar_estruturas

.PHONY: all teste sanitizar memcheck clean assets

all: $(TARGET)

$(TARGET): $(OBJS) $(RAYLIB_DEP) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(TEST_BIN): $(OBJS_TEST) $(RAYLIB_DEP) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_TEST) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/testar_estruturas.o: tests/testar_estruturas.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

third_party/raylib/src/libraylib.a:
	./scripts/baixar_raylib.sh

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Baixa sprites e o dados.txt da PokéAPI para assets/pokemon/.
# O jogo em si não faz HTTP; este alvo é só para refazer os arquivos.
assets:
	./tools/baixar_assets.sh

# A janela não entra no teste automático. --simular joga o caso sem raylib na tela.
teste: $(TARGET) $(TEST_BIN)
	./$(TEST_BIN)
	./$(TARGET) --simular

sanitizar:
	$(MAKE) clean
	$(MAKE) teste CFLAGS="$(CFLAGS) -g -O1 -fsanitize=address,undefined" LDFLAGS="$(LDFLAGS) -fsanitize=address,undefined"

memcheck: $(TARGET) $(TEST_BIN)
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_BIN)
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) --simular

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
