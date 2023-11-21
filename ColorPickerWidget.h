#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

#include <QtWidgets>

class ColorPickerWidget : public QWidget {
    Q_OBJECT

public:
    ColorPickerWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        QGridLayout* layout = new QGridLayout(this);

        // Define a list of colors to fill the grid.
        QList<QColor> colors = {
            QColor("red"), QColor("green"), QColor("blue"), QColor("yellow"), QColor("purple"),
            QColor("orange"), QColor("brown"), QColor("grey"), QColor("pink"), QColor("lime"),
            QColor("cyan"), QColor("magenta"), QColor("gold"), QColor("salmon"), QColor("navy")
        };

        // Create buttons for colors in a 5 columns x 3 rows grid.
        for (int i = 0; i < colors.size(); ++i) {
            QPushButton* button = new QPushButton;
            button->setFixedSize(20, 20); // Set fixed size for buttons

            // Set the button background color and remove the border
            QString css = QString("background-color: %1; border: none;").arg(colors[i].name());
            button->setStyleSheet(css);

            // Connect the button's clicked signal to the ColorPickerWidget's colorSelected slot
            connect(button, &QPushButton::clicked, this, [this, colors, i]() {
                emit colorSelected(colors[i]);
                });

            layout->addWidget(button, i / 5, i % 5); // Add to grid layout
        }
    }

signals:
    void colorSelected(const QColor& color);
};

#endif
