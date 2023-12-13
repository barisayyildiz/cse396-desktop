#ifndef SCANNEDPOINTS_H
#define SCANNEDPOINTS_H

#include <QObject>
#include <QtCharts>
#include <QVector>

class ScannedPoints : public QChartView
{
    Q_OBJECT
public:
    explicit ScannedPoints(QWidget *parent = nullptr);
    int stepCounter;
    int yMax;
    QLineSeries *series;
    void addNewDataPoint(double y);

private:
    QChart* chart_;
    QSplineSeries* series_;
    QVector<int>* recentPoints;

};

#endif // SCANNEDPOINTS_H
