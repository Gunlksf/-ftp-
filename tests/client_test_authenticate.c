#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define MAX_BUFFER_SIZE 1024

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];
    
    // 创建套接字
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("客户端套接字创建失败");
        exit(1);
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 连接本地服务器

    // 连接到服务器
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("连接到服务器失败");
        close(client_fd);
        exit(1);
    }

    printf("请输入用户名: ");
    fgets(username, MAX_BUFFER_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0';  // 去掉换行符
    send(client_fd, username, strlen(username), 0);
    printf("请输入密码: ");
    fgets(password, MAX_BUFFER_SIZE, stdin);
    password[strcspn(password, "\n")] = '\0';  // 去掉换行符

    // 发送用户名和密码给服务器
    send(client_fd, password, strlen(password), 0);

    // 接收服务器的认证结果
    int read_size = recv(client_fd, response, MAX_BUFFER_SIZE, 0);
    if (read_size > 0)
    {
        response[read_size] = '\0';  // 结束字符串
        printf("服务器回应: %s\n", response);
    }
    else
    {
        printf("未收到服务器的回应\n");
    }

    // 关闭连接
    close(client_fd);

    return 0;
}

