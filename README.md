# TCP <-> UNIX Socket Proxy Bridge

This project implements a **Bidirectional Proxy Server** written in C that bridges the gap between a network TCP socket and a local UNIX domain socket.

It demonstrates core Linux system programming concepts, including Berkeley sockets, Inter-Process Communication (IPC), and I/O multiplexing using `select()`.

## Overview

I have two programs: a proxy server and a proxy client. These two programs are connected to each other via a Berkeley socket and function as translators. The proxy server is connected to the server via a Unix socket and transmits messages to it through this socket. Similarly, the proxy client is connected to the client and transmits messages to it via a Unix socket. This is how the server and client communicate with each other.

### Key Features
* **Hybrid Socket Handling:** Manages both `AF_INET` (IPv4) and `AF_UNIX` (Local IPC) sockets in a single application.
* **I/O Multiplexing:** Uses the `select()` system call to handle multiple connections within a single thread/process. This avoids the overhead of multi-threading or `fork()`.
* **Non-blocking Architecture:** Efficiently handles data flow without blocking the execution on a single client.

## Architecture

The system consists of three main components:

1.  **Proxy Server (`server_proxy`)**: The core bridge that routes traffic.
2.  **Client**: A standard network/unix client simulating a remote connection.
3.  **Proxy Client (`client_proxy`)**: A local client that routes traffic from unix socket to server proxy through network socket.
4.  **Server**: A standard server that reads messages from a Unix socket and sends a response to the message.

## Getting Started

* **Linux Environment (Ubuntu, Debian, Arch, etc.)**
* **GCC Compiler**

---

## Compilation

You can simply run Makefile in project directory:
```bash
make
```
Also you can compile single program. To do this use this command:
```bash
make file_you_want
```
Example
```bash
make server
```
To delete all executables from directory use:
```bash
make clean
```

## Running

To run all programs you can use script, but be shure to give script executable rights. To do this use this command:
```bash
sudo chmod +x run_progs.sh
```
Than you can run the script:
```bash
./run_progs.sh
```
Also you can run all programs one by one:
```bash
./server
```