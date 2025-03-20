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
