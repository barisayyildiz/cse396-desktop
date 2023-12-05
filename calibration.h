#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QVBoxLayout>
#include "scanner.h"

class Calibration : public QWidget
{
    Q_OBJECT
public:
    explicit Calibration(Scanner* scanner, QWidget *parent = nullptr);

private:
    Scanner* scanner;

    QGridLayout *layout;

signals:
};

#endif // CALIBRATION_H