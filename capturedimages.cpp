#include "capturedimages.h"

#include <QPainter>

CapturedImages::CapturedImages(Scanner *scanner, QWidget *parent)
    : QWidget{parent}
{
    this->layout = new QVBoxLayout(this);

    this->sliderValue = 0;
    this->originalImage = new QPixmap("images/original/" + QString::number(this->sliderValue) + ".jpg");
    this->finalImage = new QPixmap("images/red_line/" + QString::number(this->sliderValue) + ".jpg");

    this->rect = new QRect(*scanner->getTopLeftPoint(), *scanner->getBottomRightPoint());

    this->initialPainter = new QPainter(this->originalImage);
    this->initialPainter->setPen(QColor(Qt::green));
    this->initialPainter->drawRect(*rect);
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

    // Set layout for the content widget
    this->contentWidget->setLayout(contentLayout);
    this->scrollArea->setWidget(contentWidget);

    // Set layout for the main window
    this->setLayout(layout);


    // Connect slider's valueChanged signal to a custom slot
    connect(slider, &QSlider::valueChanged, [this](
                                                int value) {
        // Load images based on slider value
        this->originalImage = new QPixmap("images/original/" + QString::number(value) + ".jpg");
        this->finalImage = new QPixmap("images/red_line/" + QString::number(value) + ".jpg");

        // Draw bounding box on the original image
        QPainter originalPainter(this->originalImage);
        originalPainter.setPen(QColor(Qt::green));
        originalPainter.drawRect(*this->rect);

        // Add a grey border to each image
        addBorder(*this->originalImage, QColor(Qt::gray));
        addBorder(*this->finalImage, QColor(Qt::gray));

        // Update the displayed pixmaps
        this->originalImageLabel->setPixmap(*this->originalImage);
        this->finalImageLabel->setPixmap(*this->finalImage);
    });


}

void CapturedImages::addBorder(QPixmap& pixmap, const QColor& color, int borderWidth) {
    QPainter painter(&pixmap);
    painter.setPen(color);
    QRect borderRect(0, 0, pixmap.width(), pixmap.height());
    painter.drawRect(borderRect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth));
    painter.end();
}
