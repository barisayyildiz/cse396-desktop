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
Calibration* Communication::calibration = nullptr;

bool Communication::isConfigSet = false;
std::mutex Communication::configMutex;
std::condition_variable Communication::configSetCondition;

// New definitions for readImageForCalibration
bool Communication::isCalibrationConfigSet = false;
std::mutex Communication::calibrationConfigMutex;
std::condition_variable Communication::calibrationConfigSetCondition;

bool Communication::isLive = false;
std::mutex Communication::liveMutex;
std::condition_variable Communication::liveCondition;

int clientSocket;
int serverSocket;
int configSocket;
int broadcastSocket;
int calibrationImageSocket;
int liveSocket;

void Communication::setConfig() {
    // Implement the code to set the configuration (broadcastConfig)

    // Signal that the config is set
    std::unique_lock<std::mutex> configLock(configMutex, std::defer_lock);
    std::unique_lock<std::mutex> calibrationLock(calibrationConfigMutex, std::defer_lock);
    std::unique_lock<std::mutex> liveLock(liveMutex, std::defer_lock);

    std::lock(configLock, calibrationLock, liveLock); // Lock both mutexes at once

    isConfigSet = true;
    isCalibrationConfigSet = true;
    isLive = true;

    configLock.unlock();
    calibrationLock.unlock();
    liveLock.unlock();

    // Notify waiting threads
    configSetCondition.notify_one();
    calibrationConfigSetCondition.notify_one();
    liveCondition.notify_one();
}

void Communication::readLiveData() {
    //char buffer[BUFFER_SIZE];
    std::vector<char> buffer(BUFFER_SIZE, '\0');
    int round;
    int numOfRounds;

    // Wait for the config to be set
    std::unique_lock<std::mutex> lock(liveMutex);
    liveCondition.wait(lock, [] { return isLive; });

    bool inSync = false;

    while(true) {
        /*memset(buffer, '\0', BUFFER_SIZE);
        int bytesRead = recv(liveSocket, buffer, BUFFER_SIZE, 0);
        if(bytesRead == 0) {
            continue;
        }
        qDebug() << "bytesRead:" << bytesRead;
        qDebug() << "buffer:" << buffer;
        char* token = strtok(buffer, " ");
        qDebug() << "token:" << token;
*/
        std::fill(buffer.begin(), buffer.end(), '\0');
        // Receive data
        int bytesRead = recv(liveSocket, buffer.data(), BUFFER_SIZE, 0);

        if (bytesRead <= 0) {
            // Handle error or connection closed
            break;
        }

        std::cout << "bytesRead: " << bytesRead << std::endl;
        std::cout << "buffer: " << buffer.data() << std::endl;

        std::istringstream iss(buffer.data());
        std::string token;

        iss >> token;

        if(token == "START_SCANNING") {
            inSync = true;
            scanner->startTimer();
            pointCloud->resetPointCloud();
            scanner->updateScanner();
            send(liveSocket, "ack", 3, 0);
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            continue;
        } else if(token == "ROUND") {
            qDebug() << "round";
            send(liveSocket, "ack", 3, 0);
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            //scanner->updateScanner();
            //token = strtok(NULL, " ");
            iss >> round;
            //round = atoi(token);
            qDebug() << "currentStep: " << round;
            scanner->setCurrentStep(round);
            //token = strtok(NULL, " ");
            //qDebug() << "token: " << token;
            iss >> numOfRounds;
            //numOfRounds = atoi(token);
            //scanner->setHorizontalPrecision(atoi(token));
            scanner->setHorizontalPrecision(numOfRounds);
            qDebug() << "round number: " << round;
            if(round%5 == 0) {
                pointCloud->reRenderGraph();
            }
        } else if(token == "NUMBER_OF_VERTICES") {
            qDebug() << "number of vertices";
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            //token = strtok(NULL, " ");
            //int numOfScannedPoints = atoi(token);
            int numOfScannedPoints;
            iss >> numOfScannedPoints;
            qDebug() << "number of scanned points : " << numOfScannedPoints;
            scannedPoints->addNewDataPoint(numOfScannedPoints);
            scanner->setNumberOfPointsScanned(scanner->getNumberOfPointsScanned() + numOfScannedPoints);
            send(liveSocket, "ack", 3, 0);
            for(int i=0; i<numOfScannedPoints; i++) {
                //memset(buffer, '\0', BUFFER_SIZE);
                //recv(liveSocket, buffer, BUFFER_SIZE, 0);
                std::fill(buffer.begin(), buffer.end(), '\0');
                recv(liveSocket, buffer.data(), BUFFER_SIZE, 0);

                //send(liveSocket, buffer, sizeof(buffer), 0);

                double x, y, z;
                std::istringstream iss(buffer.data());
                iss >> x >> y >> z;

                //QTextStream stream(buffer);
                //stream >> x >> y >> z;
                qDebug() << "x: " << x << ", y: " << y << ", z: " << z;

                pointCloud->addNewDataPoint(x, y, z);
                //pointCloud->reRenderGraph();
                send(liveSocket, "ack", 3, 0);
            }
        }
        else if(token == "IMAGES") {
            int imgSize;
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            send(liveSocket, "ack", 3, 0);
            recv(liveSocket, &imgSize, sizeof(int), 0);
            send(liveSocket, "ack", 3, 0);
            qDebug() << "imgSize: " << imgSize;

            std::string save_path = "received_files/original/" + std::to_string(round-1) + ".jpg";
            int fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
                int remaining = std::min(BUFFER_SIZE, imgSize - i);
                //memset(buffer, '\0', BUFFER_SIZE);
                std::fill(buffer.begin(), buffer.end(), '\0');
                recv(liveSocket, buffer.data(), remaining, 0);
                //recv(liveSocket, buffer, remaining, 0);

                // Write the received data to the file
                //write(fileDescriptor, buffer, remaining);
                write(fileDescriptor, buffer.data(), remaining);

                //send(liveSocket, buffer, sizeof(buffer), 0);
                send(liveSocket, "ack", 3, 0);
            }
            // Close the file and socket
            close(fileDescriptor);

            qDebug() << "first image is done...";

            imgSize;
            recv(liveSocket, &imgSize, sizeof(int), 0);
            send(liveSocket, "ack", 3, 0);
            qDebug() << "imgSize: " << imgSize;

            save_path = "received_files/final/" + std::to_string(round-1) + ".jpg";
            fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
                int remaining = std::min(BUFFER_SIZE, imgSize - i);
                //memset(buffer, '\0', BUFFER_SIZE);
                //recv(liveSocket, buffer, remaining, 0);
                std::fill(buffer.begin(), buffer.end(), '\0');
                recv(liveSocket, buffer.data(), remaining, 0);

                // Write the received data to the file
                //write(fileDescriptor, buffer, remaining);
                write(fileDescriptor, buffer.data(), remaining);

                //send(liveSocket, buffer, sizeof(buffer), 0);
                send(liveSocket, "ack", 3, 0);
            }
            // Close the file and socket
            close(fileDescriptor);
            scanner->updateScanner();
            qDebug() << "second image is done...";
        } else if(token == "FINISH_SCANNING") {
            send(liveSocket, "ack", 3, 0);
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            scanner->updateScanner();
            pointCloud->reRenderGraph();
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
        } else if(token == "FILE") {
            send(liveSocket, "ack", 3, 0);
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            //token = strtok(NULL, " ");
            int objSize;
            iss >> objSize;
            //int objSize = atoi(token);
            //recv(liveSocket, &objSize, sizeof(int), 0);
            qDebug() << "objSize: " << objSize;

            // Receive the .obj content in packages of size 1024
            std::string objContent;
            int chunkSize = BUFFER_SIZE;
            char objBuffer[chunkSize];
            for (int i = 0; i < objSize; i += chunkSize) {
                int remaining = std::min(chunkSize, objSize - i);
                memset(objBuffer, '\0', chunkSize);
                recv(liveSocket, objBuffer, remaining, 0);
                objContent += objBuffer;

                // Send a message to the server to keep them in sync
                //send(liveSocket, buffer, BUFFER_SIZE, 0);
                send(liveSocket, "ack", 3, 0);
            }

            // Write the .obj content to a local file
            std::ofstream objFile("received_files/3d.obj");
            objFile << objContent;
            objFile.close();

            openGlWidget->loadModel("received_files/3d.obj");
            scanner->updateScanner();
        } else if(token == "FILE_END") {
            //send(liveSocket, buffer, BUFFER_SIZE, 0);
            send(liveSocket, "ack", 3, 0);
        } else {
            continue;
        }
    }
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
            qDebug() << token;
            if(strcmp(token, "RUNNING") == 0) {
                scanner->setScannerState(ScannerState::RUNNING);
                scanner->setCurrentStep(0);
                //std::thread t1(Communication::readData);
                //t1.detach();
            } else if(strcmp(token, "CANCELLED") == 0) {
                scanner->setScannerState(ScannerState::CANCELLED);
            } else if(strcmp(token, "FINISHED") == 0) {
                scanner->setScannerState(ScannerState::FINISHED);
            }
            scanner->updateScanner();
        } else if(strcmp(token, "four_points") == 0) {
            // four_points 0.0 0.0 100.0 100.0
            token = strtok(NULL, " ");
            x = atof(token);
            token = strtok(NULL, " ");
            y = atof(token);
            scanner->setTopLeftPoint(new QPoint(x, y));
            token = strtok(NULL, " ");
            x = atof(token);
            token = strtok(NULL, " ");
            y = atof(token);
            scanner->setBottomRightPoint(new QPoint(x, y));
            scanner->updateScanner();
        }
    }
}

int Communication::readImageForCalibration() {
    char buffer[BUFFER_SIZE];

    // Wait for the config to be set
    std::unique_lock<std::mutex> lock(calibrationConfigMutex);
    calibrationConfigSetCondition.wait(lock, [] { return isCalibrationConfigSet; });

    while(true) {
        int imgSize;
        recv(calibrationImageSocket, &imgSize, sizeof(int), 0);
        qDebug() << "imgSize: " << imgSize;

        std::string save_path = "received_files/calibration.jpg";
        int fileDescriptor = open(save_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        for (int i = 0; i < imgSize; i += BUFFER_SIZE) {
            int remaining = std::min(BUFFER_SIZE, imgSize - i);
            memset(buffer, '\0', BUFFER_SIZE);
            recv(calibrationImageSocket, buffer, remaining, 0);

            // Write the received data to the file
            write(fileDescriptor, buffer, remaining);

            send(calibrationImageSocket, buffer, sizeof(buffer), 0);
        }
        // Close the file and socket
        close(fileDescriptor);
        calibration->reloadImage();
    }
    return 0;
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
    float bottom_right_x = calibrationPolygon[1].x();
    float bottom_right_y = calibrationPolygon[1].y();

    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    sprintf(buffer, "four_points %f %f %f %f", top_left_x, top_left_y, bottom_right_x, bottom_right_y);
    qDebug() << buffer;
    send(configSocket, buffer, BUFFER_SIZE, 0);
}
