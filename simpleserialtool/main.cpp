#include "simpleserialtool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SimpleSerialTool w;

    w.setWindowTitle("SerialTool");
    w.setWindowIcon(QIcon("../../0-image/bower.png"));
    w.show();

    return a.exec();
}
