#!/bin/bash

set -e 

PLAINTEXT="hello.txt"
ENCRYPTED="hello.enc"

echo "Creating plaintext file..."
echo "Hello World!" > $PLAINTEXT

echo "Building program..."
make

echo "Encrypting file..."
echo "testkey" | ./fe $PLAINTEXT $ENCRYPTED

echo "Done"
ls -l "$PLAINTEXT" "$ENCRYPTED"

echo "Hex dump of encrypted file:"
hexdump -C "$ENCRYPTED"
