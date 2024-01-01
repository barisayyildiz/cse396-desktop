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

    lastX = 0.0f;
    lastY = 0.0f;

    backgroundColor = QColor("grey");

    isWireframe = false;
}

void OpenGLWidget::initializeGL()
{
    //initialize opengl functions
    this->initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    float aspectRatio = width() / (float)height();

    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

    shader = new Shader("res/shaders/shader.vs", "res/shaders/shader.ps");
    model = new Model("received_files/3d.obj", "New Model");

    emit modelUploaded();

    std::vector<std::string> paths{
                                   "res/textures/floor.jpg",
                                    "res/textures/flower-design.jpg",
                                    "res/textures/grass.jpg",
                                    "res/textures/wood.jpg",
                                    "res/textures/flag.jpg",
                                    "res/textures/vercetti.png",
                                    "res/textures/gs.jpg"
    };

    for (auto path : paths)
    {
        mapTexture[path] = Material::LoadTexture(path.c_str());
    }

    model->SetMeshTexture(0, "res/textures/vercetti.png");

    //signal that everything is initialize, now we can fill the animation list on GUI
    emit initialized();
}

void OpenGLWidget::paintGL()
{
    if(reloadFlag) {
        model = new Model("received_files/3d.obj", "New Model");
        model->SetMeshTexture(0, "res/textures/vercetti.png");
        reloadFlag = false;
    }
    //set the background color
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);

    //clear the color and depth using the color we set above
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(isWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader->Use();

    glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

    // Set matrices
    shader->SetMat4("projection", projection);
    shader->SetMat4("view", camera.GetViewMatrix());
    shader->SetMat4("modelMatrix", modelMat);

    // Set light properties
    /*shader->SetVec3("light.direction", glm::vec3(1.0f, -1.0f, -1.0f));  // Directional light from the top-left
    shader->SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));*/

    // Set light properties for the first light
    shader->SetVec3("light.direction", glm::vec3(1.0f, -1.0f, -1.0f));  // Directional light from the top-left
    shader->SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->SetVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));

    // Set light properties for the second light (opposite direction)
    shader->SetVec3("lightDirection2", glm::vec3(-1.0f, 1.0f, 1.0f));  // Directional light from the bottom-right
    shader->SetVec3("lightAmbient2", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->SetVec3("lightDiffuse2", glm::vec3(0.5f, 0.5f, 0.5f));

    if(model) {
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
    }
}

void OpenGLWidget::resizeGL(int w, int h)
{
    float aspectRatio = width() / (float)height();

    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);
}


void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
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

void OpenGLWidget::loadModel(const std::string path)
{
    reloadFlag = true;
    /*makeCurrent();
    shader = new Shader("res/shaders/shader.vs", "res/shaders/shader.ps");
    model = new Model("received_files/3d.obj", "New Model");
    emit modelUploaded();
    model->SetMeshTexture(0, "res/textures/vercetti.png");*/
    //model->Delete();
    //this->initializeGL();
    /*model = NULL;
    model = new Model("received_files/3d.obj", "New Model");*/
    //model = new Model(path, "New Model");
}
