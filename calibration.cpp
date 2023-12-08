#include "calibration.h"

#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

Calibration::Calibration(Scanner* scanner, QWidget *parent)
    : QWidget{parent}, started(false), counter(0)
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

    startButton = new QPushButton("Start to Calibrate", this);
    resetButton = new QPushButton("Reset the Calibration", this);
    submitButton = new QPushButton("Submit", this);

    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    buttonLayout->addWidget(startButton);// Adjust the spacing here
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(submitButton);
    buttonLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->layout->addWidget(buttonsWidget, 0, 4, 1, 1);

    resetButton->setEnabled(false);

    connect(startButton, &QPushButton::pressed, [this]() {
        this->started = true;
        resetButton->setEnabled(false);
        qDebug() << this->started;
    });

    connect(resetButton, &QPushButton::pressed, [this]() {
        this->started = false;
        this->counter = 0;
        qDebug() << this->started;
    });

    connect(submitButton, &QPushButton::pressed, [this]() {
        this->started = true;
        qDebug() << this->started;

        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.exec();
    });

    this->setLayout(this->layout);

}
