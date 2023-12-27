#include "communication.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

#include "openglwidget.h"

ScannedPoints* Communication::scannedPoints = nullptr;
PointCloud* Communication::pointCloud = nullptr;
Scanner* Communication::scanner = nullptr;
OpenGLWidget* Communication::openGlWidget = nullptr;

bool Communication::isConfigSet = false;
std::mutex Communication::configMutex;
std::condition_variable Communication::configSetCondition;

int clientSocket;
int serverSocket;
int configSocket;
int broadcastSocket;

void Communication::setConfig() {
    // Implement the code to set the configuration (broadcastConfig)

    // Signal that the config is set
    std::unique_lock<std::mutex> lock(configMutex);
    isConfigSet = true;
    lock.unlock();
    configSetCondition.notify_one();
}

void Communication::readData()
{
    scanner->startTimer();
    char buffer[BUFFER_SIZE];
    bool isCancelled = false;
    pointCloud->resetPointCloud();
    int round;
    int numOfRounds;

    while (true) {
        // Accept incoming connections
        qDebug() << "accepting...";

        if(scanner->getScannerState() == CANCELLED) {
            isCancelled = true;
            break;
        }

        if(scanner->getScannerState() == FINISHED) {
            break;
        }

        //send(serverSocket, buffer, BUFFER_SIZE, 0);

        memset(buffer, '\0', BUFFER_SIZE);
        int bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0);
        qDebug() << "bytes read inside readData: " << bytesRead;
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(serverSocket);
            break;
        }

        char* token;
        token = strtok(buffer, " ");

        qDebug() << "token: " << token;
        round = atoi(token);
        qDebug() << "currentStep: " << round;
        scanner->setCurrentStep(round);
        token = strtok(nullptr, " ");
        qDebug() << "token: " << token;
        numOfRounds = atoi(token);
        scanner->setHorizontalPrecision(atoi(token));
        qDebug() << "round number: " << round;

        memset(buffer, '\0', BUFFER_SIZE);
        bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(serverSocket);
            break;
        }

        int numOfScannedPoints = atoi(buffer);
        qDebug() << "number of scanned points : " << numOfScannedPoints;

        scannedPoints->addNewDataPoint(numOfScannedPoints);
        scanner->setNumberOfPointsScanned(scanner->getNumberOfPointsScanned() + numOfScannedPoints);

        /*ScannerState scannerState;
        int verticalPrecision;
        int numberOfPointsScanned;*/

        for(int i=0; i<numOfScannedPoints; i++) {
            memset(buffer, '\0', BUFFER_SIZE);
            recv(serverSocket, buffer, sizeof(buffer), 0);
            send(serverSocket, buffer, sizeof(buffer), 0);
            //qDebug() << "buffer: " << buffer;

            //qDebug() << buffer;
            double x, y, z;

            QTextStream stream(buffer);
            stream >> x >> y >> z;

            //std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
            //qDebug() << "x: " << x << ", y: " << y << ", z: " << z;
            pointCloud->addNewDataPoint(x, y, z);
        }

        int imgSize;
        recv(serverSocket, &imgSize, sizeof(int), 0);
        qDebug() << "imgSize: " << imgSize;

        std::string save_path = "received_files/original/" + std::to_string(round-1) + ".jpg";
        int fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
            int remaining = std::min(BUFFER_SIZE, imgSize - i);
            memset(buffer, '\0', BUFFER_SIZE);
            recv(serverSocket, buffer, remaining, 0);

            // Write the received data to the file
            write(fileDescriptor, buffer, remaining);

            send(serverSocket, buffer, sizeof(buffer), 0);
        }
        // Close the file and socket
        close(fileDescriptor);

        imgSize;
        recv(serverSocket, &imgSize, sizeof(int), 0);
        qDebug() << "imgSize: " << imgSize;

        save_path = "received_files/final/" + std::to_string(round-1) + ".jpg";
        fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
            int remaining = std::min(BUFFER_SIZE, imgSize - i);
            memset(buffer, '\0', BUFFER_SIZE);
            recv(serverSocket, buffer, remaining, 0);

            // Write the received data to the file
            write(fileDescriptor, buffer, remaining);

            send(serverSocket, buffer, sizeof(buffer), 0);
        }
        // Close the file and socket
        close(fileDescriptor);

        if(round%5 == 0) {
            pointCloud->reRenderGraph();
        }
        scanner->updateScanner();
        //qDebug() << "----------------";
        //chartView->addNewDataPoint(numOfScannedPoints);

        if(round == numOfRounds) {
            break;
        }
    }
    pointCloud->reRenderGraph();
    qDebug() << "end of readdata thread" ;

    if(!isCancelled) {
        int objSize;
        recv(serverSocket, &objSize, sizeof(int), 0);
        qDebug() << "objSize: " << objSize;

        // Receive the .obj content in packages of size 1024
        std::string objContent;
        int chunkSize = 1024;
        char objBuffer[chunkSize];
        for (int i = 0; i < objSize; i += chunkSize) {
            int remaining = std::min(chunkSize, objSize - i);
            memset(objBuffer, '\0', chunkSize);
            recv(serverSocket, objBuffer, remaining, 0);
            objContent += objBuffer;

            // Send a message to the server to keep them in sync
            send(serverSocket, "ACK", 3, 0);
        }

        // Write the .obj content to a local file
        std::ofstream objFile("received_files/3d.obj");
        objFile << objContent;
        objFile.close();

        openGlWidget->loadModel("received_files/3d.obj");
    }

    scanner->stopTimer();
    scanner->updateScanner();
}

/*
 * initialize IDLE 256 100 333.000000 225.000000 337.000000 760.000000
 * scanner_state IDLE/RUNNING/CANCELLED/FINISHED
 * command start
 * command cancel
 * command finish
 */
int Communication::readFromScanner() {
    char buffer[BUFFER_SIZE];
    float x, y;

    // Wait for the config to be set
    std::unique_lock<std::mutex> lock(configMutex);
    configSetCondition.wait(lock, [] { return isConfigSet; });

    qDebug() << "broadcastSocket: " << broadcastSocket;

    while(true) {
        memset(buffer, '\0', BUFFER_SIZE);
        int bytesRead = recv(broadcastSocket, buffer, BUFFER_SIZE, 0);
        if(bytesRead == 0) {
            continue;
        }
        char* token = strtok(buffer, " ");
        if(strcmp(token, "initialize") == 0) {
            token = strtok(NULL, " "); // IDLE/RUNNING/CANCELLED/FINISHED
            if(strcmp(token, "start") == 0) {
                scanner->setScannerState(ScannerState::RUNNING);
            } else if(strcmp(token, "cancel") == 0) {
                scanner->setScannerState(ScannerState::CANCELLED);
            } else if(strcmp(token, "finish") == 0) {
                scanner->setScannerState(ScannerState::FINISHED);
            }
            token = strtok(NULL, " "); // horizontal_precision
            scanner->setHorizontalPrecision(atoi(token));
            token = strtok(NULL, " ");
            scanner->setVerticalPrecision(atoi(token));
            token = strtok(NULL, " "); x = atof(token);
            token = strtok(NULL, " "); y = atof(token);
            scanner->setTopLeftPoint(new QPoint(x, y));
            token = strtok(NULL, " "); x = atof(token);
            token = strtok(NULL, " "); y = atof(token);
            scanner->setBottomRightPoint(new QPoint(x, y));
        } else if(strcmp(token, "scanner_state") == 0) {
            token = strtok(NULL, " "); // IDLE/RUNNING/CANCELLED/FINISHED
            if(strcmp(token, "RUNNING") == 0) {
                scanner->setScannerState(ScannerState::RUNNING);
                scanner->setCurrentStep(0);
                std::thread t1(Communication::readData);
                t1.detach();
            } else if(strcmp(token, "CANCELLED") == 0) {
                scanner->setScannerState(ScannerState::CANCELLED);
            } else if(strcmp(token, "FINISHED") == 0) {
                scanner->setScannerState(ScannerState::FINISHED);
            }
            scanner->updateScanner();
        }
    }
}

int Communication::sendConfig(const char* command) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "command %s", command);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);
}

int Communication::sendConfig(int horizontalPrecision, int verticalPrecision)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "precision %d %d", horizontalPrecision, verticalPrecision);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);
}

int Communication::sendConfig(const QPolygon& calibrationPolygon)
{
    float top_left_x = calibrationPolygon[0].x();
    float top_left_y = calibrationPolygon[0].y();
    float top_right_x = calibrationPolygon[1].x();
    float top_right_y = calibrationPolygon[1].y();
    float bottom_right_x = calibrationPolygon[2].x();
    float bottom_right_y = calibrationPolygon[2].y();
    float bottom_left_x = calibrationPolygon[3].x();
    float bottom_left_y = calibrationPolygon[3].y();

    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "four_points %f %f %f %f %f %f %f %f", top_left_x, top_left_y, top_right_x, top_right_y, bottom_right_x, bottom_right_y, bottom_left_x, bottom_left_y);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);
}
