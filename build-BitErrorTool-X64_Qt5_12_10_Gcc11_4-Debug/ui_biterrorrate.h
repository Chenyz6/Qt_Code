/********************************************************************************
** Form generated from reading UI file 'biterrorrate.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BITERRORRATE_H
#define UI_BITERRORRATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BitErrorRate
{
public:
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_3;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QLabel *label_7;
    QLineEdit *lineEdit_biterrornum;
    QLineEdit *lineEdit_bitlength;
    QLabel *label;
    QLineEdit *lineEdit_ber;
    QLabel *label_2;
    QLineEdit *lineEdit_bitnum;
    QLabel *label_5;
    QProgressBar *progressBar;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLineEdit *lineEdit_filepath;
    QPushButton *pushButton_filefile;
    QLabel *label_6;
    QLineEdit *lineEdit_bitpath;
    QPushButton *pushButton_bitfilepath;
    QLabel *label_4;
    QLineEdit *lineEdit_start;
    QPushButton *pushButton_start;

    void setupUi(QWidget *BitErrorRate)
    {
        if (BitErrorRate->objectName().isEmpty())
            BitErrorRate->setObjectName(QString::fromUtf8("BitErrorRate"));
        BitErrorRate->resize(632, 510);
        gridLayout_4 = new QGridLayout(BitErrorRate);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        widget = new QWidget(BitErrorRate);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout_3->addWidget(widget, 0, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_7 = new QLabel(BitErrorRate);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 6, 0, 1, 1);

        lineEdit_biterrornum = new QLineEdit(BitErrorRate);
        lineEdit_biterrornum->setObjectName(QString::fromUtf8("lineEdit_biterrornum"));
        lineEdit_biterrornum->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_biterrornum, 3, 0, 1, 1);

        lineEdit_bitlength = new QLineEdit(BitErrorRate);
        lineEdit_bitlength->setObjectName(QString::fromUtf8("lineEdit_bitlength"));
        lineEdit_bitlength->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_bitlength, 1, 0, 1, 1);

        label = new QLabel(BitErrorRate);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 2, 0, 1, 1);

        lineEdit_ber = new QLineEdit(BitErrorRate);
        lineEdit_ber->setObjectName(QString::fromUtf8("lineEdit_ber"));
        lineEdit_ber->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_ber, 7, 0, 1, 1);

        label_2 = new QLabel(BitErrorRate);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 4, 0, 1, 1);

        lineEdit_bitnum = new QLineEdit(BitErrorRate);
        lineEdit_bitnum->setObjectName(QString::fromUtf8("lineEdit_bitnum"));
        lineEdit_bitnum->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_bitnum, 5, 0, 1, 1);

        label_5 = new QLabel(BitErrorRate);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 0, 1, 1, 1);

        gridLayout_3->setColumnStretch(0, 3);
        gridLayout_3->setColumnStretch(1, 2);

        gridLayout_4->addLayout(gridLayout_3, 2, 0, 1, 1);

        progressBar = new QProgressBar(BitErrorRate);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        gridLayout_4->addWidget(progressBar, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(BitErrorRate);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        lineEdit_filepath = new QLineEdit(groupBox_2);
        lineEdit_filepath->setObjectName(QString::fromUtf8("lineEdit_filepath"));

        gridLayout->addWidget(lineEdit_filepath, 0, 1, 1, 1);

        pushButton_filefile = new QPushButton(groupBox_2);
        pushButton_filefile->setObjectName(QString::fromUtf8("pushButton_filefile"));

        gridLayout->addWidget(pushButton_filefile, 0, 2, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 1, 0, 1, 1);

        lineEdit_bitpath = new QLineEdit(groupBox_2);
        lineEdit_bitpath->setObjectName(QString::fromUtf8("lineEdit_bitpath"));

        gridLayout->addWidget(lineEdit_bitpath, 1, 1, 1, 1);

        pushButton_bitfilepath = new QPushButton(groupBox_2);
        pushButton_bitfilepath->setObjectName(QString::fromUtf8("pushButton_bitfilepath"));

        gridLayout->addWidget(pushButton_bitfilepath, 1, 2, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);

        lineEdit_start = new QLineEdit(groupBox_2);
        lineEdit_start->setObjectName(QString::fromUtf8("lineEdit_start"));

        gridLayout->addWidget(lineEdit_start, 2, 1, 1, 1);

        pushButton_start = new QPushButton(groupBox_2);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));

        gridLayout->addWidget(pushButton_start, 2, 2, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 0, 0, 1, 1);


        retranslateUi(BitErrorRate);

        QMetaObject::connectSlotsByName(BitErrorRate);
    } // setupUi

    void retranslateUi(QWidget *BitErrorRate)
    {
        BitErrorRate->setWindowTitle(QApplication::translate("BitErrorRate", "Form", nullptr));
        label_7->setText(QApplication::translate("BitErrorRate", "\350\257\257\347\240\201\347\216\207:", nullptr));
        label->setText(QApplication::translate("BitErrorRate", "\351\224\231\350\257\257\347\240\201\345\205\203\344\270\252\346\225\260:", nullptr));
        lineEdit_ber->setText(QString());
        label_2->setText(QApplication::translate("BitErrorRate", "\347\273\237\350\256\241\347\240\201\345\205\203\344\270\252\346\225\260:", nullptr));
        lineEdit_bitnum->setText(QString());
        label_5->setText(QApplication::translate("BitErrorRate", "\346\257\224\347\211\271\345\272\217\345\210\227\351\225\277\345\272\246:", nullptr));
        groupBox_2->setTitle(QApplication::translate("BitErrorRate", "\350\257\257\347\240\201\347\216\207\346\265\213\350\257\225\345\217\202\346\225\260", nullptr));
        label_3->setText(QApplication::translate("BitErrorRate", "\346\265\213\350\257\225\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        lineEdit_filepath->setPlaceholderText(QApplication::translate("BitErrorRate", "\350\257\267\350\276\223\345\205\245\346\210\226\346\265\217\350\247\210\350\267\257\345\276\204", nullptr));
        pushButton_filefile->setText(QApplication::translate("BitErrorRate", "\346\265\217\350\247\210", nullptr));
        label_6->setText(QApplication::translate("BitErrorRate", "\346\257\224\347\211\271\345\272\217\345\210\227\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        lineEdit_bitpath->setPlaceholderText(QApplication::translate("BitErrorRate", "\350\257\267\350\276\223\345\205\245\346\210\226\346\265\217\350\247\210\350\267\257\345\276\204", nullptr));
        pushButton_bitfilepath->setText(QApplication::translate("BitErrorRate", "\346\265\217\350\247\210", nullptr));
        label_4->setText(QApplication::translate("BitErrorRate", "\350\265\267\345\247\213\344\275\215\347\275\256[Bit]", nullptr));
        lineEdit_start->setText(QApplication::translate("BitErrorRate", "0", nullptr));
        pushButton_start->setText(QApplication::translate("BitErrorRate", "\346\265\213\350\257\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BitErrorRate: public Ui_BitErrorRate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BITERRORRATE_H
