#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "global.h"
#include "pointcloud.h"
#include "scannedpoints.h"
#include "scanner.h"
#include "openglwidget.h"
#include <QPolygon>

class Communication
{
public:
    static ScannedPoints* scannedPoints;
    static PointCloud* pointCloud;
    static Scanner* scanner;
    static OpenGLWidget* openGlWidget;
    static void readData();
    static int sendConfig(const char* command);
    static int sendConfig(int horizontalPrecision, int verticalPrecision);
    static int sendConfig(const QPolygon& calibrationPolygon);
};

#endif // COMMUNICATION_H
