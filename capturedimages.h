#ifndef CAPTUREDIMAGES_H
#define CAPTUREDIMAGES_H

#include <QWidget>
#include <QRect>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSlider>
#include <QLabel>
#include "scanner.h"

class CapturedImages : public QWidget
{
    Q_OBJECT
public:
    explicit CapturedImages(Scanner *scanner, QWidget *parent = nullptr);

private:
    int sliderValue;
    QPixmap *originalImage;
    QPixmap *finalImage;
    QRect *rect;
    QPainter *initialPainter;

    QVBoxLayout *layout;

    // for images
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QGridLayout *contentLayout;

    QSlider *slider;

    QLabel *originalLabel;
    QLabel *originalImageLabel;
    QLabel *finalLabel;
    QLabel *finalImageLabel;

    void addBorder(QPixmap& pixmap, const QColor& color, int borderWidth = 5);

};



#endif // CAPTUREDIMAGES_H
