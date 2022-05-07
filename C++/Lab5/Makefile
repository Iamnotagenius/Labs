OUT = a.out
OBJDIR = obj
SRC = main.cpp # source files
OBJS = $(patsubst %,${OBJDIR}/%,${SRC:.cpp=.o})
LIBS = # libs here
CC = g++
CFLAGS = --std=c++20

all: build ${OBJS}

$(OBJS): obj/%.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

build: ${OBJS}
	${CC} ${OBJS} ${LIBS} -o ${OUT}

debug: ${OBJS}
	${CC} -Wall -g ${SRC} ${LIBS} -o ${OUT}
