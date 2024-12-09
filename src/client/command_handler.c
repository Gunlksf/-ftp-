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

void command_client(int client_fd) // 定义一个处理客户端命令的函数，参数为客户端的文件描述符
{
    FILE *fp;                      // 定义文件指针
    char *char_p;                  // 字符指针，用于指向字符缓冲区
    int fd1;                       // 文件描述符
    char ret[1024];                // 读取缓冲区
    int nread;                     // 读取字节数
    char *str_loop;                // 字符串指针，查找特定命令
    char *str_info = malloc(1024); // 为传输的字符串分配内存
    int file_size;                 // 文件大小
    int count;                     // 需要发送的块数
    int read_size = 0;             // 已读字节数（未使用）
    int write_size = 0;            // 已写字节数（未使用）
    char write_buf[1024];          // 用于写入的数据缓冲区
    char read_buf[1024];           // 用于读取的数据缓冲区

    while (1) // 无限循环，直到收到退出命令
    {
        memset(write_buf, '\0', 1024); // 清空写入缓冲区
        char_p = write_buf;            // 设置 char_p 指向写入缓冲区

        while (1) // 获取用户输入的命令
        {
            scanf("%c", char_p); // 从标准输入读取字符
            if (*char_p == '\n') // 如果输入的是换行符
            {
                *char_p = '\0'; // 替换为字符串结束符
                break;          // 跳出循环
            }
            else if (*char_p == ' ') // 如果输入的是空格
                continue;            // 跳过空格
            else
                char_p++; // 否则移动到下一个字符
        }
        // 如果输入的命令是 "quit"，退出循环
        if (strcmp(write_buf, "quit") == 0)
        {
            // printf("write quit!\n");
            break;
        }
        // 检查输入的命令是否为 "load"，并处理文件加载请求
        if ((str_loop = strstr(write_buf, "load")) != NULL)
        {
            str_loop += 4; // 跳过 "load" 字符串
            printf("finding file:%s\n", str_loop);
            fd1 = open(str_loop, O_RDWR); // 打开指定的文件
            if (fd1 == -1)                // 如果文件无法打开
            {
                if (write(client_fd, "fail", strlen("fail")) == -1) // 向客户端发送失败消息
                {
                    printf("write error!\n");
                }
                printf("dont find file!\n");
            }
            else // 如果文件成功打开
            {
                file_size = lseek(fd1, 0, SEEK_END);                    // 获取文件大小
                lseek(fd1, 0, SEEK_SET);                                // 将文件指针移动到文件开头
                count = (file_size / 1023) + 1;                         // 计算需要发送的块数
                nread = 0;                                              // 重新初始化读取字节数
                memset(str_info, '\0', 1024);                           // 清空 str_info 缓冲区
                sprintf(str_info, "file start%s", str_loop);            // 发送文件开始信息
                if (write(client_fd, str_info, strlen(str_info)) == -1) // 向客户端发送文件开始信息
                {
                    printf("write error!\n");
                }
                usleep(100000); // 延时，确保数据发送完

                // 循环读取文件并分块发送
                while (count--)
                {
                    memset(ret, '\0', 1024);      // 清空读取缓冲区
                    nread = read(fd1, ret, 1023); // 从文件读取数据
                    if (nread == -1)              // 如果读取失败
                        printf("read fail!\n");
                    else // 如果读取成功
                    {
                        strncpy(ret, ret, nread);                     // 只复制实际读取的字节
                        if (write(client_fd, ret, strlen(ret)) == -1) // 向客户端发送文件数据
                        {
                            printf("write error!\n");
                        }
                    }
                }

                usleep(100000); // 延时，确保文件数据已发送

                // 发送文件结束标志
                if (write(client_fd, "file exit", strlen("file exit")) == -1)
                {
                    printf("write error!\n");
                }
                printf("file load finish!\n");
                close(fd1); // 关闭文件
            }
        }
        else // 如果不是 "load" 命令
        {
            if(!((strstr(write_buf,"scp")!=NULL)||(strstr(write_buf,"sls")!=NULL)||(strstr(write_buf,"spwd")!=NULL)||(strstr(write_buf,"quit")!=NULL)))
            {
            printf("this command dont exist!\n");
            }
            // 向客户端发送输入的命令
            if (write(client_fd, write_buf, strlen(write_buf)) == -1)
            {
                printf("write error!\n");
            }
        }


    }
}

void file_client(int client_fd)  // 客户端接收文件的函数，参数为客户端的文件描述符
{
    char *file_loop;  // 字符串指针，用于查找文件名
    int file_flag = 0;  // 文件标志，表示是否处于接收文件的状态
    int fd1;  // 文件描述符，用于打开文件
    int n_write;  // 写入的字节数
    char *char_p;  // 字符指针，用于操作文件路径字符串
    int this_index = 0;  // 当前字符位置，用于提取文件路径
    int my_index = 0;  // 字符位置索引，配合 `this_index` 来定位文件路径中的分隔符
    char read_buf[1024];  // 用于存放从服务器接收到的数据
    int read_size;  // 实际读取的数据字节
    while (1)  // 无限循环，直到接收到 "quit" 命令
    {
        memset(read_buf, '\0', 1024);  // 清空接收缓冲区
        // 从服务器读取数据
        if ((read_size = read(client_fd, read_buf, 1024)) == -1)
        {
            printf("read quit!\n");  // 如果读取失败，输出提示
        }
        
        strncpy(read_buf, read_buf, read_size);  // 复制实际读取的数据到缓冲区

        // 如果接收到 "file exit" 命令，表示文件传输完成
        if (strcmp(read_buf, "file exit") == 0)
        {
            file_flag = 0;  // 重置文件标志，表示不再接收文件
            close(fd1);  // 关闭文件
            printf("file load finish!\n");  // 打印文件加载完成的消息
        }
        else if (file_flag == 1)  // 如果处于文件接收状态
        {
            n_write = write(fd1, read_buf, read_size);  // 将接收到的数据写入文件
            if (n_write == -1)  // 如果写入失败
            {
                printf("write fail!\n");  // 输出错误信息
                perror("why");  // 输出详细错误信息
            }
        }
        else if ((file_loop = strstr(read_buf, "file start")) != NULL)  // 如果接收到文件开始标识
        {
            file_flag = 1;  // 设置文件接收标志
            file_loop += strlen("file start");  // 跳过 "file start" 字符串部分，获取文件名
            printf("loading file:%s\n", file_loop);  // 打印正在加载的文件名
            char_p = file_loop;  // 设置 char_p 为文件名的起始位置
            this_index = 0;  // 重置索引
            my_index = 0;  // 重置索引

            // 解析文件路径并提取文件名
            while (1)
            {
                if (*char_p == '\0')  // 如果到达字符串末尾，结束循环
                    break;
                else if (*char_p == '/')  // 如果遇到路径分隔符 "/"
                    this_index = my_index + 1;  // 记录最后一个分隔符的位置
                my_index++;  // 增加字符索引
                char_p++;  // 移动到下一个字符
            }

            // 将路径字符串截断，只保留文件名
            char_p = file_loop;  // 将 char_p 重置为文件名的起始位置
            while (1)
            {
                if (*(char_p + this_index) == '\0')  // 如果到达文件名末尾
                {
                    *char_p = '\0';  // 将终止符位置置为 '\0'
                    break;
                }
                *char_p = *(char_p + this_index);  // 复制文件名部分
                char_p++;  // 移动到下一个字符
            }

            // 创建并打开文件，准备接收数据
            fd1 = open(file_loop, O_RDWR | O_CREAT | O_TRUNC, 0600);
        }
        else if (strcmp(read_buf, "fail") == 0)  // 如果接收到 "fail" 标识，表示文件加载失败
        {
            printf("file load fail!\n");  // 打印文件加载失败消息
        }
        else if (strcmp(read_buf, "quit") == 0)  // 如果接收到 "quit" 命令，退出循环
        {
            // printf("read quit!\n");
            break;  // 退出循环，结束接收
        }
        else  // 如果是其他命令，打印接收到的内容
        {
            printf("%s", read_buf);  // 打印其他内容
        }
    }
}

