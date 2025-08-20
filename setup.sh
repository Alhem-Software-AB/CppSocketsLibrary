#!/bin/bash
mkdir -p dist/bin/
wget https://www.alhem.net/tmp/artify -O dist/bin/artify
export PATH=$PATH:`pwd`/dist/bin
