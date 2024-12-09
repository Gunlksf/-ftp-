# 项目名称
PROJECT_NAME = file_transfer

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -g -I./src/server/include -I./src/client/include

# 目标目录
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests
SERVER_DIR = $(SRC_DIR)/server
CLIENT_DIR = $(SRC_DIR)/client

# 头文件目录
SERVER_INC_DIR = src/server/include
CLIENT_INC_DIR = src/client/include

# 源文件
SERVER_SRC = $(SERVER_DIR)/server.c $(SERVER_DIR)/command_handler.c $(SERVER_DIR)/authenticate.c
CLIENT_SRC = $(CLIENT_DIR)/client.c $(CLIENT_DIR)/command_handler.c $(CLIENT_DIR)/authenticate.c
TEST_SERVER_FILE_SRC = $(TEST_DIR)/server_test_file.c $(SERVER_DIR)/command_handler.c
TEST_SERVER_AUTH_SRC = $(TEST_DIR)/server_test_authenticate.c $(SERVER_DIR)/authenticate.c
TEST_CLIENT_FILE_SRC = $(TEST_DIR)/client_test_file.c 
TEST_CLIENT_AUTH_SRC = $(TEST_DIR)/client_test_authenticate.c 

# 输出文件
SERVER_BIN = $(BUILD_DIR)/server
CLIENT_BIN = $(BUILD_DIR)/client
TEST_SERVER_FILE_BIN = $(BUILD_DIR)/server_test_file
TEST_SERVER_AUTH_BIN = $(BUILD_DIR)/server_test_authenticate
TEST_CLIENT_FILE_BIN = $(BUILD_DIR)/client_test_file
TEST_CLIENT_AUTH_BIN = $(BUILD_DIR)/client_test_authenticate

# 默认目标
all: $(SERVER_BIN) $(CLIENT_BIN) $(TEST_SERVER_FILE_BIN) $(TEST_SERVER_AUTH_BIN) $(TEST_CLIENT_FILE_BIN) $(TEST_CLIENT_AUTH_BIN)

# 服务器可执行文件
$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# 客户端可执行文件
$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

# 服务器文件传输模块单元测试
$(TEST_SERVER_FILE_BIN): $(TEST_SERVER_FILE_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TEST_SERVER_FILE_BIN) $(TEST_SERVER_FILE_SRC) -lcheck

# 服务器认证模块单元测试
$(TEST_SERVER_AUTH_BIN): $(TEST_SERVER_AUTH_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TEST_SERVER_AUTH_BIN) $(TEST_SERVER_AUTH_SRC) -lcheck

# 客户端文件传输模块单元测试
$(TEST_CLIENT_FILE_BIN): $(TEST_CLIENT_FILE_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TEST_CLIENT_FILE_BIN) $(TEST_CLIENT_FILE_SRC) -lcheck

# 客户端认证模块单元测试
$(TEST_CLIENT_AUTH_BIN): $(TEST_CLIENT_AUTH_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TEST_CLIENT_AUTH_BIN) $(TEST_CLIENT_AUTH_SRC) -lcheck

# 清理目标
clean:
	rm -rf $(BUILD_DIR)

# 执行所有单元测试
test: $(TEST_SERVER_FILE_BIN) $(TEST_SERVER_AUTH_BIN) $(TEST_CLIENT_FILE_BIN) $(TEST_CLIENT_AUTH_BIN)
	@echo "Running all tests..."
	$(TEST_SERVER_FILE_BIN)
	$(TEST_SERVER_AUTH_BIN)
	$(TEST_CLIENT_FILE_BIN)
	$(TEST_CLIENT_AUTH_BIN)


# 运行服务器
run_server: $(SERVER_BIN)
	@echo "Running server with arguments: $(ARGS)"
	./$(SERVER_BIN) $(ARGS)

# 运行客户端
run_client: $(CLIENT_BIN)
	@echo "Running client with arguments: $(ARGS)"
	./$(CLIENT_BIN) $(ARGS)


# 运行服务器和客户端
run: run_server run_client
