#!/bin/sh

if [ -f Vision ]; then
  cp Vision Vision.`arch`
fi

if [ -f Vision.ppc -a -f Vision.i386 ]; then
  lipo -create Vision.i386 Vision.ppc -output Vision.univ
else
  echo no Vision.i386 nor Vision.ppc
  exit 1
fi

if [ -f Vision.univ ]; then
  cp Vision.univ Vision.app/Contents/MacOS/Vision
fi

rm -f Vision.`arch`

