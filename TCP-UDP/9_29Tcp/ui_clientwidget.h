/********************************************************************************
** Form generated from reading UI file 'clientwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTWIDGET_H
#define UI_CLIENTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_clientwidget
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEditIp;
    QPushButton *pushButtonConnect;
    QLabel *label_2;
    QLineEdit *lineEditPort;
    QTextEdit *textEditRead;
    QTextEdit *textEditWrite;
    QPushButton *pushButtonSend;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonClose;

    void setupUi(QWidget *clientwidget)
    {
        if (clientwidget->objectName().isEmpty())
            clientwidget->setObjectName(QString::fromUtf8("clientwidget"));
        clientwidget->resize(400, 300);
        gridLayout = new QGridLayout(clientwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(clientwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEditIp = new QLineEdit(clientwidget);
        lineEditIp->setObjectName(QString::fromUtf8("lineEditIp"));

        gridLayout->addWidget(lineEditIp, 0, 1, 1, 1);

        pushButtonConnect = new QPushButton(clientwidget);
        pushButtonConnect->setObjectName(QString::fromUtf8("pushButtonConnect"));

        gridLayout->addWidget(pushButtonConnect, 0, 2, 2, 1);

        label_2 = new QLabel(clientwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEditPort = new QLineEdit(clientwidget);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));

        gridLayout->addWidget(lineEditPort, 1, 1, 1, 1);

        textEditRead = new QTextEdit(clientwidget);
        textEditRead->setObjectName(QString::fromUtf8("textEditRead"));

        gridLayout->addWidget(textEditRead, 2, 0, 1, 3);

        textEditWrite = new QTextEdit(clientwidget);
        textEditWrite->setObjectName(QString::fromUtf8("textEditWrite"));

        gridLayout->addWidget(textEditWrite, 3, 0, 1, 3);

        pushButtonSend = new QPushButton(clientwidget);
        pushButtonSend->setObjectName(QString::fromUtf8("pushButtonSend"));

        gridLayout->addWidget(pushButtonSend, 4, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(203, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 4, 1, 1, 1);

        pushButtonClose = new QPushButton(clientwidget);
        pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));

        gridLayout->addWidget(pushButtonClose, 4, 2, 1, 1);


        retranslateUi(clientwidget);

        QMetaObject::connectSlotsByName(clientwidget);
    } // setupUi

    void retranslateUi(QWidget *clientwidget)
    {
        clientwidget->setWindowTitle(QApplication::translate("clientwidget", "Form", nullptr));
        label->setText(QApplication::translate("clientwidget", "\346\234\215\345\212\241\345\231\250ip\357\274\232", nullptr));
        lineEditIp->setText(QApplication::translate("clientwidget", "127.0.0.1", nullptr));
        pushButtonConnect->setText(QApplication::translate("clientwidget", "connect", nullptr));
        label_2->setText(QApplication::translate("clientwidget", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243\345\217\267\357\274\232", nullptr));
        lineEditPort->setText(QApplication::translate("clientwidget", "8888", nullptr));
        pushButtonSend->setText(QApplication::translate("clientwidget", "send", nullptr));
        pushButtonClose->setText(QApplication::translate("clientwidget", "close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class clientwidget: public Ui_clientwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTWIDGET_H
