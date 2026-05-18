#ifndef QTSERVERGUI_H
#define QTSERVERGUI_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>
#include <memory>
#include "Server.h"

// Qt服务器管理界面类
class QtServerGUI : public QMainWindow {
    Q_OBJECT

private:
    // 服务器核心对象
    std::unique_ptr<Server> server;
    
    // UI组件
    QTextEdit* logDisplay;      // 日志显示区域
    QTableWidget* clientTable;  // 客户端列表表格
    QLabel* statusLabel;        // 状态标签
    QLabel* portLabel;          // 端口标签
    QLabel* clientCountLabel;   // 客户端数量标签
    QPushButton* startButton;   // 启动服务器按钮
    QPushButton* stopButton;    // 停止服务器按钮
    QPushButton* clearLogButton;// 清除日志按钮
    
    QTimer* updateTimer;        // 定时更新UI的定时器
    
    // 辅助方法
    void setupUI();             // 初始化UI
    void appendLog(const QString& msg);  // 添加日志
    void updateClientList();    // 更新客户端列表
    
private slots:
    void onStartServer();       // 启动服务器
    void onStopServer();        // 停止服务器
    void onClearLog();          // 清除日志
    void onUpdateUI();          // 定时更新UI
    
public:
    QtServerGUI(QWidget* parent = nullptr);
    ~QtServerGUI();
};

#endif