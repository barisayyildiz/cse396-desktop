#include "calibration.h"
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent> // Add this include for mouse events
#include <QPainter>
#include "communication.h"

Calibration::Calibration(Scanner* scanner, QWidget* parent)
    : QWidget{parent}
{
    this->isSubmitted = false;
    this->scanner = scanner;
    this->layout = new QHBoxLayout(this);

    calibrationImage = new QPixmap("received_files/calibration.jpg");

    imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(*calibrationImage);
    //this->layout->addWidget(imageLabel, 0, 0, 1, 4);
    this->layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->layout->addWidget(imageLabel);


    imageLabel->setContentsMargins(0, 0, 0, 0);

    imageLabel->setFixedSize(calibrationImage->size());
    imageLabel->setScaledContents(true);

    // Install event filter for the image label
    imageLabel->installEventFilter(this);

    QWidget* buttonsWidget = new QWidget;
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonsWidget);

    fetchImageButton = new QPushButton("Fetch an Image for Calibration", this);
    resetButton = new QPushButton("Reset the Calibration", this);
    submitButton = new QPushButton("Submit", this);

    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    buttonLayout->addWidget(fetchImageButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(submitButton);
    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    //this->layout->addWidget(buttonsWidget, 0, 4, 1, 1);
    this->layout->addWidget(buttonsWidget);
    this->layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(fetchImageButton, &QPushButton::pressed, [this]() {

    });

    connect(resetButton, &QPushButton::pressed, [this]() {
        this->calibrationPolygon.clear();
        this->isSubmitted = false;

        // Load the original image and set it as the pixmap
        QPixmap originalImage("received_files/calibration.jpg");
        imageLabel->setPixmap(originalImage);

        this->update();
    });

    connect(submitButton, &QPushButton::pressed, [this]() {
        if(this->calibrationPolygon.size() < 2) {
            QMessageBox::critical(this, "Invalid Request", "Please select 2 points");
        } else {
            if(this->scanner->getConnected()) {
                this->isSubmitted = true;
                this->update();
                Communication::sendConfig(this->calibrationPolygon);
                QMessageBox msgBox;
                msgBox.setText("Scanner calibrations have been updated");
                msgBox.exec();
            } else {
                QMessageBox::critical(this, "Scanner not connected", "Please connect to scanner");
            }
        }
    });

    this->setLayout(this->layout);
}

bool Calibration::eventFilter(QObject* obj, QEvent* event)
{
    if (calibrationPolygon.size() == 2) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && calibrationPolygon.boundingRect().contains(mouseEvent->pos())) {
                isDragging = true;
                dragStartPos = mouseEvent->pos();
                originalRect = calibrationPolygon.boundingRect();
            }
        } else if (event->type() == QEvent::MouseMove && isDragging) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF delta = mouseEvent->pos() - dragStartPos;

            // Check if any point after translation would go outside the image bounds
            bool anyPointOutOfBounds = false;
            for (const QPoint& point : calibrationPolygon) {
                QPoint updatedPoint = point + delta.toPoint();
                if (updatedPoint.x() < 0 || updatedPoint.x() >= 1024 || updatedPoint.y() < 0 || updatedPoint.y() >= 768) {
                    anyPointOutOfBounds = true;
                    break;
                }
            }

            if (!anyPointOutOfBounds) {
                calibrationPolygon.translate(delta.toPoint());
                dragStartPos = mouseEvent->pos();
                qDebug() << calibrationPolygon;
                this->update();
            }
        } else if (event->type() == QEvent::MouseButtonRelease && isDragging) {
            isDragging = false;
        }
    }

    if (event->type() == QEvent::MouseButtonPress && calibrationPolygon.size() != 2)
    {
        QLabel* imageLabel = qobject_cast<QLabel*>(obj);
        if (imageLabel) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

            // Check if the click is inside the image
            if (mouseEvent->button() == Qt::LeftButton) {
                // Get the coordinates of the click
                int x = mouseEvent->x();
                int y = mouseEvent->y();
                qDebug() << "x: " << x << ", y: " << y;
                calibrationPolygon << QPoint(x,y);
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

    // Load the original image
    QPixmap originalImage("received_files/calibration.jpg");

    // Create a pixmap with the same size as the original image
    QPixmap pixmap(originalImage.size());
    pixmap.fill(Qt::transparent);

    // Create a painter for the pixmap
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw shapes based on the number of points collected
    if (calibrationPolygon.size() == 1) {
        int crossSize = 20;
        QPoint clickedPoint = calibrationPolygon.first();

        painter.setPen(QColor(Qt::red));

        // Draw horizontal line
        painter.drawLine(clickedPoint.x() - crossSize, clickedPoint.y(), clickedPoint.x() + crossSize, clickedPoint.y());

        // Draw vertical line
        painter.drawLine(clickedPoint.x(), clickedPoint.y() - crossSize, clickedPoint.x(), clickedPoint.y() + crossSize);
    }

    if(isSubmitted) {
        painter.setPen(QColor(Qt::green));
    } else {
        painter.setPen(QColor(Qt::yellow));
    }

    if (calibrationPolygon.size() == 2) {
        // Get the first two points
        QPoint topLeft = calibrationPolygon[0];
        QPoint bottomRight = calibrationPolygon[1];

        // Draw the rectangle
        painter.drawRect(QRect(topLeft, bottomRight));
    }

    // Create a painter for the original image
    QPainter originalPainter(&originalImage);
    originalPainter.setRenderHint(QPainter::Antialiasing, true);
    originalPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Draw the pixmap onto the original image
    originalPainter.drawPixmap(0, 0, pixmap);

    // Set the updated original image as the pixmap to the QLabel
    imageLabel->setPixmap(originalImage);
}


