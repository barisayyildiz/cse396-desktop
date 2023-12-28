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

// images
#include "capturedimages.h"

// calibration
#include "calibration.h"

// connection
#include "connection.h"

// communication
#include "communication.h"

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
#include <fcntl.h>

// signal handling
#include <signal.h>
#include <cstdlib>

#include "qopenglwidget.h"
#include "global.h"

#include "communication.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

	ui.showEdgesBtn->setCheckable(true);

    Scanner* scanner = new Scanner();

    auto exportOBJAction = new QAction(QString("OBJ"), (QObject*)ui.exportBtn->menu());
    auto exportSTLAction = new QAction(QString("STL"), (QObject*)ui.exportBtn->menu());

    connect(exportOBJAction, &QAction::triggered, [=]() {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        ui.fileNameLabel->text(),  // Starting directory
                                                        "OBJ Files (*.obj)",
                                                        nullptr,
                                                        QFileDialog::DontUseNativeDialog);
        ui.openGLWidget->model->ExportModel(fileName.toStdString(), ExportFormat::OBJ);
    });

    QObject::connect(exportSTLAction, &QAction::triggered, [=]() {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        ui.fileNameLabel->text(),  // Starting directory
                                                        "STL Files (*.stl)",
                                                        nullptr,
                                                        QFileDialog::DontUseNativeDialog);
        fileName = fileName.isEmpty() ? QDir::homePath() + "/" + "defaultFileName" : fileName;
        ui.openGLWidget->model->ExportModel(fileName.toStdString(), ExportFormat::STL);
    });

    QListWidgetItem* headerItem = new QListWidgetItem("Stats");
    QFont font = headerItem->font();
    font.setPointSize(20); // Set a custom font size for the header item
    headerItem->setFont(font);
    ui.listWidget->addItem(headerItem);
    ui.listWidget->setSpacing(10);

	ui.exportBtn->addAction(exportOBJAction);
	ui.exportBtn->addAction(exportSTLAction);

    // sliders
    const QVector<int> horizontalPrecisionValues = {2, 4, 8, 16, 32, 64, 128, 256, 512};
    ui.horizontalSlider->setRange(0, 8);  // 0 corresponds to 2, 8 corresponds to 512
    ui.horizontalSlider->setTickPosition(QSlider::TicksBelow);
    ui.horizontalSlider->setTickInterval(1);

    int index = horizontalPrecisionValues.indexOf(scanner->getHorizontalPrecision());
    ui.horizontalSlider->setValue(index);
    ui.horizontalPrecisionLabel->setText(QString::number(scanner->getHorizontalPrecision()));

    ui.verticalSlider->setValue(scanner->getVerticalPrecision());
    ui.verticalPrecisionLabel->setText(QString::number(scanner->getVerticalPrecision()) + "%");

    connect(ui.horizontalSlider, &QSlider::valueChanged, this, [this, scanner, horizontalPrecisionValues](int value) {
        int horizontalPrecision = horizontalPrecisionValues.at(value);
        scanner->setHorizontalPrecision(horizontalPrecision);
        ui.horizontalPrecisionLabel->setText(QString::number(scanner->getHorizontalPrecision()));
    });
    connect(ui.verticalSlider, &QSlider::valueChanged, scanner, [this, scanner](int value) {
        scanner->setVerticalPrecision(value);
        ui.verticalPrecisionLabel->setText(QString::number(scanner->getVerticalPrecision()) + "%");
    });

	connect(ui.exportBtn, &QToolButton::clicked, [this]() {
		ui.exportBtn->showMenu();
    });

    connect(ui.colorPickerWidget, &ColorPickerWidget::colorSelected, ui.openGLWidget, &OpenGLWidget::SetBackgroundColor);

	connect(ui.imagePickerWidget, &ImagePickerWidget::textureSelected, ui.openGLWidget, &OpenGLWidget::SetModelTexture);

	connect(ui.showEdgesBtn, &QPushButton::toggled, ui.openGLWidget, [this](bool checked) {
        ui.openGLWidget->ToggleWireframe();
    });

    connect(ui.openGLWidget, &OpenGLWidget::modelUploaded , [this]() {
        QString stats = QString("Total number of nodes: %1").arg(this->ui.openGLWidget->model->getTotalNumberOfNodes());
        this->ui.listWidget->addItem(stats);
        stats = QString("Total number of meshes: %1").arg(this->ui.openGLWidget->model->getTotalNumberOfMeshes());
        this->ui.listWidget->addItem(stats);
        stats = QString("Total number of vertices: %1").arg(this->ui.openGLWidget->model->getTotalNumberOfVertices());
        this->ui.listWidget->addItem(stats);
        stats = QString("Total number of faces: %1").arg(this->ui.openGLWidget->model->getTotalNumberOfFaces());
        this->ui.listWidget->addItem(stats);
    });

    connect(ui.pushButton, &QPushButton::clicked, [this, horizontalPrecisionValues, scanner]() {
        if (scanner->getConnected()) {
            int horizontalPrecision = horizontalPrecisionValues[ui.horizontalSlider->value()];
            int verticalPrecision = ui.verticalSlider->value();
            Communication::sendConfig(horizontalPrecision, verticalPrecision);
            QMessageBox msgBox;
            msgBox.setText("Scanner precisions have been updated");
            msgBox.exec();
        } else {
            QMessageBox::critical(this, "Scanner not connected", "Please connect to scanner");
        }
    });

    // 2d chart
    ScannedPoints *scannedPoints = new ScannedPoints();
    ui.chartsVLayout->addWidget(scannedPoints);

    Footer* footer = new Footer(scanner);
    ui.chartsVLayout->addWidget(footer);

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

    CapturedImages *capturedImages = new CapturedImages(scanner);
    ui.imagesVLayout->addWidget(capturedImages);

    Calibration *calibration = new Calibration(scanner);
    ui.calibrationVLayout->addWidget(calibration);

    Connection *connection = new Connection(scanner);
    ui.connectionVLayout->addWidget(connection);

    connect(connection, &Connection::scannerStateReceived, [this, scanner, horizontalPrecisionValues] {
        int index = horizontalPrecisionValues.indexOf(scanner->getHorizontalPrecision());
        ui.horizontalSlider->setValue(index);
        ui.horizontalPrecisionLabel->setText(QString::number(scanner->getHorizontalPrecision()));
        ui.verticalPrecisionLabel->setText(QString::number(scanner->getVerticalPrecision()) + "%");
        scanner->updateScanner();
    });

    QObject::connect(scanner, &Scanner::updateScannerSignal, capturedImages, &CapturedImages::capturedImagesUpdated);

    Communication::scannedPoints = scannedPoints;
    Communication::pointCloud = pointCloud;
    Communication::scanner = scanner;
    Communication::openGlWidget = ui.openGLWidget;

    std::thread tClient(Communication::readFromScanner);
    tClient.detach();

    std::thread tCalibrationImage(Communication::readImageForCalibration);
    tCalibrationImage.detach();

    this->showMaximized(); // Maximize the window first

    // Now set the fixed size to control the initial size when maximized
    //this->setFixedSize(800, 600);

}

MainWindow::~MainWindow()
{}
