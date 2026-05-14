CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
LDFLAGS = -lpthread

# 目录结构
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = .

# 源文件
SERVER_SRCS = $(SRC_DIR)/Server.cpp \
              $(SRC_DIR)/Store.cpp \
              $(SRC_DIR)/CommandHandler.cpp \
              $(SRC_DIR)/ThreadPool.cpp

CLIENT_SRCS = $(SRC_DIR)/Client.cpp

# 对象文件（在build目录下）
SERVER_OBJS = $(SERVER_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CLIENT_OBJS = $(CLIENT_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# 可执行文件
SERVER_TARGET = $(BIN_DIR)/nameserver
CLIENT_TARGET = $(BIN_DIR)/nameclient

# 头文件（自动依赖）
HEADERS = $(SRC_DIR)/ThreadPool.h \
          $(SRC_DIR)/Store.h \
          $(SRC_DIR)/CommandHandler.h \
          $(SRC_DIR)/Server.h \
          $(SRC_DIR)/Client.h

.PHONY: all clean rebuild server client help debug

# 默认目标
all: server client

# 创建必要的目录
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# 链接服务器
server: $(BUILD_DIR) $(BIN_DIR) $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	@echo "🔗 链接服务器..."
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "✅ 服务器构建完成: $@"

# 链接客户端
client: $(BUILD_DIR) $(BIN_DIR) $(CLIENT_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	@echo "🔗 链接客户端..."
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "✅ 客户端构建完成: $@"

# 编译规则：自动生成依赖文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@echo "🔨 编译 $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MM $< -MT $@ -MF $(@:.o=.d)

# 包含依赖文件
-include $(SERVER_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d)

# 调试版本（带更多调试信息）
debug: CXXFLAGS += -DDEBUG -ggdb3
debug: clean all
	@echo "🐛 调试版本已构建"

# 发布版本（优化）
release: CXXFLAGS = -std=c++17 -Wall -O3 -DNDEBUG
release: clean all
	@echo "🚀 发布版本已构建"

# 清理构建文件
clean:
	@echo "🧹 清理构建文件..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✅ 清理完成"

# 完全重新构建
rebuild: clean all

# 运行服务器（默认端口8888）
run-server: server
	@echo "🚀 启动服务器（端口：8888）..."
	@./$(SERVER_TARGET)

# 运行服务器（自定义端口）
run-server-%: server
	@echo "🚀 启动服务器（端口：$*）..."
	@./$(SERVER_TARGET) $*

# 运行客户端（默认127.0.0.1:8888）
run-client: client
	@echo "🚀 启动客户端（127.0.0.1:8888）..."
	@./$(CLIENT_TARGET)

# 运行客户端（自定义地址和端口）
run-client-%: client
	@echo "🚀 启动客户端..."
	@./$(CLIENT_TARGET) $(subst -,.,$(word 1,$(subst :, ,$*))) $(word 2,$(subst :, ,$*))

# 测试示例
test: server client
	@echo "🧪 运行测试..."
	@echo "1. 启动服务器: make run-server"
	@echo "2. 启动客户端: make run-client"
	@echo "3. 在客户端输入命令测试"

# 显示帮助信息
help:
	@echo "╔══════════════════════════════════════════════════════════════╗"
	@echo "║                    NameServer Makefile                       ║"
	@echo "╠══════════════════════════════════════════════════════════════╣"
	@echo "║ 可用命令:                                                    ║"
	@echo "║                                                              ║"
	@echo "║  📦 构建:                                                    ║"
	@echo "║    make all         - 构建服务器和客户端                     ║"
	@echo "║    make server      - 只构建服务器                           ║"
	@echo "║    make client      - 只构建客户端                           ║"
	@echo "║    make debug       - 构建调试版本                           ║"
	@echo "║    make release     - 构建发布版本                           ║"
	@echo "║                                                              ║"
	@echo "║  🚀 运行:                                                    ║"
	@echo "║    make run-server  - 运行服务器（默认端口8888）             ║"
	@echo "║    make run-server-8080 - 运行服务器（自定义端口）           ║"
	@echo "║    make run-client  - 运行客户端（连接127.0.0.1:8888）       ║"
	@echo "║    make run-client-192.168.1.100:8888 - 自定义连接           ║"
	@echo "║                                                              ║"
	@echo "║  🧹 维护:                                                    ║"
	@echo "║    make clean       - 清理构建文件                           ║"
	@echo "║    make rebuild     - 完全重新构建                           ║"
	@echo "║    make test        - 显示测试说明                           ║"
	@echo "║    make help        - 显示此帮助信息                         ║"
	@echo "╚══════════════════════════════════════════════════════════════╝"

# 安装（复制到系统路径）
install: all
	@echo "📦 安装到 /usr/local/bin..."
	sudo cp $(SERVER_TARGET) /usr/local/bin/
	sudo cp $(CLIENT_TARGET) /usr/local/bin/
	@echo "✅ 安装完成"

# 卸载
uninstall:
	@echo "🗑️  从 /usr/local/bin 卸载..."
	sudo rm -f /usr/local/bin/nameserver
	sudo rm -f /usr/local/bin/nameclient
	@echo "✅ 卸载完成"