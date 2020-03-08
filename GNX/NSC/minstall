#! /bin/csh -f

# minstall.sh: version 1.6 of 2/7/83
# Unix System Command Source File
#
# @(#)minstall.sh	1.6 (NSC) 2/7/83

set cmd = cp		# Assume we'll copy the file
@ rflag = 0		# Assume no ranlibing
@ sflag = 0		# Assume no stripping
@ xflag = 0		# Assume no deleting source file's extension
while ($#argv >= 0)
    switch ($1)
    case -m: 
	set cmd = mv	# User wants to move instead
	shift
	breaksw
    case -r: 
	@ rflag = 1	# Build ranlib index for destination files
	shift
	breaksw
    case -s:
	@ sflag = 1	# Strip symbols from destination files
	shift
	breaksw
    case -x:
	@ xflag = 1	# Remove source file's extension
	shift
	breaksw
    default:
	break;		# Done with options
    endsw
end
if ($#argv <  2) then
    echo 'usage: install [-m] [-r] [-s] file file'
    echo 'or:    install [-m] [-r] [-s] [-x] file [file ...] destdir'
    exit 1
endif
if (-d $argv[$#argv]) then		# Copy many files into one directory
    set dstdir = $argv[$#argv]		# Get directory
    @ argc = $#argv - 1			# Reduce argv to list of source files
    set argv = ($argv[1-$argc])		#   ..
    foreach srcfile ($argv)
	set dstfile = $srcfile:h	# Remove source file's path
	if ($xflag) set dstfile = $srcfile:r # Remove extension if necessary
	set dstfile = $dstdir/$dstfile	# Build final destination file name
	rm -f $dstfile			# In case it has nasty protections
	$cmd $srcfile $dstfile		# Copy or move the file
	if ($sflag) mstrip $dstfile	# Strip symbols if necessary
	if ($rflag) nranlib $dstfile	# Build index for .a file if necessary
	chmod 770 $dstfile		# Leave writable by the project
    end
else					# Copy file to file
    if ($#argv != 2) then		# file to file requires exactly 2 args
	echo 'usage: install [-m] [-r] [-s] file file'
	echo 'or:    install [-m] [-r] [-s] [-x] file [file ...] destdir'
	exit 1
    else
	set srcfile = $argv[1]		# Just one file
	set dstfile = $argv[2]		# User's explicit destination
	rm -f $dstfile			# In case it has nasty protections
	$cmd $srcfile $dstfile		# Copy or move the file
	if ($sflag) mstrip $dstfile	# Strip symbols if necessary
	if ($rflag) nranlib $dstfile	# Build index for .a file if necessary
	chmod 770 $dstfile		# Leave writable by the project
    endif
endif
