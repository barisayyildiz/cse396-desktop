#include <QtWidgets/QApplication>
#include <QLocale>

#include <unistd.h>

#include "MainWindow.h"

#include "communication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.showMaximized();

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&] {
        if(scanner->getConnected()) {
            Communication::sendConfig("disconnect");
            close(serverSocket);
            close(configSocket);
            close(broadcastSocket);
            close(calibrationImageSocket);
            close(liveSocket);
        }
    });

    mainWindow.setStyleSheet("background-color: #121212; color: white");

    return a.exec();
}
