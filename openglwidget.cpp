#include "openglwidget.h"

#include <QKeyEvent>

#include <QMouseEvent>

#include <QtMath>

#include <QMatrix4x4>

#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Shader.h"


OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    deltaTime = 0.0;

    lastX = 0.0f;
    lastY = 0.0f;

    backgroundColor = QColor("grey");

    isWireframe = false;
}

void OpenGLWidget::initializeGL()
{
    //initialize opengl functions
    this->initializeOpenGLFunctions();

    //enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);

    startTime = std::chrono::steady_clock::now();

    float aspectRatio = width() / (float)height();

    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));

    shader = new Shader("res/shaders/shader.vs", "res/shaders/shader.ps");

    model = new Model("res/models/3d.obj", "New Model");

    std::vector<std::string> paths{ "res/textures/floor.jpg", "res/textures/flower-design.jpg", "res/textures/grass.jpg", "res/textures/wood.jpg" };

    for (auto path : paths)
    {
        mapTexture[path] = Material::LoadTexture(path.c_str());
    }

    model->SetMeshTexture(0, "res/models/alliance.png");

    //signal that everything is initialize, now we can fill the animation list on GUI
    emit initialized();
}

void OpenGLWidget::paintGL()
{

    //get current time
    auto currentTime = std::chrono::steady_clock::now();

    /*
     *  calculate delta time
     *
     *  delta time is time between two frames in milliseconds
    */
    deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

    //set the background color
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);

    //clear the color and depth using the color we set above
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(isWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));

    shader->Use();

    glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    /*glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    //glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));

    shader->SetMat4("modelMatrix", modelMat);*/
    // Set matrices
    shader->SetMat4("projection", projection);
    shader->SetMat4("view", camera.GetViewMatrix());
    shader->SetMat4("modelMatrix", modelMat);

    // Set light properties
    shader->SetVec3("light.direction", glm::vec3(1.0f, -1.0f, -1.0f));  // Directional light from the top-left
    shader->SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));

    const auto& meshes{ model->GetMeshes() };

    for (const auto& mesh : meshes)
    {
        if (!isWireframe)
            glBindTexture(GL_TEXTURE_2D, mesh.material->GetParameterTexture(Material::NORMAL));
        else
            glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(mesh.VAO);

        glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //Qt update
    update();

    startTime = currentTime;
}

void OpenGLWidget::resizeGL(int w, int h)
{
    float aspectRatio = width() / (float)height();

    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
}


void OpenGLWidget::keyPressEvent(QKeyEvent * event)
{
    float sec = deltaTime / 1000.0f;

    /*if (event->key() == Qt::Key::Key_W)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, sec);
    if (event->key() == Qt::Key::Key_S)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, sec);
    if (event->key() == Qt::Key::Key_A)
        camera.ProcessKeyboard(Camera_Movement::LEFT, sec);
    if (event->key() == Qt::Key::Key_D)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, sec);*/

    if (event->key() == Qt::Key::Key_W)
        camera.ProcessMouseScroll(0.1f);
    if (event->key() == Qt::Key::Key_S)
        camera.ProcessMouseScroll(-0.1f);

    event->accept();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "mouse press event";
    lastX = event->x();
    lastY = event->y();
    event->accept();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    int xpos = event->x();
    int ypos = event->y();

    float deltaX = xpos - lastX;
    float deltaY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    qDebug() << deltaX << ", " << deltaY;

    if (event->buttons() & Qt::LeftButton)
    {
        if (event->modifiers() & Qt::ControlModifier) {
            // Handle panning
            camera.ProcessInput(deltaX, deltaY, true);
        }
        else {
            // Handle regular camera movement
            camera.ProcessInput(deltaX, deltaY);
        }
    }

    event->accept();
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    // The delta is given in 1/8th degree steps
    float delta = event->angleDelta().y();

    // You can adjust the sensitivity based on your requirements
    float  sensitivity= 0.002f;

    camera.ProcessMouseScroll(delta * sensitivity);

    // Accept the event to prevent it from being propagated further
    event->accept();
}



void OpenGLWidget::SetModelTexture(std::string path)
{
    if (model->GetMeshes().empty())
        return;

    auto id = mapTexture.at(path);

    model->GetMeshes().at(0).material->Set(id);
}
