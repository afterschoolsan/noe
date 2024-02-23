CC := clang
COMMON_CFLAGS := -Wall -Wextra -ggdb

TEST_CFLAGS := $(COMMON_CFLAGS)
TEST_LFLAGS := -lX11
TEST_DEPENDENCIES := ./src/noeplatform_linux.c ./src/ncore.c

test.exe: ./test.c $(TEST_DEPENDENCIES)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LFLAGS)
