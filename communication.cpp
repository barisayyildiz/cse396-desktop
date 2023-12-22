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

int dataSocket;
int configSocket;

void Communication::readData()
{
    scanner->startTimer();
    char buffer[BUFFER_SIZE];
    bool isCancelled = false;
    pointCloud->resetPointCloud();

    while (true) {
        // Accept incoming connections
        qDebug() << "accepting...";

        memset(buffer, '\0', BUFFER_SIZE);
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(clientSocket);
            break;
        }

        if(strcmp(buffer, "CANCEL") == 0) {
            send(clientSocket, buffer, sizeof(buffer), 0);
            isCancelled = true;
            break;
        }

        if(strcmp(buffer, "FINISHED") == 0) {
            break;
        }

        char* token;
        token = strtok(buffer, " ");

        //qDebug() << "token: " << token;
        int round = atoi(token);
        qDebug() << "currentStep: " << round;
        scanner->setCurrentStep(round);
        token = strtok(nullptr, " ");
        scanner->setHorizontalPrecision(atoi(token));
        //qDebug() << "round number: " << round;

        memset(buffer, '\0', BUFFER_SIZE);
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            qDebug() << "Error receiving data.";
            close(clientSocket);
            break;
        }

        int numOfScannedPoints = atoi(buffer);
        //qDebug() << "number of scanned points : " << numOfScannedPoints;

        scannedPoints->addNewDataPoint(numOfScannedPoints);
        scanner->setNumberOfPointsScanned(scanner->getNumberOfPointsScanned() + numOfScannedPoints);

        /*ScannerState scannerState;
        int verticalPrecision;
        int numberOfPointsScanned;*/

        for(int i=0; i<numOfScannedPoints; i++) {
            memset(buffer, '\0', BUFFER_SIZE);
            recv(clientSocket, buffer, sizeof(buffer), 0);
            send(clientSocket, buffer, sizeof(buffer), 0);
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
        recv(clientSocket, &imgSize, sizeof(int), 0);
        qDebug() << "imgSize: " << imgSize;

        std::string save_path = "received_files/original/" + std::to_string(round-1) + ".jpg";
        int fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
            int remaining = std::min(BUFFER_SIZE, imgSize - i);
            memset(buffer, '\0', BUFFER_SIZE);
            recv(clientSocket, buffer, remaining, 0);

            // Write the received data to the file
            write(fileDescriptor, buffer, remaining);

            send(clientSocket, buffer, sizeof(buffer), 0);
        }
        // Close the file and socket
        close(fileDescriptor);

        imgSize;
        recv(clientSocket, &imgSize, sizeof(int), 0);
        qDebug() << "imgSize: " << imgSize;

        save_path = "received_files/final/" + std::to_string(round-1) + ".jpg";
        fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
            int remaining = std::min(BUFFER_SIZE, imgSize - i);
            memset(buffer, '\0', BUFFER_SIZE);
            recv(clientSocket, buffer, remaining, 0);

            // Write the received data to the file
            write(fileDescriptor, buffer, remaining);

            send(clientSocket, buffer, sizeof(buffer), 0);
        }
        // Close the file and socket
        close(fileDescriptor);

        if(round%5 == 0) {
            pointCloud->reRenderGraph();
        }
        scanner->updateScanner();
        //qDebug() << "----------------";
        //chartView->addNewDataPoint(numOfScannedPoints);
    }
    pointCloud->reRenderGraph();
    qDebug() << "end of readdata thread" ;

    if(!isCancelled) {
        int objSize;
        recv(clientSocket, &objSize, sizeof(int), 0);
        qDebug() << "objSize: " << objSize;

        // Receive the .obj content in packages of size 1024
        std::string objContent;
        int chunkSize = 1024;
        char objBuffer[chunkSize];
        for (int i = 0; i < objSize; i += chunkSize) {
            int remaining = std::min(chunkSize, objSize - i);
            memset(objBuffer, '\0', chunkSize);
            recv(clientSocket, objBuffer, remaining, 0);
            objContent += objBuffer;

            // Send a message to the server to keep them in sync
            send(clientSocket, "ACK", 3, 0);
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

int Communication::sendConfig(const char* command) {
    qDebug() << "Gonna send some configurations";
    configSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(configSocket == -1) {
        qDebug() << "Error creating client socket";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(CONFIG_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(configSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server." << std::endl;
        return 1;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "command %s", command);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);

    close(configSocket);
}

int Communication::sendConfig(int horizontalPrecision, int verticalPrecision)
{
    qDebug() << "Gonna send some configurations";
    configSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(configSocket == -1) {
        qDebug() << "Error creating client socket";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(CONFIG_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(configSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server." << std::endl;
        return 1;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "precision %d %d", horizontalPrecision, verticalPrecision);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);

    close(configSocket);

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

    qDebug() << "Gonna send some configurations";
    configSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(configSocket == -1) {
        qDebug() << "Error creating client socket";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(CONFIG_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(configSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server for sendConfig." << std::endl;
        return 1;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "four_points %f %f %f %f %f %f %f %f", top_left_x, top_left_y, top_right_x, top_right_y, bottom_right_x, bottom_right_y, bottom_left_x, bottom_left_y);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);

    close(configSocket);
}
