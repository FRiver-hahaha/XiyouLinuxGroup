#include <QApplication>
#include "QtClientGUI.h"

// Qt客户端版本的main函数
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QtClientGUI window;
    window.show();
    
    return app.exec();
}