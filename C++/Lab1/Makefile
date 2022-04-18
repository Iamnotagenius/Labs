OUT = a.out
OBJDIR = obj
SRC = main.cpp algebra.cpp geometry.cpp # source files
OBJS = $(patsubst %,${OBJDIR}/%,${SRC:.cpp=.o})
LIBS = # libs here
CC = g++

all: build ${OBJS}

build: ${OBJS}
	${CC} ${SRC} ${LIBS} -o ${OUT}

debug: ${OBJS}
	${CC} -Wall -g ${SRC} ${LIBS} -o ${OUT}
