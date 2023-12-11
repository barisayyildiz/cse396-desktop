#include "calibration.h"
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent> // Add this include for mouse events
#include <QPainter>

Calibration::Calibration(Scanner* scanner, QWidget* parent)
    : QWidget{parent}, isActive(false), counter(0), isDrawn(false)
{
    this->scanner = scanner;
    this->layout = new QGridLayout(this);

    calibrationImage = new QPixmap("images/original/0.jpg");

    imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(*calibrationImage);
    this->layout->addWidget(imageLabel, 0, 0, 1, 4);


    imageLabel->setContentsMargins(0, 0, 0, 0);

    imageLabel->setFixedSize(calibrationImage->size());
    imageLabel->setScaledContents(true);

    // Install event filter for the image label
    imageLabel->installEventFilter(this);

    QWidget* buttonsWidget = new QWidget;
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonsWidget);

    startButton = new QPushButton("Start to Calibrate", this);
    resetButton = new QPushButton("Reset the Calibration", this);
    submitButton = new QPushButton("Submit", this);

    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(submitButton);
    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->layout->addWidget(buttonsWidget, 0, 4, 1, 1);

    resetButton->setEnabled(false);

    connect(startButton, &QPushButton::pressed, [this]() {
        this->isActive = true;
        resetButton->setEnabled(true);
        qDebug() << this->isActive;
    });

    connect(resetButton, &QPushButton::pressed, [this]() {
        this->isActive = false;
        this->counter = 0;
        this->isDrawn = false;
        this->calibrationPolygon.clear();

        // Load the original image and set it as the pixmap
        QPixmap originalImage("images/original/0.jpg");
        imageLabel->setPixmap(originalImage);

        this->update();
        qDebug() << this->isActive;
    });

    connect(submitButton, &QPushButton::pressed, [this]() {
        this->isActive = true;
        qDebug() << this->isActive;

        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.exec();
    });

    this->setLayout(this->layout);
}

bool Calibration::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && this->counter != 4)
    {
        QLabel* imageLabel = qobject_cast<QLabel*>(obj);
        if (imageLabel) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

            qDebug() << "inside event filter";

            // Check if the click is inside the image
            if (mouseEvent->button() == Qt::LeftButton) {
                // Get the coordinates of the click
                int x = mouseEvent->x();
                int y = mouseEvent->y();
                switch(this->counter){
                case 0:
                    topLeft = new QPoint(x, y);
                    break;
                case 1:
                    topRight = new QPoint(x, y);
                    break;
                case 2:
                    bottomRight = new QPoint(x, y);
                    break;
                case 3:
                    bottomLeft = new QPoint(x, y);
                    break;
                }
                qDebug() << "x: " << x << ", y: " << y;
                calibrationPolygon << QPoint(x,y);
                this->counter++;
                //qDebug() << QPoint(x,y);
                //qDebug() << this->counter;
                this->update();
            }
        }
    }

    // Call the base class implementation
    return QWidget::eventFilter(obj, event);
}

void Calibration::paintEvent(QPaintEvent* event)
{
    // Call the base class implementation
    QWidget::paintEvent(event);

    if(calibrationPolygon.size() == 4 && !isActive) {

    }

    // If all four points are collected, draw the shape
    if (calibrationPolygon.size() == 4 && !isDrawn && isActive) {
        // Load the original image
        QPixmap originalImage("images/original/0.jpg");

        // Create a pixmap with the same size as the original image
        QPixmap pixmap(originalImage.size());
        pixmap.fill(Qt::transparent);

        // Create a painter for the pixmap
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QColor(Qt::green));

        // Draw the polygon on the pixmap
        painter.drawPolygon(calibrationPolygon);

        // Create a painter for the original image
        QPainter originalPainter(&originalImage);
        originalPainter.setRenderHint(QPainter::Antialiasing, true);
        originalPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        // Draw the pixmap onto the original image
        originalPainter.drawPixmap(0, 0, pixmap);

        // Set the updated original image as the pixmap to the QLabel
        imageLabel->setPixmap(originalImage);

        isDrawn = true;
    }
}

