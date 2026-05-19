#include "ElaWidgetTools/ElaApplication.h"
#include "ElaWidgetTools/ElaWindow.h"
#include "ElaWidgetTools/ElaPushButton.h"

int main(int argc, char *argv[])
{
    ElaApplication app(argc, argv);
    
    ElaWindow window;
    window.setWindowTitle("ElaWidgetTools 快速测试");
    
    ElaPushButton button("点击我", &window);
    button.setGeometry(150, 100, 120, 40);
    
    window.show();
    
    return app.exec();
}