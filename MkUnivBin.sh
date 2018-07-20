#!/bin/sh

if [ -f Visio ]; then
  cp Visio Visio.`arch`
fi

if [ -f Visio.ppc -a -f Visio.i386 ]; then
  lipo -create Visio.i386 Visio.ppc -output Visio.univ
else
  echo no Visio.i386 nor Visio.ppc
  exit 1
fi

if [ -f Visio.univ ]; then
  cp Visio.univ Visio.app/Contents/MacOS/Visio
fi

rm -f Visio.`arch`

