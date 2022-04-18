OUT = a.out
OBJDIR = obj
SRC = main.cpp # source files
OBJS = $(patsubst %,${OBJDIR}/%,${SRC:.cpp=.o})
LIBS = # libs here
CC = g++
all: build ${OBJS}

$(OBJS): obj/%.o: %.cpp
	${CC} -Wall -c ${CFLAGS} $< -o $@

build: ${OBJS}
	${CC} ${OBJS} ${LIBS} -o ${OUT}

debug: ${OBJS}
	${CC} -Wall -g ${SRC} ${LIBS} -o ${OUT}
