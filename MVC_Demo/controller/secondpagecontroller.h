#ifndef SECONDPAGECONTROLLER_H
#define SECONDPAGECONTROLLER_H

#include <QObject>

class QSecondPageController : public QObject
{
    Q_OBJECT
public:    
    static QSecondPageController* GetInstance();

signals:
    void count_signal();

private:
    static QSecondPageController *m_instance;
    QSecondPageController();

private slots:
};

#endif // SECONDPAGECONTROLLER_H
