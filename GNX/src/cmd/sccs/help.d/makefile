#
# makefile: version 1.3 of 11/16/82
# (System III) Sccs Makefile
#
# @(#)makefile	1.3 (NSC) 11/16/82
#

#
# Recreate all the sccs helpfiles.
#

INSDIR	= /v/mesaux/usr/nsc/lib/help

SCCS	= .
GET	= sccs -d${SCCS} get

FILES	= ad bd cb cm cmds co de default ge he prs rc un ut vc

all:	${FILES}
${FILES}:
	${GET} $@

install: all
#	-if [ ! -d ${INSDIR} ]; then mkdir ${INSDIR}; chmod 755 ${INSDIR}; fi
	minstall ${FILES} ${INSDIR}

clean:
#	rm -f ${FILES}
	echo 'clean sccs/help.d'
