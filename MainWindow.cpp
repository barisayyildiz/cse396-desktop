#include "MainWindow.h"
#include "footer.h"
#include "scanner.h"

#include "Model.h"
#include <QTextStream>
#include <QFileDialog>
#include <QAction>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>
#include <QListWidgetItem>

// charts
#include "pointcloud.h"
#include "scannedpoints.h"

// communication and multithreading
#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

// signal handling
#include <signal.h>
#include <cstdlib>

#define DATA_PORT 5000
#define CONFIG_PORT 4000

#define BUFFER_SIZE 1024

int dataSocket;
int configSocket;

void signalCallbackHandler(int signum) {
    std::cout << "Caught signal " << signum << std::endl;
    close(dataSocket);
    close(configSocket);
    exit(signum);
}

int sendConfig() {
    qDebug() << "Gonna send some configurations";
    configSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(configSocket == -1) {
        qDebug() << "Error creating client socket";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(CONFIG_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(configSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server." << std::endl;
        return 1;
    }

    // Send data to the server
    std::string dataToSend = "configurations changed...";
    if (send(configSocket, dataToSend.c_str(), dataToSend.size(), 0) == -1) {
        std::cerr << "Error sending data." << std::endl;
        return 1;
    }

    close(configSocket);
}


int readData(int& serverSocket, ScannedPoints* scannedPoints, PointCloud* pointCloud, Scanner* scanner) {
    qDebug() << "Start of the socket thread";
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        qDebug() << "Error creating server socket.";
        return 1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DATA_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket to the address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        qDebug() << "Error binding server socket.";
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        qDebug() << "Error listening for incoming connections.";
        return 1;
    }

    qDebug() << "Server is listening for incoming connections...";

    int clientSocket;
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == -1) {
        qDebug() << "Error accepting the connection.";
        return 1;
    }

    qDebug() << "waiting for RUNNING command";
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    recv(clientSocket, buffer, sizeof(buffer), 0);
    qDebug() << "initial command: " << buffer;

    scanner->setScannerState(ScannerState::RUNNING);
    scanner->updateScanner();

    scanner->startTimer();

    while (true) {
        // Accept incoming connections
        qDebug() << "accepting...";

        memset(buffer, '\0', BUFFER_SIZE);
        sprintf(buffer, "%s", "OK");
        send(clientSocket, buffer, sizeof(buffer), 0);

        memset(buffer, '\0', BUFFER_SIZE);
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(clientSocket);
            break;
        }

        if(strcmp(buffer, "FINISHED") == 0) {
            close(clientSocket);
            break;
        }

        char* token;
        token = strtok(buffer, " ");

        //qDebug() << "token: " << token;
        int round = atoi(token);
        qDebug() << "currentStep: " << round;
        scanner->setCurrentStep(round);
        token = strtok(nullptr, " ");
        scanner->setHorizontalPrecision(atoi(token));
        //qDebug() << "round number: " << round;

        memset(buffer, '\0', BUFFER_SIZE);
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(clientSocket);
            break;
        }

        int numOfScannedPoints = atoi(buffer);
        //qDebug() << "number of scanned points : " << numOfScannedPoints;

        scannedPoints->addNewDataPoint(numOfScannedPoints);
        scanner->setNumberOfPointsScanned(scanner->getNumberOfPointsScanned() + numOfScannedPoints);

        /*ScannerState scannerState;
        int verticalPrecision;
        int numberOfPointsScanned;*/

        for(int i=0; i<numOfScannedPoints; i++) {
            memset(buffer, '\0', BUFFER_SIZE);
            recv(clientSocket, buffer, sizeof(buffer), 0);
            send(clientSocket, buffer, sizeof(buffer), 0);
            //qDebug() << "buffer: " << buffer;

            qDebug() << buffer;
            double x, y, z;

            QTextStream stream(buffer);
            stream >> x >> y >> z;

            std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
            //qDebug() << "x: " << x << ", y: " << y << ", z: " << z;
            pointCloud->addNewDataPoint(x, y, z);
        }

        if(round%5 == 0) {
            pointCloud->reRenderGraph();
        }
        scanner->updateScanner();
        //qDebug() << "----------------";
        //chartView->addNewDataPoint(numOfScannedPoints);
    }
    pointCloud->reRenderGraph();
    qDebug() << "end of readdata thread" ;

    scanner->stopTimer();
    scanner->updateScanner();

    // Close the client socket
    close(clientSocket);

    // Close the server socket (usually never reached in this example)
    close(serverSocket);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

	ui.showEdgesBtn->setCheckable(true);

    auto exportOBJAction = new QAction(QString("OBJ"), (QObject*)ui.exportBtn->menu());
    auto exportSTLAction = new QAction(QString("STL"), (QObject*)ui.exportBtn->menu());

    connect(exportOBJAction, &QAction::triggered, [=]() {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        QDir::homePath(),  // Starting directory
                                                        "OBJ Files (*.obj)",
                                                        nullptr,
                                                        QFileDialog::DontUseNativeDialog);
        ui.openGLWidget->model->ExportModel(fileName.toStdString(), ExportFormat::OBJ);
    });

    QObject::connect(exportSTLAction, &QAction::triggered, [=]() {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        QDir::homePath(),  // Starting directory
                                                        "STL Files (*.stl)",
                                                        nullptr,
                                                        QFileDialog::DontUseNativeDialog);
        ui.openGLWidget->model->ExportModel(fileName.toStdString(), ExportFormat::STL);
    });

    QListWidgetItem* headerItem = new QListWidgetItem("Stats");
    QFont font = headerItem->font();
    font.setPointSize(20); // Set a custom font size for the header item
    headerItem->setFont(font);
    ui.listWidget->addItem(headerItem);
    ui.listWidget->setSpacing(10);
    ui.listWidget->addItem(new QListWidgetItem("Additional text"));
    ui.listWidget->addItem(new QListWidgetItem("Additional text"));
    ui.listWidget->addItem(new QListWidgetItem("Additional text"));
    ui.listWidget->addItem(new QListWidgetItem("Additional text"));

	ui.exportBtn->addAction(exportOBJAction);
	ui.exportBtn->addAction(exportSTLAction);

	connect(ui.exportBtn, &QToolButton::clicked, [this]() {
		ui.exportBtn->showMenu();
		});

    connect(ui.colorPickerWidget, &ColorPickerWidget::colorSelected, ui.openGLWidget, &OpenGLWidget::SetBackgroundColor);

	connect(ui.imagePickerWidget, &ImagePickerWidget::textureSelected, ui.openGLWidget, &OpenGLWidget::SetModelTexture);

	connect(ui.showEdgesBtn, &QPushButton::toggled, ui.openGLWidget, [this](bool checked) {

        ui.openGLWidget->ToggleWireframe();

        });

    // 2d chart
    ScannedPoints *scannedPoints = new ScannedPoints();
    scannedPoints->addNewDataPoint(10);
    scannedPoints->addNewDataPoint(20);
    QPushButton *button = new QPushButton("Add new data point");
    ui.chartsVLayout->addWidget(scannedPoints);
    ui.chartsVLayout->addWidget(button);

    Scanner* scanner = new Scanner();

    Footer* footer = new Footer(scanner);
    ui.chartsVLayout->addLayout(footer);

    QObject::connect(scanner, &Scanner::updateScannerSignal, footer, &Footer::footerUpdated);

    // 3d scatter graph
    PointCloud *pointCloud = new PointCloud();
    for(int i=0; i<0; i++) {
        pointCloud->addNewDataPoint(rand()%100, rand()%100, rand()%100);
    }
    auto container = QWidget::createWindowContainer(pointCloud);
    container->setMinimumSize(QSize(200, 200));
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    ui.pointCloudVLayout->addWidget(container);

    // new thread
    std::thread t1(readData, std::ref(dataSocket), scannedPoints, pointCloud, scanner);
    t1.detach();

    QObject::connect(button, &QPushButton::clicked, [&]() {
        qDebug() << "clicked...";
        //chartView.addNewDataPoint(0.7);
        sendConfig();
    });

    /*QObject::connect(pointCloud->series, &QLineSeries::pointAdded, [&] (int index) {
        qDebug() << "index at " << index;
    });
*/

}

MainWindow::~MainWindow()
{}
