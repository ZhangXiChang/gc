#include "MainWindow.h"

#include <QApplication>

#include <iostream>
#include <memory>

class App
{
private:
    QApplication *mApplication;
    MainWindow *mMainWindow;

public:
    int exec() { return mApplication->exec(); }

public:
    App(int argc, char *argv[])
    {
        mApplication = new QApplication(argc, argv);
        mMainWindow = new MainWindow();
        mMainWindow->show();
    }
    ~App()
    {
        delete mMainWindow;
        delete mApplication;
    }

    using Ptr = std::shared_ptr<App>;
    static Ptr MakePtr(int argc, char *argv[]) { return std::make_shared<App>(argc, argv); }
};
int main(int argc, char *argv[])
{
    App::Ptr app;
    try
    {
        app = App::MakePtr(argc, argv);
        return app->exec();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
