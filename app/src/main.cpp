#include "Window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    auto app = QApplication(argc, argv);
    auto window = Window();
    window.show();
    return app.exec();
}
