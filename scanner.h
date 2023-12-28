#ifndef SCANNER_H
#define SCANNER_H

#include <chrono>
#include <thread>
#include <QObject>
#include <QPoint>

enum ScannerState {
    RUNNING,
    FINISHED,
    CANCELLED,
    IDLE
};

class Scanner: public QObject
{
    Q_OBJECT
public:
    Scanner();
    void startTimer();
    void stopTimer();
    void resetTimer();
    QPoint* getTopLeftPoint() { return this->topLeftPoint; }
    QPoint* getBottomRightPoint () { return this->bottomRightPoint; }
    ScannerState getScannerState();
    int getHorizontalPrecision();
    int getVerticalPrecision();
    int getCurrentStep();
    int getNumberOfPointsScanned();
    bool getConnected() { return this->connected; }
    std::chrono::high_resolution_clock::time_point getStartTime();
    void setTopLeftPoint(QPoint *point) { this->topLeftPoint = point; }
    void setBottomRightPoint(QPoint *point) { this->bottomRightPoint = point; }
    void setScannerState(ScannerState scannerState);
    void setHorizontalPrecision(int horizontalPrecision);
    void setVerticalPrecision(int verticalPrecision);
    void setCurrentStep(int currentStep);
    void setNumberOfPointsScanned(int numberOfPointsScanned);
    void setConnected(bool connected) { this->connected = connected; }
    void updateScanner();
signals:
    void updateScannerSignal();
private:
    ScannerState scannerState;
    int horizontalPrecision; // number of slices needs to be completed
    int verticalPrecision;
    int currentStep;
    int numberOfPointsScanned;
    bool connected;

    QPoint* topLeftPoint;
    QPoint* bottomRightPoint;

    void calculateElapsedTime();
    void outputElapsedTime();

    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::thread outputThread;
};

#endif // SCANNER_H
