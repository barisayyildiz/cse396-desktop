#pragma once

#include <QOpenGLWidget>
#include <qopenglfunctions_4_3_core.h>

#include <chrono>

#include <unordered_map>

#include "Camera.h"

class Model;
class Shader;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
Q_OBJECT
public:
    OpenGLWidget(QWidget* parent = nullptr);

    virtual ~OpenGLWidget() override {}

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void initialized();

public slots:
    void SetBackgroundColor(QColor bgColor)
    {
        backgroundColor = bgColor;
    }

    void ToggleWireframe()
    {
        isWireframe = !isWireframe;
    }

    void SetModelTexture(std::string path);

private:
    std::chrono::steady_clock::time_point startTime;

    float lastX;
    float lastY;

    double deltaTime;

    //OrbitCamera camera{ glm::vec3(0.0f, 0.5f, 3.0f) };
    OrbitCamera camera;

    QColor backgroundColor;

    bool isWireframe;

    std::unordered_map<std::string, unsigned int> mapTexture;

public:
    Model* model;

    Shader* shader;

    glm::mat4 projection;

    unsigned int uboMatrices;
};
