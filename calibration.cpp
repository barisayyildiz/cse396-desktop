#include "calibration.h"

#include <QPixmap>
#include <QLabel>
#include <QPushButton>

Calibration::Calibration(Scanner* scanner, QWidget *parent)
    : QWidget{parent}
{
    this->scanner = scanner;
    //this->layout = new QVBoxLayout(this);
    //QHBoxLayout *horizontalLayout = new QGridLayout(this);
    this->layout = new QGridLayout(this);

    QPixmap *image = new QPixmap("images/original/0.jpg");

    QLabel *imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(*image);
    this->layout->addWidget(imageLabel, 0, 0, 1, 4);

    QWidget *buttonsWidget = new QWidget;
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonsWidget);
    buttonLayout->setSpacing(20);

    QPushButton *startButton = new QPushButton("Start to Calibrate", this);
    QPushButton *resetButton = new QPushButton("Reset the Calibration", this);
    QPushButton *submitButton = new QPushButton("Submit", this);

    buttonLayout->addWidget(startButton);
    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));  // Adjust the spacing here
    buttonLayout->addWidget(resetButton);
    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));  // Adjust the spacing here
    buttonLayout->addWidget(submitButton);

    this->layout->addWidget(buttonsWidget, 0, 4, 1, 1);


    this->setLayout(this->layout);

}
