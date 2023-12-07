#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QtWidgets>
#include <QColorDialog>

class ColorPickerWidget : public QWidget {
    Q_OBJECT

public:
    ColorPickerWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        QGridLayout* layout = new QGridLayout(this);
        color = QColor(170, 170, 170);

        QPushButton *colorButton = new QPushButton("Pick a Color");
        connect(colorButton, &QPushButton::clicked, this, &ColorPickerWidget::openColorDialog);
        layout->addWidget(colorButton);
    }

private slots:
    void openColorDialog() {
        color = QColorDialog::getColor(color, this, QString(), QColorDialog::DontUseNativeDialog);
        if(color.isValid()) {
            emit colorSelected(color);
        }
    }

private:
    QColor color;

signals:
    void colorSelected(const QColor& color);
};

#endif
