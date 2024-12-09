#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/command_handler.h"
#include "./include/authenticate.h"



int main(int argc, char *argv[])
{
    pid_t pid;
    if (argc != 3)
    {
        printf("arg error!\n");
        exit(-1);
    }
    int client_fd;
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(client_addr.sin_addr));
    int flag = connect(client_fd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
    if (flag == -1)
    {
        printf("connect fail!\n");
        exit(-1);
    }
    if (authenticate(client_fd) == 0)
    {
        printf("认证失败，关闭客户端\n");
        close(client_fd);
        return 0; // 认证失败，终止客户端程序
    }
    printf("服务器:%s %s 连接成功!\n", argv[1], argv[2]);
    if ((pid = fork()) == -1)
    {
        printf("fork fail!\n");
    }
    else if (pid > 0)
    {
        command_client(client_fd);
    }
    else
    {
        file_client(client_fd);
    }
    close(client_fd);
    return 0;
}

