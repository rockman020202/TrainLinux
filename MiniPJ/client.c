#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define LISTEN_PORT 8082

typedef struct {
    char peer_ip[INET_ADDRSTRLEN];
    int peer_port;
    int socket_fd;
} Connection;

Connection connections[MAX_CLIENTS];
int connection_count = 0;
pthread_t receive_thread;
int server_sock; // Socket server

void print_help() {
    printf("Available commands:\n");
    printf("help        - Show available commands\n");
    printf("myip        - Show IP address\n");
    printf("myport      - Show listening port\n");
    printf("connect     - Connect to a server\n");
    printf("list        - List connected clients\n");
    printf("terminate   - Terminate a connection\n");
    printf("send        - Send message\n");
    printf("exit        - Exit the program\n");
}

void *receive_from_server(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Disconnected from server.\n");
            close(sock);
            pthread_exit(NULL);
        }
        printf("[Server]: %s\n", buffer);
        fflush(stdout); // Make sure messages appear immediately
    }
    return NULL;
}

void connect_to_peer(char *dest_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    // Create socket TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, dest_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address\n");
        close(sock);
        return;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return;
    }

    printf("Connected to %s on port %d\n", dest_ip, port);

    //add list
    strcpy(connections[connection_count].peer_ip, dest_ip);
    connections[connection_count].peer_port = port;
    connections[connection_count].socket_fd = sock;
    connection_count++;

    // Create thread to receive messages from server
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_from_server, &sock);
    pthread_detach(recv_thread);
}

void list_connections() {
    if (connection_count == 0) {
        printf("No active connections.\n");
        return;
    }

    printf("Active Connections:\n");
    printf("%-5s %-15s %-5s\n", "ID", "IP Address", "Port");
    printf("-----------------------------\n");

    for (int i = 0; i < connection_count; i++) {
        printf("%-5d %-15s %-5d\n", i + 1, connections[i].peer_ip, connections[i].peer_port);
    }
}

void terminate_connection(int id) {
    if (id < 1 || id > connection_count) {
        printf("Invalid connection ID.\n");
        return;
    }

    int index = id - 1;
    close(connections[index].socket_fd);
    printf("Connection with %s:%d terminated.\n", connections[index].peer_ip, connections[index].peer_port);

    for (int i = index; i < connection_count - 1; i++) {
        connections[i] = connections[i + 1];
    }
    connection_count--;
}

void get_my_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
            printf("Your IP Address: %s\n", ip);
            break;
        }
    }

    freeifaddrs(ifaddr);
}

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg); 
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            close(client_sock);
            return NULL;
        }
        printf("\nReceived: %s\n", buffer);
    }
    return NULL;
}

void send_message(int id, char *message) {
    if (id < 1 || id > connection_count) {
        printf("Invalid connection ID.\n");
        return;
    }

    int index = id - 1;
    send(connections[index].socket_fd, message, strlen(message), 0);
    printf("Message sent to %s:%d\n", connections[index].peer_ip, connections[index].peer_port);
}

void process_command(char *command) {
    char cmd[BUFFER_SIZE], dest_ip[BUFFER_SIZE];
    int port, id;
    
    if (sscanf(command, "%s %s %d", cmd, dest_ip, &port) == 3 && strcmp(cmd, "connect") == 0) {
        connect_to_peer(dest_ip, port);
    } else if (strcmp(command, "help") == 0) {
        print_help();
    } else if (strcmp(command, "myip") == 0) {
        get_my_ip();
    } else if (strcmp(command, "list") == 0) {
        list_connections();
    } else if (sscanf(command, "%s %d", cmd, &id) == 2 && strcmp(cmd, "terminate") == 0) {
        terminate_connection(id);
    } else if (strcmp(command, "myport") == 0) {
        printf("Listening on port: 8080\n");
    } else if (strncmp(command, "send ", 5) == 0) {
        char message[BUFFER_SIZE];
        if (sscanf(command + 5, "%d %[^\n]", &id, message) == 2) {
            send_message(id, message);
        } else {
            printf("Invalid send command. Usage: send <ID> <message>\n");
        }
    } else if (strcmp(command, "exit") == 0) {
        printf("Exiting...\n");
        exit(0);
    } else {
        printf("Invalid command. Type 'help' for a list of commands.\n");
    }
}

int main() {
    char command[BUFFER_SIZE];
    char sendbuff[BUFFER_SIZE], recvbuff[BUFFER_SIZE];
    while (1) {
        printf("CLI> ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0;
        process_command(command);
    }
    return 0;
}

server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFF_SIZE 256
#define LISTEN_BACKLOG 5

int client_fd;

// Function to handle receiving messages from client
void *receive_messages(void *arg) {
    char recvbuff[BUFF_SIZE];
    while (1) {
        memset(recvbuff, 0, BUFF_SIZE);
        int bytes_received = recv(client_fd, recvbuff, BUFF_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            close(client_fd);
            exit(0);
        }
        printf("\nClient: %s\n", recvbuff);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char sendbuff[BUFF_SIZE];
    pthread_t recv_thread;

    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int port_no = atoi(argv[1]);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, LISTEN_BACKLOG) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", port_no);

    addr_size = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
    if (client_fd == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Create a message receiving thread
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_detach(recv_thread);

    // Main thread sends messages
    while (1) {
        printf("Reply to client: ");
        fgets(sendbuff, BUFF_SIZE, stdin);
        sendbuff[strcspn(sendbuff, "\n")] = 0;
        send(client_fd, sendbuff, strlen(sendbuff), MSG_NOSIGNAL);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
