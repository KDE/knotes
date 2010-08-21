#! /bin/sh
$EXTRACTRC *.rc *.kcfg >> rc.cpp
$XGETTEXT *.cpp -o $podir/knotes.pot
