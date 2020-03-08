
#! /bin/sh

# newvers.sh: version 1.1 of 2/17/83
# 
#
# @(#)newvers.sh	1.1 (NSC) 2/17/83
if [ ! -r version ]; then echo 0 > version; fi
touch version
echo ".static" > "vers.s"
echo -n "hello::	.byte	lf,cr,'" >> "vers.s"
awk '	{	version = $1 + 1; }\
END	{	printf "National DB16000 Monitor (Rev. 1.%d) ", version >> "vers.s";\
		printf "%d\n", version > "version"; }' < version
echo -n "($USER) " >> vers.s
echo -n `date` >> vers.s
echo "',lf,cr,0" >> vers.s
echo ".endseg" >> "vers.s"
