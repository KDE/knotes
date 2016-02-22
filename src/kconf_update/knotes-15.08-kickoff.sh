#!/bin/sh

kickoffrcname=`qtpaths --locate-file  GenericConfigLocation kickoffrc`
if [ -f "$kickoffrcname" ]; then
   sed -i "s/\/knotes.desktop/\/org.kde.knotes.desktop/" $kickoffrcname
fi
