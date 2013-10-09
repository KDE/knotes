#! /bin/sh
$EXTRACTRC *.rc settings/*.kcfg >> rc.cpp
$XGETTEXT *.cpp -o $podir/knotes.pot
