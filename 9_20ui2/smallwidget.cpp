#include "smallwidget.h"
#include <QSpinBox>  //上下按钮
#include <QSlider>   //滑块
#include <QHBoxLayout>  //水平布局


SmallWidget::SmallWidget()
{
    QSpinBox *spin = new QSpinBox();
    QSlider *slider = new QSlider();
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(spin);
    hLayout->addWidget(slider);
    connect(spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ) //因为valueChanged有两种类型 所以要进行强制转换

}
