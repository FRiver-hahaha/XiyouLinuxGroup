# ============================================================
# NameServer - Makefile
# 所有源文件在同一目录下
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
# 源文件列表
# ============================================================

# 命令行服务器
SERVER_SRCS = Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server.cpp

# 命令行客户端
CLIENT_SRCS = Client.cpp main_client.cpp

# Qt GUI 服务器
QT_SERVER_SRCS = QtServerGUI.cpp Server.cpp Store.cpp CommandHandler.cpp ThreadPool.cpp main_server_qt.cpp

# Qt GUI 客户端
QT_CLIENT_SRCS = QtClientGUI.cpp main_client_qt.cpp

# ============================================================
# 目标文件
# ============================================================
SERVER_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRCS))
CLIENT_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRCS))
QT_SERVER_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(QT_SERVER_SRCS)) $(BUILD_DIR)/moc_QtServerGUI.o
QT_CLIENT_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(QT_CLIENT_SRCS)) $(BUILD_DIR)/moc_QtClientGUI.o

# ============================================================
# 可执行文件
# ============================================================
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
.PHONY: all clean rebuild server client qt-server qt-client qt-gui help info

# 默认目标
all: server client qt-server qt-client

# ============================================================
# 构建目标
# ============================================================

server: $(BUILD_DIR) $(BIN_DIR) $(SERVER_TARGET)
	@echo "$(GREEN)✅ 命令行服务器构建完成: $(SERVER_TARGET)$(RESET)"

client: $(BUILD_DIR) $(BIN_DIR) $(CLIENT_TARGET)
	@echo "$(GREEN)✅ 命令行客户端构建完成: $(CLIENT_TARGET)$(RESET)"

qt-server: $(BUILD_DIR) $(BIN_DIR) $(QT_SERVER_TARGET)
	@echo "$(GREEN)✅ Qt GUI 服务器构建完成: $(QT_SERVER_TARGET)$(RESET)"

qt-client: $(BUILD_DIR) $(BIN_DIR) $(QT_CLIENT_TARGET)
	@echo "$(GREEN)✅ Qt GUI 客户端构建完成: $(QT_CLIENT_TARGET)$(RESET)"

qt-gui: qt-server qt-client

# ============================================================
# 链接规则
# ============================================================

$(SERVER_TARGET): $(SERVER_OBJS)
	@echo "$(YELLOW)🔗 链接命令行服务器...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	@echo "$(YELLOW)🔗 链接命令行客户端...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

$(QT_SERVER_TARGET): $(QT_SERVER_OBJS)
	@echo "$(YELLOW)🔗 链接 Qt GUI 服务器...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS) $(QT_LDFLAGS)

$(QT_CLIENT_TARGET): $(QT_CLIENT_OBJS)
	@echo "$(YELLOW)🔗 链接 Qt GUI 客户端...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS) $(QT_LDFLAGS)

# ============================================================
# 编译规则
# ============================================================

# 普通源文件编译（不需要 Qt 的文件）
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译规则：对于需要 Qt 的源文件，使用通用规则
# 通过判断是否为 Qt 相关文件来决定是否添加 QT_CXXFLAGS
define compile_qt_file
	@mkdir -p $(dir $@)
	@echo "$(BLUE)🔨 编译 Qt 文件 $<...$(RESET)"
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@
endef

# Qt 相关源文件特殊编译 - 使用模式匹配
$(BUILD_DIR)/QtServerGUI.o: QtServerGUI.cpp QtServerGUI.h Server.h
	$(compile_qt_file)

$(BUILD_DIR)/QtClientGUI.o: QtClientGUI.cpp QtClientGUI.h
	$(compile_qt_file)

$(BUILD_DIR)/main_server_qt.o: main_server_qt.cpp QtServerGUI.h Server.h
	$(compile_qt_file)

$(BUILD_DIR)/main_client_qt.o: main_client_qt.cpp QtClientGUI.h
	$(compile_qt_file)

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
# 运行命令
# ============================================================
run-server: server
	@echo "$(GREEN)🚀 启动命令行服务器（端口：8888）...$(RESET)"
	@./$(SERVER_TARGET)

run-client: client
	@echo "$(GREEN)🚀 启动命令行客户端...$(RESET)"
	@./$(CLIENT_TARGET)

run-qt-server: qt-server
	@echo "$(GREEN)🚀 启动 Qt GUI 服务器...$(RESET)"
	@./$(QT_SERVER_TARGET)

run-qt-client: qt-client
	@echo "$(GREEN)🚀 启动 Qt GUI 客户端...$(RESET)"
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
	@echo "$(YELLOW)║              NameServer Makefile 帮助信息                   ║$(RESET)"
	@echo "$(YELLOW)╠══════════════════════════════════════════════════════════════╣$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)📦 构建命令:$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make              - 构建所有版本（默认）           $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make all          - 构建所有版本                   $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make server       - 构建命令行服务器               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make client       - 构建命令行客户端               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-server    - 构建 Qt GUI 服务器             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-client    - 构建 Qt GUI 客户端             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make qt-gui       - 构建所有 Qt GUI 版本           $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)🚀 运行命令:$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-server   - 运行命令行服务器               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-client   - 运行命令行客户端               $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-qt-server- 运行 Qt GUI 服务器             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make run-qt-client- 运行 Qt GUI 客户端             $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  $(GREEN)🧹 维护命令:$(RESET)                                              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make clean        - 清理构建文件                  $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make rebuild      - 重新构建所有版本              $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make info         - 显示系统信息                  $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)║$(RESET)  make help         - 显示此帮助信息                $(YELLOW)║$(RESET)"
	@echo "$(YELLOW)╚══════════════════════════════════════════════════════════════╝$(RESET)"