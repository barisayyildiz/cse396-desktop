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
            this->scanner->setConnected(true);
            this->scanner->updateScanner();

            // connect to server
            struct sockaddr_in server_addr;
            struct sockaddr_in config_addr;
            struct sockaddr_in broadcast_addr;

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

            qDebug() << "connection: serverSocket: " << serverSocket;

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            server_addr.sin_port = htons(SERVER_PORT);

            config_addr.sin_family = AF_INET;
            config_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            config_addr.sin_port = htons(CONFIG_PORT);

            broadcast_addr.sin_family = AF_INET;
            broadcast_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            broadcast_addr.sin_port = htons(BROADCAST_PORT);

            if (::connect(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(configSocket, (struct sockaddr*)&config_addr, sizeof(config_addr)) == -1) {
                perror("Connection failed");
            }
            if (::connect(broadcastSocket, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr)) == -1) {
                perror("Connection failed");
            }

            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE);
            sprintf(buffer, "desktop");
            send(serverSocket, buffer, BUFFER_SIZE, 0);

            // read initial scanner state
            memset(buffer, BUFFER_SIZE, 0);
            recv(serverSocket, buffer, BUFFER_SIZE, 0);
            qDebug() << buffer;

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
                }
                token = strtok(NULL, " ");
            }
            emit scannerStateReceived();
            Communication::setConfig();
        } else {
            QMessageBox::critical(this, "Invalid IP Address", "Please enter a valid IP address.");
        }
    });

}
