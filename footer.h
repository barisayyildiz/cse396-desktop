#ifndef FOOTER_H
#define FOOTER_H

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include "scanner.h"

class Footer: public QWidget
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
        QPushButton *button = new QPushButton();
        QProgressBar *progressBar = new QProgressBar();
        QVBoxLayout *layout = new QVBoxLayout();
        QHBoxLayout *statsLayout = new QHBoxLayout();
        QHBoxLayout *progressBarLayout = new QHBoxLayout();
        void setupWidgets();
        void clearWidgets();
    public slots:
        void footerUpdated();
};

#endif // FOOTER_H
