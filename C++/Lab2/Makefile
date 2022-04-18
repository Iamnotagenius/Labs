OUT = a.out
SRC = main.cpp currency.cpp # source files
OBJS = ${SRC:.cpp=.o}
LIBS = -lcurl -ljsoncpp -pthread # libs here
CC = g++
CFLAGS = -std=c++20

all: build ${OBJS}

$(OBJS): %.o: %.cpp
	${CC} -c ${CFLAGS} $< -o $@

build: ${OBJS}
	${CC} ${OBJS} ${LIBS} -o ${OUT}

debug: ${OBJS}
	${CC} -Wall -g ${SRC} ${LIBS} -o ${OUT}
