#ifndef SCANNER_H
#define SCANNER_H

#include <chrono>
#include <thread>
#include <QObject>

enum ScannerState {
    RUNNING,
    FINISHED,
    STOPPED
};

class Scanner: public QObject
{
    Q_OBJECT
public:
    Scanner();
    void startTimer();
    void stopTimer();
    void resetTimer();
public:
    ScannerState getScannerState();
    int getHorizontalPrecision();
    int getVerticalPrecision();
    int getCurrentStep();
    int getNumberOfPointsScanned();
    std::chrono::high_resolution_clock::time_point getStartTime();
    void setScannerState(ScannerState scannerState);
    void setHorizontalPrecision(int horizontalPrecision);
    void setVerticalPrecision(int verticalPrecision);
    void setCurrentStep(int currentStep);
    void setNumberOfPointsScanned(int numberOfPointsScanned);
    void updateScanner();
signals:
    void updateScannerSignal();
private:
    ScannerState scannerState;
    int horizontalPrecision; // number of slices needs to be completed
    int verticalPrecision;
    int currentStep;
    int numberOfPointsScanned;

    void calculateElapsedTime();
    void outputElapsedTime();

    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::thread outputThread;
};

#endif // SCANNER_H
