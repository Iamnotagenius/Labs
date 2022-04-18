SRC = uint1024_t.c test.c
OBJS = ${SRC:.c=.o}

.c.o:
	${CC} -c ${CFLAGS} $<

test: ${OBJS}
	gcc ${OBJS}

debug: ${OBJS}
	${CC} -g -c ${CFLAGS} ${SRC}
	gcc -g ${OBJS}
