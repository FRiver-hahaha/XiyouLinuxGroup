#include "QtServerGUI.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QScrollBar> 

QtServerGUI::QtServerGUI(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    
    // 创建定时器，每1秒更新一次UI
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &QtServerGUI::onUpdateUI);
    updateTimer->start(1000);
}

QtServerGUI::~QtServerGUI() {
    if (server && server->isRunning()) {
        server->stop();
    }
}

void QtServerGUI::setupUI() {
    setWindowTitle("键值存储服务器 - 管理界面");
    setMinimumSize(900, 700);
    
    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // ========== 控制面板区域 ==========
    QGroupBox* controlGroup = new QGroupBox("服务器控制");
    QHBoxLayout* controlLayout = new QHBoxLayout();
    
    startButton = new QPushButton("启动服务器");
    startButton->setStyleSheet("QPushButton { background-color: green; color: white; font-size: 14px; padding: 5px; }");
    stopButton = new QPushButton("停止服务器");
    stopButton->setEnabled(false);
    stopButton->setStyleSheet("QPushButton { background-color: red; color: white; font-size: 14px; padding: 5px; }");
    clearLogButton = new QPushButton("清除日志");
    
    statusLabel = new QLabel("状态: 未启动");
    portLabel = new QLabel("端口: 8888");
    clientCountLabel = new QLabel("客户端数: 0");
    
    controlLayout->addWidget(startButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(clearLogButton);
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);
    controlLayout->addWidget(portLabel);
    controlLayout->addWidget(clientCountLabel);
    
    controlGroup->setLayout(controlLayout);
    mainLayout->addWidget(controlGroup);
    
    // ========== 客户端列表区域 ==========
    QGroupBox* clientGroup = new QGroupBox("在线客户端");
    QVBoxLayout* clientLayout = new QVBoxLayout();
    
    clientTable = new QTableWidget();
    clientTable->setColumnCount(2);
    clientTable->setHorizontalHeaderLabels({"客户端ID", "Socket描述符"});
    clientTable->horizontalHeader()->setStretchLastSection(true);
    clientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    clientLayout->addWidget(clientTable);
    clientGroup->setLayout(clientLayout);
    mainLayout->addWidget(clientGroup);
    
    // ========== 日志显示区域 ==========
    QGroupBox* logGroup = new QGroupBox("服务器日志");
    QVBoxLayout* logLayout = new QVBoxLayout();
    
    logDisplay = new QTextEdit();
    logDisplay->setReadOnly(true);
    logDisplay->setFont(QFont("Consolas", 10));
    
    logLayout->addWidget(logDisplay);
    logGroup->setLayout(logLayout);
    mainLayout->addWidget(logGroup);
    
    // 连接信号槽
    connect(startButton, &QPushButton::clicked, this, &QtServerGUI::onStartServer);
    connect(stopButton, &QPushButton::clicked, this, &QtServerGUI::onStopServer);
    connect(clearLogButton, &QPushButton::clicked, this, &QtServerGUI::onClearLog);
    
    appendLog("服务器管理界面已启动");
}

void QtServerGUI::appendLog(const QString& msg) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logDisplay->append(QString("[%1] %2").arg(timestamp, msg));
}

void QtServerGUI::onStartServer() {
    if (server && server->isRunning()) {
        appendLog("服务器已经在运行中");
        return;
    }
    
    // 创建服务器实例
    server = std::make_unique<Server>(8888);
    
    // 设置日志回调
    server->setLogCallback([this](const string& msg) {
        // Qt的信号槽需要在线程安全的方式下调用
        QMetaObject::invokeMethod(this, [this, msg]() {
            appendLog(QString::fromStdString(msg));
        });
    });
    
    // 设置客户端数量变化回调
    server->setClientCountCallback([this](int count) {
        QMetaObject::invokeMethod(this, [this, count]() {
            clientCountLabel->setText(QString("客户端数: %1").arg(count));
            updateClientList();
        });
    });
    
    // 设置命令执行回调
    server->setCommandCallback([this](const string& cmd, const string& response) {
        QMetaObject::invokeMethod(this, [this, cmd, response]() {
            appendLog(QString("命令: %1 -> 响应: %2").arg(
                QString::fromStdString(cmd),
                QString::fromStdString(response)
            ));
        });
    });
    
    // 启动服务器
    if (server->start()) {
        // 在新线程中运行服务器
        std::thread serverThread([this]() {
            server->run();
        });
        serverThread.detach();
        
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        statusLabel->setText("状态: 运行中");
        statusLabel->setStyleSheet("QLabel { color: green; }");
        appendLog("服务器已启动，监听端口 8888");
    } else {
        appendLog("服务器启动失败");
        QMessageBox::critical(this, "错误", "无法启动服务器，请检查端口是否被占用");
        server.reset();
    }
}

void QtServerGUI::onStopServer() {
    if (server && server->isRunning()) {
        server->stop();
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        statusLabel->setText("状态: 已停止");
        statusLabel->setStyleSheet("QLabel { color: red; }");
        appendLog("服务器已停止");
        
        // 清空客户端列表
        clientTable->setRowCount(0);
        clientCountLabel->setText("客户端数: 0");
    }
}

void QtServerGUI::onClearLog() {
    logDisplay->clear();
    appendLog("日志已清除");
}

void QtServerGUI::onUpdateUI() {
    if (server && server->isRunning()) {
        int clientCount = server->getClientCount();
        if (clientCountLabel->text() != QString("客户端数: %1").arg(clientCount)) {
            clientCountLabel->setText(QString("客户端数: %1").arg(clientCount));
            updateClientList();
        }
    }
}

void QtServerGUI::updateClientList() {
    if (!server || !server->isRunning()) {
        clientTable->setRowCount(0);
        return;
    }
    
    // 由于无法直接获取客户端详细信息，这里仅显示数量
    // 实际项目中可以在Server类中添加获取客户端列表的方法
    int clientCount = server->getClientCount();
    clientTable->setRowCount(clientCount);
    
    for (int i = 0; i < clientCount; ++i) {
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem* fdItem = new QTableWidgetItem("客户端 " + QString::number(i + 1));
        clientTable->setItem(i, 0, idItem);
        clientTable->setItem(i, 1, fdItem);
    }
}