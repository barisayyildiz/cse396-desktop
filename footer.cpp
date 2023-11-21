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

    setAlignment(Qt::AlignCenter);
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

    if (scanner->getScannerState() == RUNNING) {
        running->setText("Running");
    } else if (scanner->getScannerState() == STOPPED) {
        running->setText("Stopped");
    } else {
        running->setText("Finished");
    }

    // Assuming these methods return appropriate values
    steps->setText(QString("%1 steps out of %2").arg(currentStep).arg(horizontalPrecision));
    degree->setText(QString("%1 degree").arg(static_cast<double>(currentStep) / horizontalPrecision * 360));
    percentage->setText(QString("%1% Completed").arg((static_cast<double>(currentStep) / horizontalPrecision) * 100));
    numOfPoints->setText(QString("%1 points scanned").arg(numberOfPointsScanned));
    time->setText(QString("%1 seconds passed").arg(duration));

}
