#include <QtWidgets/QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.showMaximized();

    mainWindow.setStyleSheet("background-color: #121212; color: white");

    return a.exec();
}
