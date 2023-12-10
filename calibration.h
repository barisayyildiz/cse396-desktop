#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPoint>
#include <QPushButton>
#include <QPolygon>
#include <QLabel>
#include "scanner.h"

class Calibration : public QWidget
{
    Q_OBJECT
public:
    explicit Calibration(Scanner* scanner, QWidget *parent = nullptr);

private:
    Scanner* scanner;

    QGridLayout *layout;

    QPoint* topLeft;
    QPoint* topRight;
    QPoint* bottomRight;
    QPoint* bottomLeft;

    QLabel* imageLabel;

    QPixmap *calibrationImage;

    QPushButton* startButton;
    QPushButton* resetButton;
    QPushButton* submitButton;

    QPolygon calibrationPolygon;

    int counter;
    bool isActive;
    bool isDrawn;

    bool eventFilter(QObject* obj, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
};

#endif // CALIBRATION_H
