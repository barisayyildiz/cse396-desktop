#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPoint>
#include <QPushButton>
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

    QPushButton* startButton;
    QPushButton* resetButton;
    QPushButton* submitButton;

    int counter;
    bool started;

signals:
};

#endif // CALIBRATION_H
