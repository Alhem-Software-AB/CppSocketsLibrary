#!/bin/bash
artify build Sockets.fc
unzip -d .. -o -q Sockets.zip
rm -f Sockets.zip Sockets.json
