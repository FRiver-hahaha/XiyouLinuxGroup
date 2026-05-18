#include <QApplication>
#include "QtServerGUI.h"

// Qt服务器版本的main函数
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QtServerGUI window;
    window.show();
    
    return app.exec();
}