#!/bin/sh
while [ -n "$1" ]; do
 case "$1" in
  clean)
   xcodebuild -project cardboard.xcodeproj clean -configuration Release
   ;;
  build)
   xcodebuild -project src/xcode/cardboard.xcodeproj -configuration Release -alltargets
   ;;
  install)
   cp -v build/Release/cardboard.app/Contents/MacOS/cardboard ../../cardboard.app/Contents/MacOS/cardboard_universal
   chmod +x ../../cardboard.app/Contents/MacOS/cardboard_universal
   ;;
  package)
   exec ./package.sh
   ;;
 esac
 shift
done
