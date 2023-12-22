#include "pointcloud.h"
#include <QAbstract3DGraph>
#include <QtDataVisualization>

PointCloud::PointCloud()
    : Q3DScatter()
{
    setFlags(flags() ^ Qt::FramelessWindowHint);
    Q3DTheme *theme = new Q3DTheme(Q3DTheme::ThemeEbony);
    setActiveTheme(theme);

    initSeries();

    setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    setSelectionMode(QAbstract3DGraph::SelectionNone);
}

void PointCloud::addNewDataPoint(double x, double y, double z)
{
    data << QVector3D(x, y, z);
}

void PointCloud::reRenderGraph()
{
    if(!data.isEmpty()) {
        series->dataProxy()->addItems(data);
        addSeries(series);
        data.clear();
    }
}

void PointCloud::initSeries() {
    series = new QScatter3DSeries;
    series->setItemSize(0.07f);
    //series->setMesh(QAbstract3DSeries::MeshPoint);

    // Initialize colors
    QLinearGradient linearGrad(QPointF(100, 100), QPointF(200, 200));
    linearGrad.setColorAt(0, Qt::blue);
    linearGrad.setColorAt(1, Qt::red);
    series->setBaseGradient(linearGrad);
    series->setColorStyle(Q3DTheme::ColorStyle::ColorStyleObjectGradient);
    addSeries(series);
}

void PointCloud::resetPointCloud()
{
    data.clear();
    removeSeries(series);
    delete series;
    initSeries();
    reRenderGraph();
}
