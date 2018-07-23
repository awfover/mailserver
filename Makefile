OUT = mailserver

SRC = $(shell find src -type f -name '*.c')
OBJ = ${SRC:.c=.o}

CC = clang
CFLAGS = -g -O0 -Wno-parentheses -Wno-switch-enum -Wno-unused-value
CFLAGS += -Wno-switch
CFLAGS += $(shell find src -type d | sed 's/^/-I /g')
LDFLAGS += -lm -lpthread

clang = $(shell which clang 2> /dev/null)
ifeq (, $(clang))
	CC = gcc
endif

TEST_SRC = $(shell find src test -type f -name '*.c' | sed '\/src\/main/d')
TEST_OBJ = ${TEST_SRC:.c=.o}

$(OUT): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@
	@printf "\e[36mCC\e[90m %s\e[0m\n" $@

test: test_runner
	@./$<

test_runner: $(TEST_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OUT) test_runner $(OBJ) $(TEST_OBJ)

.PHONY: clean test
