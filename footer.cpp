#include <QLabel>
#include <QHBoxLayout>
#include <chrono>

#include "footer.h"
#include "scanner.h"

Footer::Footer(Scanner* scanner, QWidget *parent): QHBoxLayout(parent)
{
    this->scanner = scanner;
    if(scanner->getScannerState() == ScannerState::RUNNING) {
        running->setText("Running");
    } else {
        running->setText("Stopped");
    }
    steps->setText(QString("0 steps out of %1").arg(scanner->getHorizontalPrecision()));
    degree->setText("0.0 degree");
    percentage->setText("0% Completed");
    numOfPoints->setText("0 points scanned");
    time->setText("0:0 seconds passed");

    button->setStyleSheet("padding: 10px; margin: 10px;");
    button->setText("Start");
    button->setEnabled(false);

    connect(button, &QPushButton::clicked, [this] {
        this->button->setEnabled(true);
        if(this->scanner->getScannerState() == ScannerState::RUNNING) {
            this->scanner->setScannerState(ScannerState::STOPPED);
            this->button->setText("Start");
        } else {
            this->scanner->setScannerState(ScannerState::RUNNING);
            this->button->setText("Stop");
        }
        this->scanner->updateScanner();
    });

    setAlignment(Qt::AlignCenter);
    this->setupWidgets();
}

void Footer::clearWidgets() {
    while (QLayoutItem* item = takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void Footer::setupWidgets() {
    addStretch();
    addWidget(running);
    addStretch();
    addWidget(steps);
    addStretch();
    addWidget(percentage);
    addStretch();
    addWidget(degree);
    addStretch();
    addWidget(numOfPoints);
    addStretch();
    addWidget(time);
    addStretch();
    addWidget(button);
    addStretch();
}

void Footer::footerUpdated()
{
    // Assuming you have getter methods in your Scanner class
    int currentStep = scanner->getCurrentStep();
    int horizontalPrecision = scanner->getHorizontalPrecision();
    int verticalPrecision = scanner->getVerticalPrecision();
    int numberOfPointsScanned = scanner->getNumberOfPointsScanned();
    qDebug() << numberOfPointsScanned;
    auto duration = std::chrono::duration<double>(
                        std::chrono::high_resolution_clock::now() - scanner->getStartTime()
                        ).count();

    if(scanner->getConnected()) {
        this->button->setEnabled(true);
        if (scanner->getScannerState() == RUNNING) {
            running->setText("Running");
            this->button->setText("Stop");

        } else {
            running->setText("Finished");
            this->button->setText("Start");
        }
    } else {
        this->button->setEnabled(false);
    }

    // Assuming these methods return appropriate values
    steps->setText(QString("%1 steps out of %2").arg(currentStep).arg(horizontalPrecision));
    degree->setText(QString("%1 degree").arg(static_cast<double>(currentStep) / horizontalPrecision * 360));
    percentage->setText(QString("%1% Completed").arg((static_cast<double>(currentStep) / horizontalPrecision) * 100));
    numOfPoints->setText(QString("%1 points scanned").arg(numberOfPointsScanned));
    time->setText(QString("%1 seconds passed").arg(duration));

}
