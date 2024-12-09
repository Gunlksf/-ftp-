#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

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

int authenticate(int client_fd);
int check_user_credentials(const char *username, const char *password);

#endif

