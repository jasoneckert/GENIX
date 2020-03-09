#! /bin/csh

# install.csh: version 1.4 of 5/6/83
# Mesa Unix System Command Script
#
# @(#)install.csh	1.4 (NSC) 5/6/83

if ($#argv < 1) then
	echo 'Usage: install.csh <-me file list>'
	exit
endif
echo stripping and installing $*
foreach i ($*)
	echo ${i}:
	ed $i << 'EOF'
1a
%beginstrip%
.
g/%beginstrip%/d
i
.\" This version has had comments stripped; an unstripped version is available.
.
+,$g/[.	]\\".*/s///
g/[ 	][ 	]*$/s///
g/^$/d
g/\\n@/d
w _mac_temp_
q
'EOF'
	if ($i == tmac.e) then
		if (-d ../../tmac) then
			cp _mac_temp_ ../../tmac/tmac.e
		else
			cp _mac_temp_ ../../tmac.e
		endif
	else
		cp _mac_temp_ ../$i
	endif
	rm _mac_temp_
end
rm -f ../revisions
cp revisions ../revisions
echo	"Done"
exit
