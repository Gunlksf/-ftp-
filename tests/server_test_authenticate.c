#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../src/server/include/authenticate.h"  // 引入你之前写的用户认证模块

#define SERVER_PORT 12345
#define MAX_BUFFER_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char username[MAX_BUFFER_SIZE], password[MAX_BUFFER_SIZE];
    int read_size;

    // 创建套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("服务器套接字创建失败");
        exit(1);
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 任意地址

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("绑定失败");
        exit(1);
    }

    // 监听套接字
    if (listen(server_fd, 5) < 0)
    {
        perror("监听失败");
        exit(1);
    }

    printf("服务器正在监听端口 %d...\n", SERVER_PORT);

    // 接受客户端连接
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("接受客户端连接失败");
        exit(1);
    }

    // 执行用户认证
    if (authenticate(client_fd))
    {
        printf("用户认证成功\n");
    }
    else
    {
        printf("用户认证失败\n");
    }

    // 关闭连接
    close(client_fd);
    close(server_fd);

    return 0;
}

