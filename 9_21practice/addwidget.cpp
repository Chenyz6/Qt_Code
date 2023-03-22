#include "addwidget.h"

addwidget::addwidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("转换后");
    e.setParent(this);
    e.setText("CYZ");
    resize(500,500);
    connect(&e, &QPushButton::clicked, this, &addwidget::addconvert);
}



void addwidget::addconvert()
{
    emit slot();
}
