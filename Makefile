# CC=gcc
# CFLAGS=-D _DEBUG -Wall

CC=gcc
LDFLAGS=-ljason
CFLAGS=-D _DEBUG -ffast-math -ggdb3 -O0 -Wall 

NAME=akinator

SRCDIR=src
OUTDIR=bin

main: ${OUTDIR}/${NAME} 
 
${OUTDIR}/${NAME}: ${SRCDIR}/*.c
	${CC} -o $@ $^ ${LDFLAGS} ${CFLAGS}

prepare:
	mkdir -p ${OUTDIR} 

clean:
	rm -rf ${OUTDIR}/*

all: prepare main 

.PHONY: all
