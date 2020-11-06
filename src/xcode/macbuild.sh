#!/bin/sh
while [ -n "$1" ]; do
 case "$1" in
  clean)
   xcodebuild -project src/xcode/cardboard.xcodeproj clean -configuration Release
   ;;
  all)
   xcodebuild -project src/xcode/cardboard.xcodeproj -configuration Release -alltargets
   ;;
  install)
   cp -v src/xcode/build/Release/cardboard.app/Contents/MacOS/cardboard cardboard.app/Contents/MacOS/cardboard_universal
   chmod +x cardboard.app/Contents/MacOS/cardboard_universal
   ;;
 esac
 shift
done
