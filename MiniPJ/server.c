#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 256
#define LISTEN_BACKLOG 5  // Số lượng tối đa kết nối chờ xử lý

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char sendbuff[BUFF_SIZE], recvbuff[BUFF_SIZE];
    socklen_t addr_size;

    /* Kiểm tra tham số dòng lệnh */
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int port_no = atoi(argv[1]);

    /* Tạo socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* Cấu hình địa chỉ server */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Gắn socket với địa chỉ server */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Bắt đầu lắng nghe */
    if (listen(server_fd, LISTEN_BACKLOG) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", port_no);

    /* Chờ client kết nối */
    addr_size = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
    if (client_fd == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    /* Giao tiếp với client */
    while (1) {
        memset(recvbuff, 0, BUFF_SIZE);
        memset(sendbuff, 0, BUFF_SIZE);

        /* Nhận tin nhắn từ client */
        int bytes_receivedRead = read(client_fd, recvbuff, BUFF_SIZE);
        if (bytes_receivedRead <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: %s\n", recvbuff);

        /* Kiểm tra nếu client gửi "exit" thì đóng kết nối */
        if (strncmp(recvbuff, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }

        /* Nhập phản hồi từ bàn phím */
        printf("Reply to client: ");
        fgets(sendbuff, BUFF_SIZE, stdin);
        sendbuff[strcspn(sendbuff, "\n")] = 0;

        /* Gửi phản hồi đến client */
        send(client_fd, sendbuff, strlen(sendbuff), 0);
        
    }

    /* Đóng kết nối */
    close(client_fd);
    close(server_fd);

    return 0;
}
