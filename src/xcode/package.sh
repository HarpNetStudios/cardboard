#!/bin/sh
cd $(dirname "$0")

CARD_HOME="../.."

#ensure old package is gone
rm -f cardboard.dmg

#make the directory which our disk image will be made of
#use /tmp as a destination because copying ourself (xcode folder) whilst compiling causes the dog to chase its tail
echo creating temporary directory
CARDPKG=`mktemp -d /tmp/cardpkg.XXXXXX` || exit 1

#leave indicator of where temp directory is in case things break
ln -sf "$CARDPKG" "build/cardpkg"

#copy executable
echo copying package
ditto "$CARD_HOME/cardboard.app" "$CARDPKG/Cardboard.app"
ditto "$CARD_HOME/cardboard.app" "$CARDPKG/Cardboard.app"

GAMEDIR="$CARDPKG/Cardboard.app/Contents/Resources"

#copy readme and data and remove unneccesary stuff
CpMac -r "$CARD_HOME/README.html" "$CARDPKG/"
CpMac -r "$CARD_HOME/docs" "$CARDPKG/"
CpMac -r "$CARD_HOME/data" "$GAMEDIR/"
CpMac -r "$CARD_HOME/packages" "$GAMEDIR/"
CpMac -r "$CARD_HOME/server-init.cfg" "$GAMEDIR/"
CpMac -r "$CARD_HOME/src" "$CARDPKG/src"
find -d "$CARDPKG" -name ".git" -exec rm -rf {} \\;
find "$CARDPKG" -name ".DS_Store" -exec rm -f {} \\;
rm -rf "$CARDPKG/src/xcode/build"
ln -sf /Applications "$CARDPKG/Applications"

#finally make a disk image out of the stuff
echo creating dmg
hdiutil create -fs HFS+ -srcfolder "$CARDPKG" -volname cardboard cardboard.dmg

#cleanup
echo cleaning up
rm -rf "$CARDPKG"

echo done!
