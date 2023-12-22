#include <QLabel>
#include <QHBoxLayout>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#include "global.h"
#include "footer.h"
#include "scanner.h"
#include "communication.h"

Footer::Footer(Scanner* scanner, QWidget *parent): QWidget(parent)
{
    this->layout = new QVBoxLayout();
    this->statsLayout = new QHBoxLayout();
    this->progressBarLayout = new QHBoxLayout();
    this->scanner = scanner;
    if(scanner->getScannerState() == ScannerState::RUNNING) {
        running->setText("Running");
    } else {
        running->setText("Idle");
    }
    steps->setText(QString("0 steps out of %1").arg(scanner->getHorizontalPrecision()));
    degree->setText("0.0 degree");
    percentage->setText("0% Completed");
    numOfPoints->setText("0 points scanned");
    time->setText("0:0 seconds passed");

    button->setStyleSheet("padding: 10px; margin: 10px;");
    button->setText("Start");
    button->setEnabled(false);

    progressBar->setRange(0, 100);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #19749B;"
        "    border-radius: 5px;"
        "    background-color: #1C1C1C;"
        "    text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #33C2FF;"
        "    width: 10px;"
        "}"
        );

    connect(button, &QPushButton::clicked, [this] {
        this->button->setEnabled(true);
        if(this->scanner->getScannerState() == ScannerState::RUNNING) {
            this->scanner->setScannerState(ScannerState::STOPPED);
            this->button->setText("Start");
            Communication::sendConfig("cancel");
        } else {
            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE);
            sprintf(buffer, "START");

            send(clientSocket, buffer, BUFFER_SIZE, 0);

            this->scanner->setScannerState(ScannerState::RUNNING);
            this->button->setText("Cancel");

            std::thread t1(Communication::readData);
            t1.detach();
        }
        this->scanner->updateScanner();
    });

    this->layout->addLayout(this->statsLayout);
    this->layout->addLayout(this->progressBarLayout);
    this->statsLayout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
    this->setupWidgets();
}

void Footer::clearWidgets() {
    while (QLayoutItem* item = this->statsLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void Footer::setupWidgets() {
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(running);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(steps);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(percentage);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(degree);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(numOfPoints);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(time);
    this->statsLayout->addStretch();
    this->statsLayout->addWidget(button);
    this->statsLayout->addStretch();
    this->progressBarLayout->addWidget(progressBar);
}

void Footer::footerUpdated()
{
    // Assuming you have getter methods in your Scanner class
    int currentStep = scanner->getCurrentStep();
    int horizontalPrecision = scanner->getHorizontalPrecision();
    int verticalPrecision = scanner->getVerticalPrecision();
    int numberOfPointsScanned = scanner->getNumberOfPointsScanned();
    auto duration = std::chrono::duration<double>(
                        std::chrono::high_resolution_clock::now() - scanner->getStartTime()
                        ).count();

    if(scanner->getConnected()) {
        this->button->setEnabled(true);
        if (scanner->getScannerState() == RUNNING) {
            running->setText("Running");
            this->button->setText("Cancel");

        } else {
            running->setText("Finished");
            this->button->setText("Start");
        }
    } else {
        this->button->setEnabled(false);
    }

    progressBar->setValue((static_cast<double>(currentStep) / horizontalPrecision) * 100);

    // Assuming these methods return appropriate values
    steps->setText(QString("%1 steps out of %2").arg(currentStep).arg(horizontalPrecision));
    degree->setText(QString("%1 degree").arg(static_cast<double>(currentStep) / horizontalPrecision * 360));
    percentage->setText(QString("%1% Completed").arg((static_cast<double>(currentStep) / horizontalPrecision) * 100));
    numOfPoints->setText(QString("%1 points scanned").arg(numberOfPointsScanned));
    time->setText(QString("%1 seconds passed").arg(duration));

}
