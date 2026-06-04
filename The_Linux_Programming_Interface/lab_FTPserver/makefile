# ============================================================
# NameServer/FTPServer - Makefile
# 支持原名字 - 年龄服务器和新 FTP 服务器
# 
# 根据 agent.md 要求更新
# 日期：2026-05-24
# ============================================================

# 编译器
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g -I.
LDFLAGS = -lpthread

# ============================================================
# Qt 配置 - 自动检测
# ============================================================

# 尝试使用 pkg-config 自动检测 Qt
QT_CXXFLAGS := $(shell pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets Qt5Network 2>/dev/null)
QT_LDFLAGS := $(shell pkg-config --libs Qt5Core Qt5Gui Qt5Widgets Qt5Network 2>/dev/null)

# 如果 pkg-config 不可用，手动设置路径
ifeq ($(QT_CXXFLAGS),)
    QT_PATH = /usr/include/x86_64-linux-gnu/qt5
    QT_CXXFLAGS = -I$(QT_PATH) \
                  -I$(QT_PATH)/QtCore \
                  -I$(QT_PATH)/QtGui \
                  -I$(QT_PATH)/QtWidgets \
                  -I$(QT_PATH)/QtNetwork \
                  -fPIC
    QT_LDFLAGS = -lQt5Core -lQt5Gui -lQt5Widgets -lQt5Network
endif

# 确保 -fPIC 标志存在
ifeq (,$(findstring -fPIC,$(QT_CXXFLAGS)))
    QT_CXXFLAGS += -fPIC
endif

# ============================================================
# 目录结构（所有文件在当前目录）
# ============================================================
BUILD_DIR = build
BIN_DIR = bin

# MOC 工具
MOC = $(shell which moc 2>/dev/null || echo /usr/lib/qt5/bin/moc)

# ============================================================
# 需要 MOC 处理的头文件
# ============================================================
MOC_HEADERS = QtServerGUI.h QtClientGUI.h

# MOC 生成文件
MOC_SRCS = $(patsubst %.h,$(BUILD_DIR)/moc_%.cpp,$(MOC_HEADERS))
MOC_OBJS = $(MOC_SRCS:.cpp=.o)

# ============================================================
# 源文件列表 - 原名字 - 年龄服务器（已注释保留）
# ============================================================
# SERVER_SRCS = Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server.cpp
# CLIENT_SRCS = Client.cpp main_client.cpp
# QT_SERVER_SRCS = QtServerGUI.cpp Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server_qt.cpp
# QT_CLIENT_SRCS = QtClientGUI.cpp main_client_qt.cpp

# ============================================================
# 源文件列表 - 新 FTP 服务器
# ============================================================

# FTP 命令行服务器（使用修改后的 Server.cpp）
FTP_SERVER_SRCS = Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server.cpp

# FTP 命令行客户端（使用修改后的 Client.cpp）
FTP_CLIENT_SRCS = Client.cpp main_client.cpp

# FTP Qt GUI 服务器（待实现）
FTP_QT_SERVER_SRCS = Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp QtFTPServerGUI.cpp main_server_qt.cpp

# FTP Qt GUI 客户端（待实现）
FTP_QT_CLIENT_SRCS = Client.cpp QtFTPClientGUI.cpp main_client_qt.cpp

# ============================================================
# 目标文件
# ============================================================
FTP_SERVER_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(FTP_SERVER_SRCS))
FTP_CLIENT_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(FTP_CLIENT_SRCS))

# 原名字 - 年龄服务器目标（保留）
SERVER_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server.cpp)
CLIENT_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,Client.cpp main_client.cpp)
QT_SERVER_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,QtServerGUI.cpp Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server_qt.cpp) $(BUILD_DIR)/moc_QtServerGUI.o
QT_CLIENT_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,QtClientGUI.cpp main_client_qt.cpp) $(BUILD_DIR)/moc_QtClientGUI.o

# ============================================================
# 可执行文件
# ============================================================

# FTP 服务器目标
FTP_SERVER_TARGET = $(BIN_DIR)/ftpserver
FTP_CLIENT_TARGET = $(BIN_DIR)/ftpclient
FTP_QT_SERVER_TARGET = $(BIN_DIR)/ftpserver_gui
FTP_QT_CLIENT_TARGET = $(BIN_DIR)/ftpclient_gui

# 原名字 - 年龄服务器目标（保留）
SERVER_TARGET = $(BIN_DIR)/nameserver
CLIENT_TARGET = $(BIN_DIR)/nameclient
QT_SERVER_TARGET = $(BIN_DIR)/nameserver_gui
QT_CLIENT_TARGET = $(BIN_DIR)/nameclient_gui

# ============================================================
# 颜色定义
# ============================================================
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
RED = \033[31m
RESET = \033[0m

# ============================================================
# 伪目标
# ============================================================
.PHONY: all clean rebuild ftp ftp-server ftp-client qt-ftp-server qt-ftp-client server client qt-server qt-client qt-gui help info

# 默认目标 - 构建 FTP 服务器
all: ftp-server ftp-client

# ============================================================
# 构建目标 - FTP 服务器
# ============================================================

ftp: ftp-server ftp-client
	@echo "$(GREEN)✅ FTP 服务器和客户端构建完成$(RESET)"

ftp-server: $(BUILD_DIR) $(BIN_DIR) $(FTP_SERVER_TARGET)
	@echo "$(GREEN)✅ FTP 命令行服务器构建完成：$(FTP_SERVER_TARGET)$(RESET)"

ftp-client: $(BUILD_DIR) $(BIN_DIR) $(FTP_CLIENT_TARGET)
	@echo "$(GREEN)✅ FTP 命令行客户端构建完成：$(FTP_CLIENT_TARGET)$(RESET)"

# ============================================================
# 构建目标 - 原名字 - 年龄服务器（保留）
# ============================================================

server: $(BUILD_DIR) $(BIN_DIR) $(SERVER_TARGET)
	@echo "$(GREEN)✅ 原命令行服务器构建完成：$(SERVER_TARGET)$(RESET)"

client: $(BUILD_DIR) $(BIN_DIR) $(CLIENT_TARGET)
	@echo "$(GREEN)✅ 原命令行客户端构建完成：$(CLIENT_TARGET)$(RESET)"

qt-server: $(BUILD_DIR) $(BIN_DIR) $(QT_SERVER_TARGET)
	@echo "$(GREEN)✅ 原 Qt GUI 服务器构建完成：$(QT_SERVER_TARGET)$(RESET)"

qt-client: $(BUILD_DIR) $(BIN_DIR) $(QT_CLIENT_TARGET)
	@echo "$(GREEN)✅ 原 Qt GUI 客户端构建完成：$(QT_CLIENT_TARGET)$(RESET)"

qt-gui: qt-server qt-client

# ============================================================
# 链接规则 - FTP 服务器
# ============================================================

$(FTP_SERVER_TARGET): $(FTP_SERVER_OBJS)
	@echo "$(YELLOW)🔗 链接 FTP 命令行服务器...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

$(FTP_CLIENT_TARGET): $(FTP_CLIENT_OBJS)
	@echo "$(YELLOW)🔗 链接 FTP 命令行客户端...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

# ============================================================
# 链接规则 - 原名字 - 年龄服务器（保留）
# ============================================================

$(SERVER_TARGET): $(SERVER_OBJS)
	@echo "$(YELLOW)🔗 链接原命令行服务器...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	@echo "$(YELLOW)🔗 链接原命令行客户端...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

$(QT_SERVER_TARGET): $(QT_SERVER_OBJS)
	@echo "$(YELLOW)🔗 链接原 Qt GUI 服务器...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS) $(QT_LDFLAGS)

$(QT_CLIENT_TARGET): $(QT_CLIENT_OBJS)
	@echo "$(YELLOW)🔗 链接原 Qt GUI 客户端...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS) $(QT_LDFLAGS)

# ============================================================
# 编译规则
# ============================================================

# 普通源文件编译（不需要 Qt 的文件）
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Qt 相关源文件特殊编译
$(BUILD_DIR)/QtServerGUI.o: QtServerGUI.cpp QtServerGUI.h Server.h
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 Qt 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/QtClientGUI.o: QtClientGUI.cpp QtClientGUI.h
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 Qt 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main_server_qt.o: main_server_qt.cpp QtServerGUI.h Server.h
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 Qt 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main_client_qt.o: main_client_qt.cpp QtClientGUI.h
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 Qt 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# MOC 生成规则
$(BUILD_DIR)/moc_%.cpp: %.h
	@mkdir -p $(BUILD_DIR)
	@echo "$(YELLOW)🎨 生成 MOC 文件 $@...$(RESET)"
	$(MOC) $< -o $@

$(BUILD_DIR)/moc_%.o: $(BUILD_DIR)/moc_%.cpp
	@echo "$(BLUE)🔨 编译 MOC 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# ============================================================
# 目录创建
# ============================================================
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# ============================================================
# 清理
# ============================================================
clean:
	@echo "$(RED)🧹 清理构建文件...$(RESET)"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(GREEN)✅ 清理完成$(RESET)"

rebuild: clean all

# ============================================================
# 运行命令 - FTP 服务器
# ============================================================
run-ftp-server: ftp-server
	@echo "$(GREEN)🚀 启动 FTP 命令行服务器（端口：2100）...$(RESET)"
	@./$(FTP_SERVER_TARGET)

run-ftp-client: ftp-client
	@echo "$(GREEN)🚀 启动 FTP 命令行客户端...$(RESET)"
	@./$(FTP_CLIENT_TARGET)

# ============================================================
# 运行命令 - 原名字 - 年龄服务器
# ============================================================
run-server: server
	@echo "$(GREEN)🚀 启动原命令行服务器（端口：8888）...$(RESET)"
	@./$(SERVER_TARGET)

run-client: client
	@echo "$(GREEN)🚀 启动原命令行客户端...$(RESET)"
	@./$(CLIENT_TARGET)

run-qt-server: qt-server
	@echo "$(GREEN)🚀 启动原 Qt GUI 服务器...$(RESET)"
	@./$(QT_SERVER_TARGET)

run-qt-client: qt-client
	@echo "$(GREEN)🚀 启动原 Qt GUI 客户端...$(RESET)"
	@./$(QT_CLIENT_TARGET)

# ============================================================
# 系统信息
# ============================================================
info:
	@echo "$(YELLOW)========================================$(RESET)"
	@echo "$(YELLOW)       构建系统信息$(RESET)"
	@echo "$(YELLOW)========================================$(RESET)"
	@echo "$(BLUE)编译器:$(RESET) $(CXX)"
	@echo "$(BLUE)编译器版本:$(RESET) $(shell $(CXX) --version | head -n1)"
	@echo "$(BLUE)Qt 头文件:$(RESET) $(QT_CXXFLAGS)"
	@echo "$(BLUE)Qt 库文件:$(RESET) $(QT_LDFLAGS)"
	@echo "$(BLUE)MOC 工具:$(RESET) $(MOC)"
	@echo "$(BLUE)构建目录:$(RESET) $(BUILD_DIR)"
	@echo "$(BLUE)输出目录:$(RESET) $(BIN_DIR)"
	@echo "$(YELLOW)========================================$(RESET)"

# ============================================================
# 帮助信息
# ============================================================
help:
	@echo "$(YELLOW)╔══════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(YELLOW)║           FTP/NameServer Makefile 帮助信息                  ║$(RESET)"
	@echo "$(YELLOW)╠══════════════════════════════════════════════════════════════╣$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)📦 构建命令 - FTP 服务器:$(RESET)                                    $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make              - 构建 FTP 服务器和客户端（默认）  $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make ftp          - 构建 FTP 服务器和客户端          $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make ftp-server   - 构建 FTP 命令行服务器            $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make ftp-client   - 构建 FTP 命令行客户端            $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)📦 构建命令 - 原名字 - 年龄服务器 (保留):$(RESET)                    $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make server       - 构建原命令行服务器               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make client       - 构建原命令行客户端               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-server    - 构建原 Qt GUI 服务器             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-client    - 构建原 Qt GUI 客户端             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-gui       - 构建所有原 Qt GUI 版本           $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)🚀 运行命令 - FTP:$(RESET)                                          $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-ftp-server  - 运行 FTP 服务器 (端口 2100)   $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-ftp-client  - 运行 FTP 客户端              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)🚀 运行命令 - 原名字 - 年龄服务器:$(RESET)                          $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-server   - 运行原命令行服务器 (端口 8888)   $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-client   - 运行原命令行客户端               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-qt-server- 运行原 Qt GUI 服务器             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-qt-client- 运行原 Qt GUI 客户端             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)🧹 维护命令:$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make clean        - 清理构建文件                  $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make rebuild      - 重新构建所有版本              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make info         - 显示系统信息                  $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make help         - 显示此帮助信息                $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)╚══════════════════════════════════════════════════════════════╝$(RESET)"