# makefile for qmail-chkpw (checkpassword tool)
#

TARGET=qmail-chkpw
BINDIR=`head -1 conf-qmail`/bin
MANDIR=`head -1 conf-qmail`/man
SRCS=main.c md5c.c hmac_md5.c
MAKEFILE=Makefile
OBJS=${SRCS:.c=.o}

CC=gcc
CFLAGS=-c `cat conf-libs` `cat conf-cc` -O3   #-Wall
LD=gcc
LDFLAGS=-g `cat conf-libs` -o ${TARGET}

default: main man qmail-chkpw

main: qmail-chkpw.c conf-qmail configure
	sed s}QMAILHOME}"`head -1 conf-qmail`"}g qmail-chkpw.c > main.c
	./configure

man: qmail-chkpw.man conf-qmail
	sed s}QMAILHOME}"`head -1 conf-qmail`"}g qmail-chkpw.man > qmail-chkpw.8

qmail-chkpw: main.c ${OBJS}
	${LD} ${LDFLAGS} ${OBJS}

clean:
	rm -f ${OBJS} ${CODEOBJS} ${GENOBJS} ${TARGET} *~ core main.c \
	qmail-chkpw.8 crypto.lib conf-cc conf-libs trylib.o

setup:
	install ${TARGET} ${BINDIR}
	chown qmaild ${BINDIR}/${TARGET}
	mkdir -p ${MANDIR}/man{1,8}
	cp cmd5checkpw.8 `head -1 conf-man`/man8
	cp qmail-chkpw.8 `head -1 conf-man`/man1
	cp -n smtpauth `head -1 conf-qmail`/users/smtpauth
	chown qmaild:qmail `head -1 conf-qmail`/users/smtpauth
	chmod 600 `head -1 conf-qmail`/users/smtpauth
