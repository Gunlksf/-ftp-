#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

extern void file_transfer(int c_fd);  // 引用外部定义的 file_transfer 函数

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 创建服务器套接字
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server socket creation failed");
        return 1;
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网络接口
    server_addr.sin_port = htons(8080);  // 端口号

    // 绑定套接字
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        return 1;
    }

    // 监听客户端连接
    if (listen(server_sock, 5) == -1) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is listening on port 8080...\n");

    // 等待并接受客户端连接
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Client connection failed");
        return 1;
    }

    printf("Client connected. Starting file transfer...\n");

    // 调用 file_transfer 函数进行文件传输处理
    file_transfer(client_sock);

    // 关闭连接
    close(client_sock);
    close(server_sock);

    return 0;
}

