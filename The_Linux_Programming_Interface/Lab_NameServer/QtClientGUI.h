#ifndef QTCLIENTGUI_H
#define QTCLIENTGUI_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QListWidget>
#include <QHeaderView>      
#include <QScrollBar>      
#include <QTimer>  

// Qt客户端GUI类
class QtClientGUI : public QMainWindow {
    Q_OBJECT

private:
    QTcpSocket* socket;         // TCP套接字
    bool isConnected;           // 连接状态
    
    // UI组件
    QTextEdit* outputDisplay;   // 输出显示区域
    QLineEdit* commandInput;    // 命令输入框
    QLineEdit* serverAddressEdit;// 服务器地址输入框
    QLineEdit* serverPortEdit;   // 服务器端口输入框
    QLabel* statusLabel;        // 状态标签
    QPushButton* connectButton; // 连接按钮
    QPushButton* disconnectButton;// 断开按钮
    QPushButton* sendButton;    // 发送按钮
    QPushButton* clearButton;   // 清除按钮
    QListWidget* quickCommandList; // 快捷命令列表
    QTableWidget* dataTable;    // 数据显示表格
    
    // 辅助方法
    void setupUI();             // 初始化UI
    void appendOutput(const QString& msg, const QString& type = "info");
    void updateDataTable(const QString& response);  // 更新数据表格
    void loadQuickCommands();   // 加载快捷命令
    
private slots:
    void onConnect();           // 连接服务器
    void onDisconnect();        // 断开连接
    void onSendCommand();       // 发送命令
    void onClearOutput();       // 清除输出
    void onQuickCommandClicked(QListWidgetItem* item);  // 快捷命令点击
    void onConnected();         // 连接成功槽函数
    void onDisconnected();      // 断开连接槽函数
    void onReadyRead();         // 收到数据槽函数
    void onError(QAbstractSocket::SocketError error);  // 错误槽函数
    
public:
    QtClientGUI(QWidget* parent = nullptr);
    ~QtClientGUI();
};

#endif