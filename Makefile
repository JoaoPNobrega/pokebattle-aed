# PokéBattle: Arena do Mewtwo
# Compila com gcc em C11, sem aviso do -Wall -Wextra -Wpedantic.

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -MMD -MP
LDFLAGS :=

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

.PHONY: all teste sanitizar memcheck clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(TEST_BIN): $(OBJS_TEST) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_TEST) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/testar_estruturas.o: tests/testar_estruturas.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

teste: $(TARGET) $(TEST_BIN)
	./$(TEST_BIN)
	./$(TARGET) --simular
	./$(TARGET) < tests/entrada_menu.txt

sanitizar:
	$(MAKE) clean
	$(MAKE) teste CFLAGS="$(CFLAGS) -g -O1 -fsanitize=address,undefined" LDFLAGS="-fsanitize=address,undefined"

memcheck: $(TARGET) $(TEST_BIN)
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_BIN)
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) --simular
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) < tests/entrada_menu.txt

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
