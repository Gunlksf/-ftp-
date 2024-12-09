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
#define USERS_FILE "src/server/users.txt" // 用户存储文件
int check_user_credentials(const char *username, const char *password);

int authenticate(int client_fd)
{
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    char auth_message[MAX_BUFFER_SIZE];
    int read_size;

    // 接收用户名
    read_size = recv(client_fd, username, sizeof(username), 0);
    if (read_size <= 0)
    {
        printf("用户名接收失败\n");
        return 0;  // 认证失败
    }
    username[read_size] = '\0';
    printf("接收到用户名: %s\n", username);  // 打印接收到的用户名

    // 接收密码
    read_size = recv(client_fd, password, sizeof(password), 0);
    if (read_size <= 0)
    {
        printf("密码接收失败\n");
        return 0;  // 认证失败
    }
    password[read_size] = '\0';
    printf("接收到密码: %s\n", password);  // 打印接收到的密码

    // 检查用户名和密码是否匹配
    if (check_user_credentials(username, password))
    {
        send(client_fd, "用户认证成功\n", 26, 0);
        return 1;  // 认证成功
    }
    else
    {
        send(client_fd, "Authentication failed.\n", 23, 0);
        return 0;  // 认证失败
    }
}




// 检查用户名和密码是否与 users.txt 文件中的记录匹配
int check_user_credentials(const char *username, const char *password)
{
    FILE *file = fopen(USERS_FILE, "r");
    if (file == NULL)
    {
        printf("Failed to open users file: %s\n", USERS_FILE);
        return 0;  // 如果无法打开文件，认证失败
    }

    char stored_username[MAX_BUFFER_SIZE], stored_password[MAX_BUFFER_SIZE];

    // 从文件中读取每行，检查用户名和密码
    while (fscanf(file, "%s %s", stored_username, stored_password) != EOF)
    {
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0)
        {
            fclose(file);
            return 1;  // 找到匹配的用户名和密码，认证成功
        }
    }

    fclose(file);
    return 0;  // 如果没有找到匹配的用户名和密码，认证失败
}
