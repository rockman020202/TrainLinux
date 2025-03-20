# Chat Application

This project includes a simple TCP server (server.c) and client (client.c) that support multi-threaded communication. The server listens for client connections and handles messages asynchronously. The client can connect to multiple peers, send messages, and manage connections.

## Features

- **help**: Show available commands
- **myip**: Show the client's IP address (excluding local address `127.0.0.1`).
- **myport**: Show the client's listening port
- **connect `<destination>` `<port>`**: Connect to a server at the given IP and port
- **list**: List active connections
- **terminate `<connection ID>`**: Terminate a connection by ID
- **send `<connection ID>` `<message>`**: Send a message to a connected client
- **exit**: Exit the program

## Build Instructions
**1.Go to the directory path and run the command**
make all
**2.Start server**
./server <port>
Example:
./server 8080
**3.Start client**
./client
Connect to the server from the client:
Example:
CLI> connect 127.0.0.1 8080
Send a message from the client:
CLI> send 1 Hello, Server!
List active connections:
CLI> list
Terminate a connection:
CLI> terminate 1
Exit the client:
CLI> exit
