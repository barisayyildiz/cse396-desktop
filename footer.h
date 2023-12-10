#ifndef FOOTER_H
#define FOOTER_H

#include <QLabel>
#include <QPushButton>
#include "scanner.h"

class Footer: public QHBoxLayout
{
    public:
        Footer(Scanner* scanner, QWidget* parent = nullptr);
    private:
        Scanner* scanner;
        QLabel *running = new QLabel();
        QLabel *steps = new QLabel();
        QLabel *percentage = new QLabel();
        QLabel *degree = new QLabel();
        QLabel *numOfPoints = new QLabel();
        QLabel *time = new QLabel();
        QPushButton *scanButton = new QPushButton();
        QPushButton *cancelButton = new QPushButton();
        QPushButton *stopButton = new QPushButton();
        void setupWidgets();
        void clearWidgets();
    public slots:
        void footerUpdated();
};

#endif // FOOTER_H
