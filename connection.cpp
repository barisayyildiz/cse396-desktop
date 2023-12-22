#include "connection.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int clientSocket;

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

    connect(submitButton, &QPushButton::pressed, [this, validator, regEx, clientSocket]() {
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

            // update clientSocket
            if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }

            qDebug() << "connection: clientSocket: " << clientSocket;

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            server_addr.sin_port = htons(3000);

            if (::connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
                perror("Connection failed");
            }
        } else {
            QMessageBox::critical(this, "Invalid IP Address", "Please enter a valid IP address.");
        }
    });

}
