CC		= gcc
# compiler flags
FLGS	= -march=native -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare

PROJDIR = $(realpath $(CURDIR))
#SRC		= $(shell find $(PROJDIR) -name '*.c')
SRC		= i2cscan.c

BIN		= i2cscan

compile:
	${CC} ${SRC} ${FLGS} -o ${BIN}

install:
	mv ${BIN} /usr/bin

uninstall:
	rm -f /usr/bin/${BIN}

clean:
	rm -f ${BIN} *.out
