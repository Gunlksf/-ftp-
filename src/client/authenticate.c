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


#define MAX_BUFFER_SIZE 1024  


int authenticate(int client_fd)
{
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    char auth_message[MAX_BUFFER_SIZE];
    int read_size;

    // 获取用户名
    printf("请输入用户名: ");
    fgets(username, MAX_BUFFER_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0'; // 去掉换行符



    // 发送用户名和密码到服务器
    if (send(client_fd, username, strlen(username), 0) == -1) {
        perror("发送用户名失败");
        return 0;  // 发送失败
    }
    
    // 获取密码
    printf("请输入密码: ");
    fgets(password, MAX_BUFFER_SIZE, stdin);
    password[strcspn(password, "\n")] = '\0'; // 去掉换行符

    if (send(client_fd, password, strlen(password), 0) == -1) {
        perror("发送密码失败");
        return 0;  // 发送失败
    }

    printf("已发送用户名: %s\n", username);
    printf("已发送密码: %s\n", password);

    // 接收服务器的认证结果
    read_size = recv(client_fd, auth_message, sizeof(auth_message), 0);
    if (read_size <= 0) {
        printf("认证失败或服务器无响应\n");
        return 0; // 认证失败
    }
    auth_message[read_size] = '\0';  // 确保字符串结束

    // 输出认证信息
    printf("服务器响应: %s\n", auth_message);

    // 根据服务器的响应判断是否认证成功
    if (strcmp(auth_message, "用户认证成功\n") == 0) {
        return 1; // 认证成功
    } else {
        return 0; // 认证失败
    }
}


