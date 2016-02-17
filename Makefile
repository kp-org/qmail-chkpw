#
# makefile for qmail-chkpw (checkpassword tool)
#

TARGET=qmail-chkpw
BINDIR=`head -1 conf-qmail`/bin
MANDIR=`head -1 conf-qmail`/man
SRCS=main.c md5c.c hmac_md5.c
MAKEFILE=Makefile
OBJS=${SRCS:.c=.o}

CC=gcc
CFLAGS=-c -lcrypt -O3 #-Wall
LD=gcc -lcrypt
LDFLAGS=-g -o ${TARGET}

default: clean main man qmail-chkpw

main: qmail-chkpw.c conf-qmail
	sed s}QMAILHOME}"`head -1 conf-qmail`"}g qmail-chkpw.c > main.c

man: qmail-chkpw.man conf-qmail
	sed s}QMAILHOME}"`head -1 conf-qmail`"}g qmail-chkpw.man > qmail-chkpw.8

qmail-chkpw: main.c ${OBJS}
	${LD} ${LDFLAGS} ${OBJS} 

clean:
	rm -f ${OBJS} ${CODEOBJS} ${GENOBJS} ${TARGET} *~ core main.c \
	  	  qmail-chkpw.8

setup:
	install ${TARGET} ${BINDIR}
#	install ${TARGET} `head -1 conf-qmail`/bin
	chown qmaild ${BINDIR}/${TARGET}
#	chmod +s ${BINDIR}/${TARGET}
	mkdir -p ${MANDIR}/man{1,8}
	cp cmd5checkpw.8 `head -1 conf-man`/man8
	cp qmail-chkpw.8 `head -1 conf-man`/man1
	cp -n smtpauth `head -1 conf-qmail`/users/smtpauth
	chown qmaild:qmail `head -1 conf-qmail`/users/smtpauth
	chmod 600 `head -1 conf-qmail`/users/smtpauth
