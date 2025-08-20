#!/bin/bash
DEST=../dist
rm -rf $DEST
mkdir -p $DEST/bin/
wget https://www.alhem.net/tmp/artify -O $DEST/bin/artify
export PATH=$PATH:`pwd`/dist/bin
artify build Sockets.fc
unzip -d .. -o -q Sockets.zip
rm -f Sockets.zip Sockets.json
