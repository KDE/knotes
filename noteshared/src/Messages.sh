#! /bin/sh
$EXTRACTRC settings/*.kcfg >> rc.cpp
$XGETTEXT `find . -name '*.h' -o -name '*.cpp'` -o $podir/libnoteshared.pot
rm -f rc.cpp
