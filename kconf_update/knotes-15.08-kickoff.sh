#!/bin/sh

sed -i "s/\/knotes.desktop/\/org.kde.knotes.desktop/" `kf5-config --path config --locate kickoffrc`
