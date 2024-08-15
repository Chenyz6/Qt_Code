#include "firstpagewidget.h"
#include "controller/firstpagecontroller.h"
#include "controller/secondpagecontroller.h"
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>

QFirstPageWidget::QFirstPageWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void QFirstPageWidget::setupUI()
{
    QPushButton *firstPage = new QPushButton(tr("我是第一页的页面：点击我实现2+3计算功能（业务逻辑功能）"));
    connect(firstPage,SIGNAL(clicked(bool)),this,SLOT(showOneFunctionUISlot()));
    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(firstPage);
    mainLayout->addStretch();

    /*注意：这个绑定不能放在按钮那里，否则会出现第n次点击按钮，然后发射n次信号，执行n次槽函数*/
    connect(QFirstPageController::GetInstance(),SIGNAL(showFinishedCountUI_signal()),this,SLOT(showFinishedCountResultUISlot()));
}

void QFirstPageWidget::showOneFunctionUISlot()
{
    /*摁下按钮，切换到新的界面*/
    QFirstPageController::GetInstance()->startedCount_signal();
}

void QFirstPageWidget::showFinishedCountResultUISlot()
{
    qDebug()<<tr("显示计算后界面！！！");
    QMessageBox::warning(this,QStringLiteral("结果界面"),QStringLiteral("显示计算后界面！！！"),QMessageBox::Yes,QMessageBox::No);
}
