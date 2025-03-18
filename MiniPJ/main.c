#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    char peer_ip[INET_ADDRSTRLEN];
    int peer_port;
    int socket_fd;
} Connection;

Connection connections[MAX_CLIENTS];
int connection_count = 0;

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
    int sock;
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
    if (connection_count < MAX_CLIENTS) {
        strcpy(connections[connection_count].peer_ip, dest_ip);
        connections[connection_count].peer_port = port;
        connections[connection_count].socket_fd = sock;
        connection_count++;
    } else {
        printf("Connection list is full!\n");
        close(sock);
    }
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


void get_my_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            // Chỉ lấy địa chỉ IP từ ens33
            if (strcmp(ifa->ifa_name, "ens33") == 0) {
                struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
                printf("IP Address on ens33: %s\n", ip);
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
}

void process_command(char *command) {
    char cmd[BUFFER_SIZE], dest_ip[BUFFER_SIZE];
    int port;
    
    if (sscanf(command, "%s %s %d", cmd, dest_ip, &port) == 3 && strcmp(cmd, "connect") == 0) {
        connect_to_peer(dest_ip, port);
    } else if (strcmp(command, "help") == 0) {
        print_help();
    } else if (strcmp(command, "myip") == 0) {
        get_my_ip();
    } else if (strcmp(command, "list") == 0) {
        list_connections();
    } else if (strcmp(command, "myport") == 0) {
        printf("Listening on port: 8080\n");
    } else if (strcmp(command, "exit") == 0) {
        printf("Exiting...\n");
        exit(0);
    } else {
        printf("Invalid command. Type 'help' for a list of commands.\n");
    }
}

int main() {
    char command[BUFFER_SIZE];
    while (1) {
        printf("CLI> ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0;
        process_command(command);
    }
    return 0;
}
