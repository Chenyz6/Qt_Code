#ifndef JIEMIAN2_H
#define JIEMIAN2_H

#include <QWidget>

namespace Ui {
class jiemian2;
}

class jiemian2 : public QWidget
{
    Q_OBJECT

public:
    explicit jiemian2(QWidget *parent = nullptr);
    ~jiemian2();

private:
    Ui::jiemian2 *ui;
};

#endif // JIEMIAN2_H
