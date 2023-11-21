#ifndef SCANNEDPOINTS_H
#define SCANNEDPOINTS_H

#include <QObject>
#include <QtCharts>

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
    QLineSeries* series_;

};

#endif // SCANNEDPOINTS_H
