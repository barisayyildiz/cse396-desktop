#include "capturedimages.h"

#include <QPainter>

CapturedImages::CapturedImages(Scanner *scanner, QWidget *parent)
    : QWidget{parent}
{
    this->scanner = scanner;
    this->layout = new QVBoxLayout(this);

    this->sliderValue = 0;
    this->originalImage = new QPixmap("received_files/original/" + QString::number(this->sliderValue) + ".jpg");
    this->finalImage = new QPixmap("received_files/final/" + QString::number(this->sliderValue) + ".jpg");

    this->initialPainter = new QPainter(this->originalImage);
    if(this->scanner->getConnected()) {
        this->rect = new QRect(*scanner->getTopLeftPoint(), *scanner->getBottomRightPoint());
        this->initialPainter->setPen(QColor(Qt::green));
        this->initialPainter->drawRect(*rect);
    }
    this->initialPainter->end();

    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setWidgetResizable(true);
    this->layout->addWidget(scrollArea);

    // Create content widget
    this->contentWidget = new QWidget(this->scrollArea);
    this->contentLayout = new QGridLayout(this->contentWidget);

    // Create labels
    this->originalLabel = new QLabel("Original Image");
    this->originalLabel->setAlignment(Qt::AlignHCenter);
    this->contentLayout->addWidget(originalLabel, 0, 0, 1, 1, Qt::AlignCenter);

    this->originalImageLabel = new QLabel;
    this->originalImageLabel->setAlignment(Qt::AlignCenter);
    this->originalImageLabel->setPixmap(*this->originalImage);
    this->contentLayout->addWidget(this->originalImageLabel, 1, 0, 1, 1, Qt::AlignCenter);

    this->finalLabel = new QLabel("Final Image");
    this->finalLabel->setAlignment(Qt::AlignHCenter);
    this->contentLayout->addWidget(this->finalLabel, 0, 1, Qt::AlignCenter);

    this->finalImageLabel = new QLabel;
    this->finalImageLabel->setPixmap(*this->finalImage);
    this->finalImageLabel->setAlignment(Qt::AlignCenter);
    this->contentLayout->addWidget(finalImageLabel, 1, 1, Qt::AlignCenter);

    // Create slider
    this->slider = new QSlider(Qt::Horizontal);
    this->layout->addWidget(slider);

    this->slider->setValue(0);
    this->slider->setRange(0, this->scanner->getCurrentStep());

    // Set layout for the content widget
    this->contentWidget->setLayout(contentLayout);
    this->scrollArea->setWidget(contentWidget);

    // Set layout for the main window
    this->setLayout(layout);


    // Connect slider's valueChanged signal to a custom slot
    connect(slider, &QSlider::valueChanged, [this](
                                                int value) {
        // Load images based on slider value
        this->originalImage = new QPixmap("received_files/original/" + QString::number(value) + ".jpg");
        this->finalImage = new QPixmap("received_files/final/" + QString::number(value) + ".jpg");

        // Update the displayed pixmaps
        this->originalImageLabel->setPixmap(*this->originalImage);
        this->finalImageLabel->setPixmap(*this->finalImage);
    });


}

void CapturedImages::capturedImagesUpdated()
{
    // Update the slider range and value
    this->slider->setRange(0, this->scanner->getCurrentStep() - 1);
    this->slider->setValue(this->scanner->getCurrentStep() - 1);

    /*
    qDebug() << "slider updated";
    // Get the current top-left and bottom-right points
    QPoint topLeft = *this->scanner->getTopLeftPoint();
    QPoint bottomRight = *this->scanner->getBottomRightPoint();

    qDebug() << "topleft and bottomright fetched";

    // Create a pixmap with the same size as the original image and fill it with a transparent color
    QPixmap pixmap(this->originalImage->size());
    pixmap.fill(Qt::transparent);

    // Create a painter for the pixmap
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw the new rectangle on the transparent pixmap
    painter.setPen(QColor(Qt::green));
    painter.drawRect(QRect(topLeft, bottomRight));
    painter.end();

    // Update the original image with the pixmap
    QPixmap updatedOriginalImage(*this->originalImage);
    QPainter originalPainter(&updatedOriginalImage);
    originalPainter.setRenderHint(QPainter::Antialiasing, true);
    originalPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    originalPainter.drawPixmap(0, 0, pixmap);
    originalPainter.end();

    // Update the displayed pixmaps
    this->originalImageLabel->setPixmap(updatedOriginalImage);
    this->finalImageLabel->setPixmap(*this->finalImage);
*/
}
