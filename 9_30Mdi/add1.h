#ifndef ADD1_H
#define ADD1_H

#include <QWidget>
#pragma execution_character_set("utf-8")
namespace Ui {
class add1;
}

class add1 : public QWidget
{
    Q_OBJECT

public:
    explicit add1(QWidget *parent = nullptr);
    ~add1();

private:
    Ui::add1 *ui;
    void text();
};

#endif // ADD1_H
