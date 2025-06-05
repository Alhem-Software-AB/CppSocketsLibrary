#!/bin/sh
set -e
cd "$(dirname "$0")"
# Build library and demo
make -C ../src -j2 >/dev/null
make >/dev/null
# Start server in background
./simple_http_server &
PID=$!
sleep 1
# Fetch page
curl -s http://127.0.0.1:8080/ > output.html
kill $PID
# Compare with reference
if diff -u index.html output.html; then
  echo "Output matches index.html"
else
  echo "Output differs" >&2
  exit 1
fi
rm -f output.html

# Test missing index.html case
mv index.html index.html.bak
./simple_http_server &
PID=$!
sleep 1
curl -s http://127.0.0.1:8080/ > output.html
kill $PID
grep -q "index.html not found" output.html && echo "404 handled" || {
  echo "Missing file handling failed" >&2
  mv index.html.bak index.html
  exit 1
}
mv index.html.bak index.html
rm -f output.html

