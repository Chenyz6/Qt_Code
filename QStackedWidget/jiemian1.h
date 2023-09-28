#ifndef JIEMIAN1_H
#define JIEMIAN1_H

#include <QWidget>

namespace Ui {
class jiemian1;
}

class jiemian1 : public QWidget
{
    Q_OBJECT

public:
    explicit jiemian1(QWidget *parent = nullptr);
    ~jiemian1();

private:
    Ui::jiemian1 *ui;
};

#endif // JIEMIAN1_H
