#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H

#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>

class EditableLabel : public QWidget 
{
    Q_OBJECT
        Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    EditableLabel(QWidget* parent = nullptr) :
        QWidget(parent),
        mLabel(new QLabel),
        mLineEdit(new QLineEdit)
    {
        setLayout(new QVBoxLayout);
        layout()->setSpacing(0);
        layout()->addWidget(&stacked);

        stacked.addWidget(mLabel);
        stacked.addWidget(mLineEdit);
        mLabel->installEventFilter(this);
        mLineEdit->installEventFilter(this);
        setSizePolicy(mLineEdit->sizePolicy());

        mLabel->setText("FileName");

        connect(mLineEdit, &QLineEdit::textChanged, this, &EditableLabel::setText);
    }

    bool eventFilter(QObject* watched, QEvent* event) {
        if (watched == mLineEdit) {
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
                if (keyEvent->key() == Qt::Key_Return ||
                    keyEvent->key() == Qt::Key_Escape ||
                    keyEvent->key() == Qt::Key_Enter)
                {
                    mLabel->setText(mLineEdit->text());
                    stacked.setCurrentIndex(0);
                }
            }
            else if (event->type() == QEvent::FocusOut) {
                mLabel->setText(mLineEdit->text());
                stacked.setCurrentIndex(0);
            }
        }
        else if (watched == mLabel) {
            if (event->type() == QEvent::MouseButtonDblClick) {
                stacked.setCurrentIndex(1);
                mLineEdit->setText(mLabel->text());
                mLineEdit->setFocus();
            }
        }
        return QWidget::eventFilter(watched, event);
    }
    QString text() const 
    {
        return mText;
    }
    void setText(const QString& text) 
    {
        if (text == mText)
            return;
        mText == text;
        emit textChanged(mText);
    }
signals:
    void textChanged(const QString& text);
private:
    QLabel* mLabel;
    QLineEdit* mLineEdit;
    QStackedWidget stacked;
    QString mText;
};

#endif
