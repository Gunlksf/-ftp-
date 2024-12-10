#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// 向服务器发送文件的函数
void send_file(int sockfd, const char *filename) {
    char buf[1024];
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("File not found");
        return;
    }

    // 发送文件开始信息
    sprintf(buf, "file start%s", filename);
    if (write(sockfd, buf, strlen(buf)) == -1) {
        perror("Failed to send file start message");
    }

    // 读取文件并传输
    int nread;
    while ((nread = fread(buf, 1, sizeof(buf), file)) > 0) {
        if (write(sockfd, buf, nread) == -1) {
            perror("Failed to send file data");
            fclose(file);
            return;
        }
    }

    // 发送文件结束信息
    if (write(sockfd, "file exit", strlen("file exit")) == -1) {
        perror("Failed to send file exit message");
    }

    printf("File %s sent successfully.\n", filename);
    fclose(file);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);  // 服务器端口
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 服务器 IP 地址

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to server. Starting file transfer...\n");

    // 调用 send_file 函数上传文件
    send_file(sockfd, "../src/client/clock2.txt");

    // 关闭连接
    close(sockfd);

    return 0;
}

