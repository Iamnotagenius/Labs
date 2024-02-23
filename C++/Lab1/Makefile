OUT = a.out
OBJDIR = obj
SRC = main.cpp algebra.cpp geometry/geometry.cpp # source files
OBJS = $(patsubst %.cpp,${OBJDIR}/%.o,$(notdir ${SRC}))
LIBS = # libs here

.PHONY: all build debug

all: build ${OBJS}

build: ${OBJS}
	${CXX} ${SRC} ${LIBS} -o ${OUT}

debug: ${OBJS}
	${CXX} -Wall -g ${SRC} ${LIBS} -o ${OUT}
