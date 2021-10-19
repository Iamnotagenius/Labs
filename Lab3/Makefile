SRC = mystrlib.c main.c stack.c queue.c
OBJS = ${SRC:.c=.o}

.c.o:
	${CC} -c ${CFLAGS} $<

main: ${OBJS}
	${CC} ${OBJS}

debug: ${OBJS}
	${CC} -Wall -g -c ${CFLAGS} ${SRC}
	${CC} -g ${OBJS}
