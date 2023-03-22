#ifndef ADD3_H
#define ADD3_H

#include <QWidget>
#pragma execution_character_set("utf-8")
namespace Ui {
class add3;
}

class add3 : public QWidget
{
    Q_OBJECT

public:
    explicit add3(QWidget *parent = nullptr);
    ~add3();
    void text();
private:
    Ui::add3 *ui;
};

#endif // ADD3_H
