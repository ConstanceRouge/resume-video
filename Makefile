CC=gcc
CFLAGS=-Wall -g -Iinclude
LDFLAGS=-lm
EXEC=bin/resume-video
SRC= $(wildcard src/*.c)
OBJ= $(subst src/,obj/,$(SRC:.c=.o))

OBJ+= obj/stb_image.o

all: $(EXEC)

bin/resume-video: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/main.o: include/common.h include/videoutils.h

obj/stb_image.o: libs/stb_image/stb_image.c libs/stb_image/stb_image.h
	$(CC) -o $@ -c $< $(CFLAGS)

obj/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf obj/*.o

mrproper: clean
	@rm -rf $(EXEC)
