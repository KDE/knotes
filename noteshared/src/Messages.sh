#! /bin/sh
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
$EXTRACTRC settings/*.kcfg >> rc.cpp
$XGETTEXT `find . -name '*.h' -o -name '*.cpp'` -o $podir/libnoteshared.pot
rm -f rc.cpp
