SRC = mystrlib.c main.c
OBJS = ${SRC:.c=.o}

.c.o:
	${CC} -c ${CFLAGS} $<

main: ${OBJS}
	gcc ${OBJS}

debug: ${OBJS}
	${CC} -g -c ${CFLAGS} ${SRC}
	gcc -g ${OBJS}
