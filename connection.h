#ifndef CONNECTION_H
#define CONNECTION_H

#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QRegularExpressionValidator>
#include <QValidator>
#include <QMessageBox>

#include "scanner.h"

class Connection : public QWidget
{
    Q_OBJECT
public:
    explicit Connection(Scanner *scanner, QWidget *parent = nullptr);

private:
    Scanner *scanner;
    QString ipAddress;
    QLineEdit *ipAddressInput;
    QVBoxLayout *layout;

signals:

};

#endif // CONNECTION_H
