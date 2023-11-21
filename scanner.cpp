#include "scanner.h"
#include <QDebug>

#include <chrono>

Scanner::Scanner()
{
    scannerState = ScannerState::FINISHED;
    horizontalPrecision = 256;
    verticalPrecision = 455;
    currentStep = 0;
    numberOfPointsScanned = 0;
}

void Scanner::startTimer()
{
    startTime = std::chrono::high_resolution_clock::now();
    scannerState = ScannerState::RUNNING;
    // Start a thread to periodically print the elapsed time
    //outputThread = std::thread(&Scanner::outputElapsedTime, this);
}

void Scanner::stopTimer() {
    if(scannerState == ScannerState::RUNNING) {
        endTime = std::chrono::high_resolution_clock::now();
        scannerState = ScannerState::FINISHED;
        //outputThread.join();
        calculateElapsedTime();
    } else {
        qDebug() << "Timer is not running. Call start() before stop().";
    }
}

void Scanner::resetTimer() {
    startTime = std::chrono::high_resolution_clock::now();
    endTime = startTime;
    scannerState = ScannerState::FINISHED;
}

void Scanner::calculateElapsedTime()
{
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    qDebug() << "Total Elapsed Time: " << duration << " seconds.";
}

void Scanner::outputElapsedTime() {
    qDebug() << "beginning of output elapsed time";
    qDebug() << scannerState << ", " ;
    while (ScannerState::RUNNING) {
        qDebug() << "inside while loop............";
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(now - startTime).count();
        qDebug() << "Elapsed Time: " << elapsed << " seconds.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


void Scanner::setScannerState(ScannerState scannerState) { this->scannerState=scannerState; }
void Scanner::setHorizontalPrecision(int horizontalPrecision) { this->horizontalPrecision=horizontalPrecision; }
void Scanner::setVerticalPrecision(int verticalPrecision) { this->verticalPrecision=verticalPrecision; }
void Scanner::setCurrentStep(int currentStep) { this->currentStep=currentStep; }
void Scanner::setNumberOfPointsScanned(int numberOfPointsScanned) { this->numberOfPointsScanned=numberOfPointsScanned; }

void Scanner::updateScanner()
{
    emit updateScannerSignal();
}

std::chrono::system_clock::time_point Scanner::getStartTime()  { return startTime; }
ScannerState Scanner::getScannerState() { return scannerState; }
int Scanner::getHorizontalPrecision() { return horizontalPrecision; }
int Scanner::getVerticalPrecision() { return verticalPrecision; }
int Scanner::getCurrentStep() { return currentStep; }
int Scanner::getNumberOfPointsScanned() { return numberOfPointsScanned; }
