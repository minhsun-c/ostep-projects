CC = gcc
CFLAGS = -Wall -Wextra -Iincludes

INC = $(wildcard includes/*.h)
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

TESTING ?= 0

ifeq ($(TESTING),1)
  CFLAGS += -DOSTEP_TESTING
endif

TARGET = wish

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c | obj 
	$(CC) $(CFLAGS) -c -o $@ $<

obj:
	mkdir obj

indent:
	clang-format -i $(SRC) $(INC)

clean:
	rm $(OBJ) $(TARGET) 
	rm -rf obj
