/********************************************************************************
** Form generated from reading UI file 'serverwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWIDGET_H
#define UI_SERVERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_serverwidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButtonSend;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QTextEdit *textEditWrite;
    QTextEdit *textEditRead;

    void setupUi(QWidget *serverwidget)
    {
        if (serverwidget->objectName().isEmpty())
            serverwidget->setObjectName(QString::fromUtf8("serverwidget"));
        serverwidget->resize(417, 309);
        gridLayout = new QGridLayout(serverwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButtonSend = new QPushButton(serverwidget);
        pushButtonSend->setObjectName(QString::fromUtf8("pushButtonSend"));

        gridLayout->addWidget(pushButtonSend, 2, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(617, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 1, 1, 1);

        pushButton = new QPushButton(serverwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 2, 2, 1, 1);

        textEditWrite = new QTextEdit(serverwidget);
        textEditWrite->setObjectName(QString::fromUtf8("textEditWrite"));

        gridLayout->addWidget(textEditWrite, 1, 0, 1, 3);

        textEditRead = new QTextEdit(serverwidget);
        textEditRead->setObjectName(QString::fromUtf8("textEditRead"));
        textEditRead->setReadOnly(true);

        gridLayout->addWidget(textEditRead, 0, 0, 1, 3);


        retranslateUi(serverwidget);

        QMetaObject::connectSlotsByName(serverwidget);
    } // setupUi

    void retranslateUi(QWidget *serverwidget)
    {
        serverwidget->setWindowTitle(QApplication::translate("serverwidget", "serverwidget", nullptr));
        pushButtonSend->setText(QApplication::translate("serverwidget", "send", nullptr));
        pushButton->setText(QApplication::translate("serverwidget", "cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class serverwidget: public Ui_serverwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWIDGET_H
