#include "secondpagewidget.h"
#include <QPushButton>

QSecondPageWidget::QSecondPageWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void QSecondPageWidget::setupUI()
{
    QPushButton *firstPage = new QPushButton(tr("我是第二页的页面：点击我实现2*3计算功能（业务逻辑功能）"));
    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(firstPage);
    mainLayout->addStretch();
}
