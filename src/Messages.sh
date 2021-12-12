#! /bin/sh
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
$EXTRACTRC data/*.rc settings/*.kcfg settings/*.kcfg.cmake >> rc.cpp
$XGETTEXT `find . -name '*.h' -o -name '*.cpp'| grep -v '/tests/'` -o $podir/knotes.pot
rm -f rc.cpp
