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
#define LISTEN_PORT 8080

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

void connect_to_peer(char *dest_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    // Tạo socket TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, dest_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address\n");
        close(sock);
        return;
    }

    // Kết nối đến server
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

void *receive_messages(void *arg) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LISTEN_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for incoming messages on port %d...\n", LISTEN_PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (*client_sock < 0) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, client_sock);
        pthread_detach(client_thread);
    }
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
    pthread_create(&receive_thread, NULL, receive_messages, NULL);
    char command[BUFFER_SIZE];
    while (1) {
        printf("CLI> ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0;
        process_command(command);
    }
    return 0;
}
