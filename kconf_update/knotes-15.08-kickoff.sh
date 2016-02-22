#!/bin/sh

kickoffrcname=`kf5-config --path config --locate kickoffrc`
if [ -f "$kickoffrcname" ]; then
   sed -i "s/\/knotes.desktop/\/org.kde.knotes.desktop/" $kickoffrcname
fi
