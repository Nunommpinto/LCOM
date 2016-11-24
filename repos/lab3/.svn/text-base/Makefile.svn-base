# Makefile for the lab3 driver.

COMPILER_TYPE= gnu

CC=gcc

PROG=	lab3
SRCS=	lab3.c test3.c keyboard.c interrupt.c test3_asm.S

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>

clean:
	rm -rf *o *d ${PROG}