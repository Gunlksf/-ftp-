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

void file_transfer(int c_fd)
{
	// 定义文件操作相关的变量
	FILE *fp;					   // 文件指针，用于读取命令行输出
	int fd1;					   // 文件描述符，用于读写文件
	char ret[1024];				   // 存储读取的数据
	int nread;					   // 存储实际读取的字节数
	char *str_loop;				   // 指向字符串操作的位置
	char *str_info = malloc(1024); // 存储文件信息的字符串
	int file_size;				   // 存储文件的大小
	int count;					   // 存储分块传输的计数
	char *file_loop;			   // 用于文件路径的指针
	int file_flag = 0;			   // 标志位，标记是否正在传输文件
	int n_write;				   // 存储写入的数据字节数
	char *char_p;				   // 字符指针，用于遍历字符串
	int this_index = 0;			   // 当前字符的位置，用于分隔文件路径
	int my_index = 0;				   // 字符计数器
	char read_buf[1024] = "";
	char write_buf[1024] = "";
	int read_size = 0;
	int write_size = 0;

	// 主循环，持续接收并处理客户端消息
	while (1)
	{
		// 清空读取缓存区
		memset(read_buf, '\0', 1024);

		// 从客户端读取数据，最多读取1024字节
		if ((read_size = read(c_fd, read_buf, 1024)) == -1)
		{
			printf("read error!\n");
		}

		// 处理读取的数据，拷贝到read_buf
		strncpy(read_buf, read_buf, read_size);

		// 如果接收到 "file exit" 消息，结束文件传输
		if (strcmp(read_buf, "file exit") == 0)
		{
			file_flag = 0;
			close(fd1);
			printf("file load finish!\n");
		}
		// 如果正在进行文件传输，写入文件
		else if (file_flag == 1)
		{
			n_write = write(fd1, read_buf, read_size);
			if (n_write == -1)
			{
				printf("write fail!\n");
				perror("why");
			}
		}
		// 如果接收到 "file start" 消息，表示开始传输文件
		else if ((file_loop = strstr(read_buf, "file start")) != NULL)
		{
			file_flag = 1;
			file_loop += strlen("file start"); // 跳过 "file start"
			printf("loading file: %s\n", file_loop);

			// 处理文件路径，计算出路径中的分隔符位置
			char_p = file_loop;
			this_index = 0;
			my_index = 0;
			while (1)
			{
				if (*char_p == '\0')
					break;
				else if (*char_p == '/')
					this_index = my_index + 1; // 找到最后一个 "/"
				my_index++;
				char_p++;
			}

			// 截取路径字符串
			char_p = file_loop;
			while (1)
			{
				if (*(char_p + this_index) == '\0')
				{
					*char_p = '\0';
					break;
				}
				*char_p = *(char_p + this_index);
				char_p++;
			}

			// 打开目标文件进行写入
			fd1 = open(file_loop, O_RDWR | O_CREAT | O_TRUNC, 0600);
		}
		// 处理 "scp" 命令，文件传输
		else if ((str_loop = strstr(read_buf, "scp")) != NULL)
		{
			str_loop += 3; // 跳过 "scp"
			printf("finding file: %s\n", str_loop);
			memset(ret, '\0', 1024);

			// 使用 popen 执行系统命令列出文件，获取目录列表
			fp = popen("ls", "r");
			nread = fread(ret, 1, 1024, fp);
			strncpy(ret, ret, nread);
			pclose(fp);

			// 清空缓冲区并尝试打开指定文件
			memset(ret, '\0', 1024);
			fd1 = open(str_loop, O_RDWR);
			if (fd1 == -1)
			{
				printf("dont find file!\n");
				if (write(c_fd, "fail", strlen("fail")) == -1)
				{
					printf("write error!\n");
				}
			}
			else
			{
				// 获取文件大小并计算分块数
				file_size = lseek(fd1, 0, SEEK_END);
				lseek(fd1, 0, SEEK_SET);
				count = (file_size / 1023) + 1;
				nread = 0;
				memset(str_info, '\0', 1024);
				sprintf(str_info, "file start%s", str_loop);

				// 发送文件传输开始的消息
				if (write(c_fd, str_info, strlen(str_info)) == -1)
				{
					printf("write error!\n");
				}
				usleep(100000);

				// 按块读取文件并传输
				while (count--)
				{
					memset(ret, '\0', 1024);
					nread = read(fd1, ret, 1023);
					if (nread == -1)
						printf("read fail!\n");
					else
					{
						strncpy(ret, ret, nread);
						if (write(c_fd, ret, strlen(ret)) == -1)
						{
							printf("write error!\n");
						}
					}
				}
				usleep(100000);

				// 传输完成，发送 "file exit" 消息
				if (write(c_fd, "file exit", strlen("file exit")) == -1)
				{
					printf("write error!\n");
				}
				printf("file load finish!\n");
			}
			close(fd1);
		}
		// 如果接收到 "fail" 消息，表示文件加载失败
		if (strcmp(read_buf, "fail") == 0)
		{
			printf("file load fail!\n");
		}

		// 如果接收到 "quit" 消息，退出程序
		if (strcmp(read_buf, "quit") == 0)
		{
			printf("client quit!\n");
			if (write(c_fd, "quit", strlen("quit")) == -1)
			{
				printf("write error!\n");
			}
			close(c_fd);
			exit(0);
		}
	}
}
