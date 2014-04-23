CC=gcc
CFLAGS=-Wall -g -Iinclude -Ilibs/stb_image -Ilibs/SVDLIBC
LDFLAGS=-lm
EXEC=bin/SVD-working
SRC= $(wildcard src/*.c)
OBJ= $(subst src/,obj/,$(SRC:.c=.o))

OBJ+= obj/stb_image.o obj/svdlib.o obj/svdutil.o obj/las2.o

all: $(EXEC)

bin/SVD-working: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/clustering.o: include/common.h include/clustering.h
obj/imagefeatures.o: include/common.h include/imagefeatures.h
obj/main.o: include/common.h include/clustering.h include/imagefeatures.h include/videoutils.h
obj/videoutils.o: include/common.h include/videoutils.h

obj/stb_image.o: libs/stb_image/stb_image.c
	$(CC) -o $@ -c $< $(CFLAGS)

obj/svdlib.o: libs/SVDLIBC/svdlib.c
	$(CC) -o $@ -c $< $(CFLAGS)

obj/svdutil.o: libs/SVDLIBC/svdutil.c
	$(CC) -o $@ -c $< $(CFLAGS)

obj/las2.o: libs/SVDLIBC/las2.c
	$(CC) -o $@ -c $< $(CFLAGS)

obj/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf obj/*.o

mrproper: clean
	@rm -rf $(EXEC)
