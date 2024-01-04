#include "connection.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "communication.h"

Connection::Connection(Scanner *scanner, QWidget *parent)
    : QWidget{parent}
{
    this->scanner = scanner;
    this->layout = new QVBoxLayout(this);
    this->ipAddressInput = new QLineEdit();
    this->ipAddressInput->setPlaceholderText("Enter Scanner's Ip Address");
    QPushButton *submitButton = new QPushButton("Submit");

    this->layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->layout->addWidget(this->ipAddressInput);
    this->layout->addWidget(submitButton);
    this->layout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QRegularExpression regEx = QRegularExpression("^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$");
    QValidator *validator = new QRegularExpressionValidator(regEx);
    this->ipAddressInput->setValidator(validator);

    this->ipAddressInput->setStyleSheet("margin: 0px 500px; padding: 10px;");
    submitButton->setStyleSheet("margin: 0px 500px; padding: 10px;");

    this->layout->setSpacing(20);
    this->setLayout(this->layout);

    connect(submitButton, &QPushButton::pressed, [this, validator, regEx, serverSocket, configSocket, broadcastSocket]() {
        QString inputText = ipAddressInput->text();
        QRegularExpressionMatch match = regEx.match(inputText, 0, QRegularExpression::PartialPreferCompleteMatch);
        bool hasMatch = match.hasMatch();
        if (hasMatch) {
            QMessageBox msgBox;
            msgBox.setText("Connected to the Scanner!");
            msgBox.exec();

            // connect to server
            struct sockaddr_in server_addr;
            struct sockaddr_in config_addr;
            struct sockaddr_in broadcast_addr;
            struct sockaddr_in calibration_image_addr;
            struct sockaddr_in live_addr;

            // update clientSocket
            if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            if ((configSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            if ((broadcastSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            if ((calibrationImageSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            if ((liveSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            // 192.168.148.153
            // 127.0.0.1


            qDebug() << "connection: serverSocket: " << serverSocket;

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr("192.168.148.153");
            server_addr.sin_port = htons(SERVER_PORT);

            config_addr.sin_family = AF_INET;
            config_addr.sin_addr.s_addr = inet_addr("192.168.148.153");
            config_addr.sin_port = htons(CONFIG_PORT);

            broadcast_addr.sin_family = AF_INET;
            broadcast_addr.sin_addr.s_addr = inet_addr("192.168.148.153");
            broadcast_addr.sin_port = htons(BROADCAST_PORT);

            calibration_image_addr.sin_family = AF_INET;
            calibration_image_addr.sin_addr.s_addr = inet_addr("192.168.148.153");
            calibration_image_addr.sin_port = htons(IMAGE_PORT);

            live_addr.sin_family = AF_INET;
            live_addr.sin_addr.s_addr = inet_addr("192.168.148.153");
            live_addr.sin_port = htons(LIVE_PORT);

            if (::connect(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(configSocket, (struct sockaddr*)&config_addr, sizeof(config_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(broadcastSocket, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(calibrationImageSocket, (struct sockaddr*)&calibration_image_addr, sizeof(calibration_image_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(liveSocket, (struct sockaddr*)&live_addr, sizeof(live_addr)) == -1) {
                perror("Connection failed");
            }


            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE);
            sprintf(buffer, "desktop");
            send(serverSocket, buffer, BUFFER_SIZE, 0);

            // read initial scanner state
            memset(buffer, '\0', 0);
            recv(serverSocket, buffer, BUFFER_SIZE, 0);
            qDebug() << buffer;

            float x, y;

            char tempBuffer[BUFFER_SIZE];
            strcpy(tempBuffer, buffer);
            char *token = strtok(tempBuffer, " ");
            while (token != NULL) {
                if (strcmp(token, "scanner_state") == 0) {
                    token = strtok(NULL, " ");
                    if(strcmp(token, "IDLE") == 0) {
                        this->scanner->setScannerState(ScannerState::IDLE);
                    } else if(strcmp(token, "RUNNING") == 0) {
                        this->scanner->setScannerState(ScannerState::RUNNING);
                    } else if(strcmp(token, "CANCELLED") == 0) {
                        this->scanner->setScannerState(ScannerState::CANCELLED);
                    } else if(strcmp(token, "FINISHED") == 0) {
                        this->scanner->setScannerState(ScannerState::CANCELLED);
                    }
                } else if (strcmp(token, "current_step") == 0) {
                    token = strtok(NULL, " ");
                    this->scanner->setCurrentStep(atoi(token));
                } else if (strcmp(token, "current_horizontal_precision") == 0) {
                    token = strtok(NULL, " ");
                    this->scanner->setHorizontalPrecision(atoi(token));
                } else if (strcmp(token, "current_vertical_precision") == 0) {
                    token = strtok(NULL, " ");
                    this->scanner->setVerticalPrecision(atoi(token));
                } else if (strcmp(token, "four_points") == 0) {
                    token = strtok(NULL, " "); x = atof(token);
                    token = strtok(NULL, " "); y = atof(token);
                    this->scanner->setTopLeftPoint(new QPoint(x, y));
                    token = strtok(NULL, " "); x = atof(token);
                    token = strtok(NULL, " "); y = atof(token);
                    this->scanner->setBottomRightPoint(new QPoint(x, y));
                }
                token = strtok(NULL, " ");
            }
            this->scanner->setConnected(true);
            emit scannerStateReceived();
            Communication::setConfig();
        } else {
            QMessageBox::critical(this, "Invalid IP Address", "Please enter a valid IP address.");
        }
    });

}
