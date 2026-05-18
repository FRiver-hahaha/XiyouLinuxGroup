#include "QtClientGUI.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>      
#include <QScrollBar>       
#include <QTimer> 

QtClientGUI::QtClientGUI(QWidget* parent) 
    : QMainWindow(parent), socket(nullptr), isConnected(false) {
    setupUI();
    loadQuickCommands();
}

QtClientGUI::~QtClientGUI() {
    if (socket && isConnected) {
        socket->disconnectFromHost();
    }
}

void QtClientGUI::setupUI() {
    setWindowTitle("键值存储客户端 - 图形界面");
    setMinimumSize(1000, 700);
    
    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // ========== 连接面板 ==========
    QGroupBox* connectionGroup = new QGroupBox("服务器连接");
    QHBoxLayout* connectionLayout = new QHBoxLayout();
    
    connectionLayout->addWidget(new QLabel("服务器地址:"));
    serverAddressEdit = new QLineEdit("127.0.0.1");
    serverAddressEdit->setMaximumWidth(150);
    connectionLayout->addWidget(serverAddressEdit);
    
    connectionLayout->addWidget(new QLabel("端口:"));
    serverPortEdit = new QLineEdit("8888");
    serverPortEdit->setMaximumWidth(80);
    connectionLayout->addWidget(serverPortEdit);
    
    connectButton = new QPushButton("连接");
    connectButton->setStyleSheet("QPushButton { background-color: green; color: white; }");
    disconnectButton = new QPushButton("断开");
    disconnectButton->setEnabled(false);
    disconnectButton->setStyleSheet("QPushButton { background-color: red; color: white; }");
    
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(disconnectButton);
    connectionLayout->addStretch();
    
    statusLabel = new QLabel("状态: 未连接");
    statusLabel->setStyleSheet("QLabel { color: red; }");
    connectionLayout->addWidget(statusLabel);
    
    connectionGroup->setLayout(connectionLayout);
    mainLayout->addWidget(connectionGroup);
    
    // ========== 数据显示区域（表格） ==========
    QGroupBox* dataGroup = new QGroupBox("数据浏览");
    QVBoxLayout* dataLayout = new QVBoxLayout();
    
    dataTable = new QTableWidget();
    dataTable->setColumnCount(2);
    dataTable->setHorizontalHeaderLabels({"名称", "年龄"});
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    dataLayout->addWidget(dataTable);
    dataGroup->setLayout(dataLayout);
    mainLayout->addWidget(dataGroup);
    
    // ========== 命令输入区域 ==========
    QGroupBox* commandGroup = new QGroupBox("命令输入");
    QVBoxLayout* commandLayout = new QVBoxLayout();
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    commandInput = new QLineEdit();
    commandInput->setPlaceholderText("输入命令 (例如: SET 张三 25, GET 张三, DEL 张三, LIST, HELP)");
    sendButton = new QPushButton("发送");
    sendButton->setEnabled(false);
    clearButton = new QPushButton("清除输出");
    
    inputLayout->addWidget(commandInput);
    inputLayout->addWidget(sendButton);
    inputLayout->addWidget(clearButton);
    commandLayout->addLayout(inputLayout);
    
    commandGroup->setLayout(commandLayout);
    mainLayout->addWidget(commandGroup);
    
    // ========== 快捷命令区域 ==========
    QGroupBox* quickGroup = new QGroupBox("快捷命令");
    QHBoxLayout* quickLayout = new QHBoxLayout();
    
    quickCommandList = new QListWidget();
    quickCommandList->setMaximumHeight(120);
    quickCommandList->setFlow(QListWidget::LeftToRight);
    quickCommandList->setWrapping(true);
    
    quickLayout->addWidget(quickCommandList);
    quickGroup->setLayout(quickLayout);
    mainLayout->addWidget(quickGroup);
    
    // ========== 输出显示区域 ==========
    QGroupBox* outputGroup = new QGroupBox("服务器响应");
    QVBoxLayout* outputLayout = new QVBoxLayout();
    
    outputDisplay = new QTextEdit();
    outputDisplay->setReadOnly(true);
    outputDisplay->setFont(QFont("Consolas", 10));
    
    outputLayout->addWidget(outputDisplay);
    outputGroup->setLayout(outputLayout);
    mainLayout->addWidget(outputGroup);
    
    // 连接信号槽
    connect(connectButton, &QPushButton::clicked, this, &QtClientGUI::onConnect);
    connect(disconnectButton, &QPushButton::clicked, this, &QtClientGUI::onDisconnect);
    connect(sendButton, &QPushButton::clicked, this, &QtClientGUI::onSendCommand);
    connect(clearButton, &QPushButton::clicked, this, &QtClientGUI::onClearOutput);
    connect(commandInput, &QLineEdit::returnPressed, this, &QtClientGUI::onSendCommand);
    connect(quickCommandList, &QListWidget::itemClicked, this, &QtClientGUI::onQuickCommandClicked);
    
    appendOutput("客户端已启动，请输入服务器地址和端口进行连接");
}

void QtClientGUI::appendOutput(const QString& msg, const QString& type) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix;
    
    if (type == "send") {
        prefix = ">>>";
    } else if (type == "recv") {
        prefix = "<<<";
    } else if (type == "error") {
        prefix = "!!!";
    } else {
        prefix = "***";
    }
    
    outputDisplay->append(QString("[%1] %2 %3").arg(timestamp, prefix, msg));
    // 自动滚动到底部
    outputDisplay->verticalScrollBar()->setValue(outputDisplay->verticalScrollBar()->maximum());
}

void QtClientGUI::updateDataTable(const QString& response) {
    // 解析LIST命令的响应，更新表格
    if (response.startsWith("LIST:")) {
        dataTable->setRowCount(0);
        QStringList lines = response.split('\n');
        int row = 0;
        
        for (const QString& line : lines) {
            if (line.contains("<=>")) {
                QStringList parts = line.trimmed().split("<=>");
                if (parts.size() >= 2) {
                    dataTable->insertRow(row);
                    dataTable->setItem(row, 0, new QTableWidgetItem(parts[0].trimmed()));
                    dataTable->setItem(row, 1, new QTableWidgetItem(parts[1].trimmed()));
                    row++;
                }
            }
        }
        appendOutput(QString("已更新数据表格，共 %1 条记录").arg(row), "info");
    }
}

void QtClientGUI::loadQuickCommands() {
    QStringList commands = {
        "HELP", "LIST", "SET 张三 25", "SET 李四 30",
        "SET 王五 35", "GET 张三", "DEL 张三", "QUIT"
    };
    quickCommandList->addItems(commands);
}

void QtClientGUI::onConnect() {
    if (socket) {
        delete socket;
        socket = nullptr;
    }
    
    QString address = serverAddressEdit->text();
    int port = serverPortEdit->text().toInt();
    
    socket = new QTcpSocket(this);
    
    // 连接信号槽
    connect(socket, &QTcpSocket::connected, this, &QtClientGUI::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &QtClientGUI::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &QtClientGUI::onReadyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), 
            this, &QtClientGUI::onError);
    
    appendOutput(QString("正在连接到 %1:%2...").arg(address, QString::number(port)));
    socket->connectToHost(address, port);
}

void QtClientGUI::onDisconnect() {
    if (socket && isConnected) {
        // 发送QUIT命令优雅退出
        socket->write("QUIT\n");
        socket->flush();
        socket->disconnectFromHost();
    } else if (socket) {
        socket->disconnectFromHost();
    }
}

void QtClientGUI::onSendCommand() {
    QString command = commandInput->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    
    if (!isConnected || !socket) {
        appendOutput("未连接到服务器，请先连接", "error");
        return;
    }
    
    appendOutput(command, "send");
    socket->write((command + "\n").toUtf8());
    socket->flush();
    
    commandInput->clear();
    
    // 如果是QUIT命令，自动断开连接
    if (command.toUpper() == "QUIT") {
        appendOutput("正在断开连接...", "info");
        QTimer::singleShot(500, this, &QtClientGUI::onDisconnect);
    }
}

void QtClientGUI::onClearOutput() {
    outputDisplay->clear();
    appendOutput("输出已清除");
}

void QtClientGUI::onQuickCommandClicked(QListWidgetItem* item) {
    commandInput->setText(item->text());
    onSendCommand();
}

void QtClientGUI::onConnected() {
    isConnected = true;
    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);
    sendButton->setEnabled(true);
    statusLabel->setText("状态: 已连接");
    statusLabel->setStyleSheet("QLabel { color: green; }");
    appendOutput("已成功连接到服务器", "info");
}

void QtClientGUI::onDisconnected() {
    isConnected = false;
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    sendButton->setEnabled(false);
    statusLabel->setText("状态: 未连接");
    statusLabel->setStyleSheet("QLabel { color: red; }");
    appendOutput("已断开连接", "info");
    
    if (socket) {
        socket->deleteLater();
        socket = nullptr;
    }
}

void QtClientGUI::onReadyRead() {
    while (socket && socket->canReadLine()) {
        QByteArray data = socket->readLine();
        QString response = QString::fromUtf8(data).trimmed();
        
        if (!response.isEmpty()) {
            appendOutput(response, "recv");
            updateDataTable(response);
        }
    }
}

void QtClientGUI::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);  // 明确标记参数未使用
    QString errorMsg = socket->errorString();
    appendOutput(QString("连接错误: %1").arg(errorMsg), "error");
    QMessageBox::critical(this, "连接错误", errorMsg);
    onDisconnected();
}