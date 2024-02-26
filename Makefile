CC := clang
COMMON_CFLAGS := -Wall -Wextra -I./src/vendors/glad/include/

VENDOR_DIR := 
VENDOR_SOURCES := ./src/vendors/glad/src/glad.c

TEST_CFLAGS := $(COMMON_CFLAGS) -ggdb
TEST_LFLAGS := -lX11 -lGL
TEST_SOURCES := ./src/noe_platform_linux.c ./src/noe_core.c  $(VENDOR_SOURCES)

test.exe: ./test.c $(TEST_SOURCES)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LFLAGS)
