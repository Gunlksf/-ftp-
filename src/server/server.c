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
#include "include/command_handler.h"
#include "include/authenticate.h"

#define MAX_CLIENTS 5  // 最大客户端数

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("arg error!\n");
        exit(-1);
    }

    int sockfd, c_fd;
    struct sockaddr_in s_addr, c_addr;
    socklen_t c_size;
    pid_t pid1;
    char *c_ip;
    char read_buf[1024] = "";
    char write_buf[1024] = "";
    int read_size = 0;
    int write_size = 0;
    memset(&s_addr, 0, sizeof(struct sockaddr_in));
    memset(&c_addr, 0, sizeof(struct sockaddr_in));

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(s_addr.sin_addr));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket create fail!\n");
        exit(-1);
    }

    if (bind(sockfd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("bind fail!\n");
        exit(-1);
    }
    else
    {
        printf("server:%s %d\n", argv[1], ntohs(s_addr.sin_port));
    }

    if (listen(sockfd, MAX_CLIENTS) == -1)
    {
        printf("listen fail!\n");
        exit(-1);
    }

    c_size = sizeof(struct sockaddr_in);

    while (1)
    {
        // 等待客户端连接
        if ((c_fd = accept(sockfd, (struct sockaddr *)&c_addr, &c_size)) == -1)
        {
            printf("ip+port get error\n");
            perror("why");
            continue;  // 如果accept失败，继续等待下一个客户端连接
        }
        else
        {
            // 进行用户认证
            if (authenticate(c_fd))
            {
                printf("用户认证成功!\n");
                c_ip = inet_ntoa(c_addr.sin_addr);
                printf("connect from:%s %d\n", c_ip, ntohs(c_addr.sin_port));
            }
            else
            {
                printf("用户认证失败!\n");
                close(c_fd);  // 如果认证失败，关闭连接
                continue;
            }

            // 为每个客户端创建一个子进程处理文件传输
            if ((pid1 = fork()) < 0)
            {
                printf("fork fail!\n");
                close(c_fd);
            }
            else if (pid1 == 0)  // 子进程
            {
                // 处理文件传输
                file_transfer(c_fd);
                close(c_fd);  // 处理完后关闭连接
                exit(0);      // 子进程退出
            }
            else  // 父进程
            {
                close(c_fd);  // 父进程不再需要该连接
            }
        }
    }

    close(sockfd);  // 关闭监听socket
    return 0;
}

