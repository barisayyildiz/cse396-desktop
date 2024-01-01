#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "global.h"
#include "pointcloud.h"
#include "scannedpoints.h"
#include "scanner.h"
#include "openglwidget.h"
#include "calibration.h"
#include <QPolygon>

class Communication
{
public:
    static ScannedPoints* scannedPoints;
    static PointCloud* pointCloud;
    static Scanner* scanner;
    static OpenGLWidget* openGlWidget;
    static Calibration* calibration;
    static void readData();
    static int sendConfig(const char* command);
    static int sendConfig(int horizontalPrecision, int verticalPrecision);
    static int sendConfig(const QPolygon& calibrationPolygon);
    static int readFromScanner();
    static int readImageForCalibration();

    static void readLiveData();

    static void setConfig();
private:
    static bool isConfigSet;
    static std::mutex configMutex;
    static std::condition_variable configSetCondition;

    // New mutex for readImageForCalibration
    static bool isCalibrationConfigSet;
    static std::mutex calibrationConfigMutex;
    static std::condition_variable calibrationConfigSetCondition;

    static bool isLive;
    static std::mutex liveMutex;
    static std::condition_variable liveCondition;
};

#endif // COMMUNICATION_H
