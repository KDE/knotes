#! /bin/sh
$EXTRACTRC data/*.rc settings/*.kcfg settings/*.kcfg.cmake >> rc.cpp
$XGETTEXT `find . -name '*.h' -o -name '*.cpp'` -o $podir/knotes.pot
rm -f rc.cpp
