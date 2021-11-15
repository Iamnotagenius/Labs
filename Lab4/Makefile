OUT = a.out
OBJDIR = obj
SRC = main.c id3lib.c argparse.c
OBJS = $(patsubst %,${OBJDIR}/%,${SRC:.c=.o})

all: build ${OBJS}

$(OBJS): obj/%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

build: ${OBJS}
	${CC} ${OBJS} -o ${OUT}

debug: ${OBJS}
	${CC} -Wall -g ${SRC}
