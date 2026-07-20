# TLS 1.3 Secure Client-Server Communication in C++

A secure client-server communication project implemented in C++ using OpenSSL. The project demonstrates how TLS 1.3 establishes a secure channel and how application data can additionally be protected using AES-256-GCM encryption.

## Features

- TLS 1.3 Secure Communication
- OpenSSL Library
- AES-256-GCM Encryption
- X.509 Certificate Authentication
- Secure TCP Socket Programming
- Client-Server Architecture
- Linux Compatible
- Automated Build using Makefile

## Technologies

- C++
- OpenSSL
- TLS 1.3
- AES-256-GCM
- POSIX Sockets
- Linux

## Project Structure

```
.
├── server.cpp
├── client.cpp
├── Makefile
├── README.md
```

## Build

Compile:

```bash
make
```

Generate certificates:

```bash
make certs
```

Run server:

```bash
./server
```

Run client:

```bash
./client
```

## Working

1. Server starts TLS 1.3 listener.
2. Client connects using TLS handshake.
3. Server certificate is presented.
4. Secure TLS session is established.
5. Messages are encrypted using AES-256-GCM.
6. Authentication tag is verified before decryption.
7. Decrypted plaintext is displayed by the server.

## Learning Outcomes

- TLS Handshake
- OpenSSL Programming
- X.509 Certificates
- AES-GCM Authenticated Encryption
- Secure Socket Programming
- TCP Networking
- Client-Server Architecture

## Future Improvements

- Mutual TLS (mTLS)
- Dynamic key exchange
- Random IV generation
- Secure key management
- Certificate verification
- Multiple client support
- Threaded server

## ⚠️ Security Notice

This project is intended for **educational and demonstration purposes only**.

For simplicity, the implementation uses **hardcoded AES-256-GCM encryption keys and initialization vectors (IVs)** to demonstrate secure communication over TLS.

**This approach is NOT secure for production use.** In real-world applications, the following security best practices should be followed:

- Generate a **cryptographically secure random IV** for every encryption operation.
- Never reuse an IV with the same AES-GCM key.
- Use a secure **key exchange mechanism** (e.g., TLS key negotiation, Diffie-Hellman, or ECDH) instead of hardcoded keys.
- Store and manage cryptographic keys securely using a **Key Management System (KMS)** or secure hardware modules (HSMs).
- Always validate peer certificates and perform proper certificate verification.

This project focuses on understanding **TLS 1.3**, **OpenSSL APIs**, **AES-256-GCM encryption**, and **secure socket programming**, rather than implementing a production-ready cryptographic system.
