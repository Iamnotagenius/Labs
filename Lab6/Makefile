OUT = a.out
OBJDIR = obj
SRC = main.c arc.c
OBJS = $(patsubst %,${OBJDIR}/%,${SRC:.c=.o})

all: build ${OBJS}

$(OBJS): obj/%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

build: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${OUT}

debug: ${OBJS}
	${CC} ${CFLAGS} -g -DDEBUG ${SRC}
