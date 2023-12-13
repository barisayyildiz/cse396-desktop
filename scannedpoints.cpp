#include "scannedpoints.h"
#include <limits.h>

#include <QtCharts>
#include <QPropertyAnimation>
#include <QEasingCurve>

ScannedPoints::ScannedPoints(QWidget *parent)
    : QChartView(parent)
{
    chart_ = new QChart();
    this->setChart(chart_);

    // Create a QLineSeries and add it to the chart.
    series_ = new QSplineSeries();
    recentPoints = new QVector<int>();

    chart_->addSeries(series_);
    chart_->setTitle("Number of scanned points");
    chart_->createDefaultAxes();
    //chart_->setTheme(QChart::ChartThemeDark);
    chart_->setBackgroundRoundness(0);
    chart_->setBackgroundBrush(QColor("#121212"));
    chart_->setTitleBrush(QColor("#FFFFFF"));

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Current Step");
    axisX->setTitleBrush(QColor("#fff"));
    chart_->setAxisX(axisX, series_);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleBrush(QColor("#fff"));
    chart_->setAxisY(axisY, series_);

    // Set the color and style of the grid for axisX
    QPen gridPenX;
    gridPenX.setColor(QColor("#666666")); // Replace with the desired color
    gridPenX.setStyle(Qt::DashLine);     // Set the line style to dashed
    axisX->setGridLinePen(gridPenX);
    axisX->setLabelsColor(QColor("#dddddd"));
    axisX->setTickType(QValueAxis::TicksDynamic); // Allow dynamic ticks
    axisX->setTickAnchor(1); // Anchor the ticks to an integer (e.g., 1)
    axisX->setTickInterval(1);

    // Set the color and style of the grid for axisY
    QPen gridPenY;
    gridPenY.setColor(QColor("#666666")); // Replace with the desired color
    gridPenY.setStyle(Qt::DashLine);     // Set the line style to dashed
    axisY->setGridLinePen(gridPenY);
    axisY->setLabelsColor(QColor("#dddddd"));

    stepCounter = 0;
    yMax = 0;
}

void ScannedPoints::addNewDataPoint(double y) {
    // Add a new data point to the series.
    series_->append(++stepCounter, y);
    recentPoints->push_back(y);
    if(recentPoints->size() > 10) {
        recentPoints->removeFirst();
    }

    int maxVal = INT_MIN;
    int minVal = INT_MAX;
    for(int i=0; i<recentPoints->size(); i++) {
        if(recentPoints->at(i) > maxVal) {
            maxVal = recentPoints->at(i);
        }
        if(recentPoints->at(i) < minVal) {
            minVal = recentPoints->at(i);
        }
    }
    QAbstractAxis *xAxis = chart_->axisX();
    xAxis->setRange(stepCounter + 1 - 10, stepCounter + 1);

    QAbstractAxis *yAxis = chart_->axisY();
    yAxis->setRange(minVal - 10, maxVal + 10);
}
