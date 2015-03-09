#/bin/bash

TARDIR="/var/ftp/ds/"
NAME="ds"

for i in ` find . | egrep '\.c$|\.h$|Makefile$'`
do
	tar -rf ${TARDIR}${NAME}-`date +%F`.tar  $i
done
