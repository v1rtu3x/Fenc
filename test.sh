#!/bin/bash

set -e 

PLAINTEXT="hello.txt"
ENCRYPTED="hello.enc"
DECRYPTED="hello.dec"

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

echo
echo "Running decryptor..."
echo "testkey" | ./fd $ENCRYPTED "$DECRYPTED"

echo 
echo "Files after decryption:"
ls -l "$PLAINTEXT" "$DECRYPTED"

echo 
echo "Checking file integrity with cmp..."
if cmp -s "$PLAINTEXT" "$DECRYPTED"; then
    echo "SUCCESS: decrypted file matches original"
else
    echo "ERROR: decrypted file differs!"
    exit 1
fi

echo
echo "Hex dump of decrypted file:"
hexdump -C "$DECRYPTED"

echo
echo "Test completed successfully."