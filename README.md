# Fenc

Fenc is a small C project that encrypts and decrypts files using an RC4-based stream cipher and verifies file integrity using SHA-256.

The project contains two programs:

- `fe` — file encryptor
- `fd` — file decryptor

## File format

Encrypted files are written in the following format:

```text
[ 2-byte padsize source ][ random padding ][ 32-byte SHA-256 hash ][ ciphertext ]
```

Where:
- the first 2 bytes are random bytes used to derive the padding size
- the padding size is computed as padsize % 4096
- the padding is random junk data placed before the encrypted payload
- the SHA-256 hash is computed from the original plaintext
- the ciphertext is the RC4-encrypted file content

## Features

- Encrypts arbitrary files
- Decrypts encrypted files
Verifies integrity using SHA-256
- Uses a temporary output file during decryption
- Rejects corrupted files or wrong keys

## Project structure

```text
.
├── fe.c         # file encryptor
├── fd.c         # file decryptor
├── fe.h         # encryptor header
├── fd.h         # decryptor header
├── rc4.c        # RC4 implementation
├── rc4.h        # RC4 header
├── sha256.c     # SHA-256 implementation
├── sha256.h     # SHA-256 header
├── makefile     # build rules
└── test.sh      # basic test script
```

## Usage
### Encrypt a file
```
./fe <input_file> <encrypted_file>
```

Example:
```
./fe hello.txt hello.enc
```

The program will prompt for a key.

### Decrypt a file
```
./fd <encrypted_file> <output_file>
```
Example:

```
./fd hello.enc hello.dec
```

The program will prompt for the same key used during encryption.

If the key is wrong or the encrypted file has been modified, the integrity check will fail and the output file will not be kept.

## Testing

A simple test script is included.

Run:
```
./test.sh
``` 

The script will:

1. create a plaintext test file
2. build the project
3. encrypt the test file
4. decrypt the encrypted file
5. compare the decrypted file with the original

If everything works correctly, the script should report that the decrypted file matches the original.

## Integrity check

During encryption:

- the plaintext is hashed with SHA-256
- the resulting 32-byte digest is written into the encrypted file header
- the plaintext is then encrypted and written as ciphertext

During decryption:

- the stored hash is read from the encrypted file
- the ciphertext is decrypted into plaintext
- the plaintext is hashed again
- the computed hash is compared with the stored hash

If the hashes do not match, the decryptor reports an integrity failure and removes the temporary output file.

## Notes

- RC4 is used here for learning purposes and is not recommended for modern secure systems.

- For real-world applications, authenticated encryption should be used instead, such as:
    - AES-GCM
    - ChaCha20-Poly1305

## Example workflow

```
echo "Hello World!" > hello.txt
./fe hello.txt hello.enc
./fd hello.enc hello.dec
cmp hello.txt hello.dec
```

If the files match, encryption and decryption are working correctly.

## Exit behavior
### Encryptor
- writes the encrypted file on success
- returns an error if file operations fail

### Decryptor
- writes decrypted output to a temporary file first
- verifies SHA-256 integrity before renaming the temp file
- deletes the temporary file if verification fails

## Future improvements
Possible improvements for the project:
- replace RC4 with a modern cipher
- derive keys using a password-based key derivation function
- store metadata more explicitly in the header
- add automated tests for wrong keys and tampered files
- refactor shared helper functions into a common utility module