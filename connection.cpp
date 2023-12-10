#include "connection.h"

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

    connect(submitButton, &QPushButton::pressed, [this, validator, regEx]() {
        QString inputText = ipAddressInput->text();
        QRegularExpressionMatch match = regEx.match(inputText, 0, QRegularExpression::PartialPreferCompleteMatch);
        bool hasMatch = match.hasMatch();
        if (hasMatch) {
            QMessageBox msgBox;
            msgBox.setText("Connected to the Scanner!");
            msgBox.exec();
            this->scanner->setConnected(true);
            this->scanner->updateScanner();
        } else {
            QMessageBox::critical(this, "Invalid IP Address", "Please enter a valid IP address.");
        }
    });

}
