#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <QObject>
#include <QAbstract3DGraph>
#include <QtDataVisualization>

class PointCloud: public Q3DScatter
{
    Q_OBJECT
public:
    PointCloud();
    void addNewDataPoint(double x, double y, double z);
    void reRenderGraph();

private:
    QScatter3DSeries *series;
    QScatterDataArray data;
};

#endif // POINTCLOUD_H
