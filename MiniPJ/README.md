README - TCP Server & Client with Multi-Threading

Overview

This project includes a simple TCP server (server.c) and client (client.c) that support multi-threaded communication. The server listens for client connections and handles messages asynchronously. The client can connect to multiple peers, send messages, and manage connections.

Features

Multi-threaded communication: Server and client use separate threads for sending and receiving messages.

Client connection management: Supports multiple simultaneous connections.

Interactive command-line interface (CLI) for the client.

Requirements

GCC compiler (for compiling C programs)

POSIX-compliant system (Linux/macOS/WSL)

Compilation

To compile both server and client programs, use:

Running the Server

Start the server by specifying a listening port:

Example:

The server will wait for client connections.

Running the Client

Start the client:

Once running, the client provides a CLI for managing connections.

Client Commands

Command

Description

help

Show available commands

myip

Show the client's IP address

myport

Show the client's listening port

connect <IP> <port>

Connect to a server at the given IP and port

list

List active connections

terminate <ID>

Terminate a connection by ID

send <ID> <message>

Send a message to a connected client

exit

Exit the program

Example Usage

Start the server:

Start the client:

Connect to the server from the client:

Send a message from the client:

List active connections:

Terminate a connection:

Exit the client:

Notes

The server must be running before the client connects.

Clients can manage multiple connections with different servers.

Messages are displayed asynchronously as they arrive.

Troubleshooting

If the client cannot connect, verify the server is running and using the correct IP and port.

Ensure firewall rules allow TCP connections on the specified port.

If the connection is terminated unexpectedly, check network stability.

License

This project is open-source and free to use.

