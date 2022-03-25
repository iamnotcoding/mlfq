CC=cc
CVERSION=c11
CFLAGS=-g3 -Wall -Wextra -pedantic-errors
LINKFLAGS=-lm -pthread
SRCS := $(shell find . -name "*c")
EXENAME=main

all	:	printinfo
	${CC} -o ${EXENAME} ${SRCS} -std=${CVERSION} ${CFLAGS} ${LINKFLAGS}

printinfo	:
	@echo CXX : ${CC}
	@echo CXXVERSION : ${CXXVERSION}
	@echo LINKFLAGS : ${LINKFLAGS}
	@echo SRCS : ${SRCS}
	@echo EXENAME : ${EXENAME}
	@echo ----------------------------
