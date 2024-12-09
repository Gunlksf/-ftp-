#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

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

void command_client(int client_fd);
void file_client(int client_fd) ;

#endif

