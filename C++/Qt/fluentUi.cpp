#include "ElaApplication.h"
#include "ElaWindow.h"
#include "ElaPushButton.h"

int main(int argc, char *argv[])
{
    // 初始化 ElaApplication，它会自动处理主题、字体等初始化工作
    ElaApplication app(argc, argv);

    // 创建一个 Fluent 风格的主窗口
    ElaWindow window;
    window.setWindowTitle("ElaWidgetTools 快速测试");

    // 创建一个 Fluent 风格的按钮
    ElaPushButton button("点击我", &window);
    button.setGeometry(150, 100, 120, 40); // 设置按钮位置和大小

    window.show();

    return app.exec();
}