#include "mainwindow.h"
#include "application.h"

#if __cplusplus < 201103L
#error "C++11 support required!"
#endif

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
