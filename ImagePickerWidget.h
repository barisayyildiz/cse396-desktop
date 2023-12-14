#ifndef IMAGEPICKERWIDGET_H
#define IMAGEPICKERWIDGET_H

#include <QtWidgets>
#include <vector>
#include <string>

class ImagePickerWidget : public QWidget {
    Q_OBJECT

public:
    ImagePickerWidget(QWidget* parent = nullptr) : QWidget(parent) {
        QGridLayout* layout = new QGridLayout(this);

        // Define a list of texture paths to fill the grid.
        std::vector<std::string> texturePaths = {
            "res/textures/floor.jpg",
            "res/textures/flower-design.jpg",
            "res/textures/grass.jpg",
            "res/textures/wood.jpg",
            "res/textures/flag.jpg",
            "res/textures/vercetti.png",
            "res/textures/gs.jpg"
        };

        // Create buttons for textures in a 5 columns x 3 rows grid.
        for (int i = 0; i < texturePaths.size(); ++i) {
            QPushButton* button = new QPushButton;
            button->setFixedSize(50, 50); // Set fixed size for buttons

            // Set the button icon to the image
            button->setIcon(QIcon(QString::fromStdString(texturePaths[i])));
            button->setIconSize(QSize(50, 50)); // Set icon size to fill the button

            // Connect the button's clicked signal to the ImagePickerWidget's textureSelected slot
            connect(button, &QPushButton::clicked, this, [this, texturePaths, i]() {
                emit textureSelected(texturePaths[i]);
                });

            layout->addWidget(button, i / 5, i % 5); // Add to grid layout
        }
    }

signals:
    void textureSelected(const std::string& texturePath);
};

#endif
