#!/bin/sh
appname=`basename $0 .sh`
bindir=$PWD
libdir=`cd "$bindir/../lib" ; pwd`
LD_LIBRARY_PATH=$libdir
export LD_LIBRARY_PATH
export QT_IM_MODULE=ibus
$bindir/$appname "$@"
