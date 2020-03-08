# @(#)makefile	1.6	8/15/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

SCCS = /v/mesaux/src/sys
SUBDIRS = sys dev h MESA

vmunix:	sources
	cd MESA; make vmunix

dirs:	${SUBDIRS}

sources: ${SUBDIRS}
	for i in ${SUBDIRS}; do cd $$i; sccs -d${SCCS}/$$i get makefile; \
	make sources; cd ..; done

clean: ${SUBDIRS}
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done

clobber: ${SUBDIRS}
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done

blast:
	-for i in ${SUBDIRS}; do \
		if [ -d $$i/SCCS ] ; \
			then echo $$i: SCCS file exists; \
			else rm -rf $$i; \
		fi; \
	done

sys:	sys
	mkdir sys

dev:	dev
	mkdir dev

h:	h
	mkdir h

MESA:	MESA
	mkdir MESA
