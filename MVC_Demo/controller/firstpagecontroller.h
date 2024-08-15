#ifndef FIRSTPAGECONTROLLER_H
#define FIRSTPAGECONTROLLER_H

#include <QObject>
#include <QThread>
#include "model/firstpagefunction.h"

class QFirstPageController : public QObject
{
    Q_OBJECT
public:
    static QFirstPageController* GetInstance();

signals:
    void startedCount_signal();
    void finishedCount_signal();
    void showFinishedCountUI_signal();

private:
    static QFirstPageController *m_instance;
    QFirstPageController();
    void initController();

    QThread *threadCount;
    QFirstPageFunction *firstPageFunction;

private slots:
    void startCountSlot();
    void quitCountThreadSlot();
    void finishedCountThreadSlot();
};

#endif // FIRSTPAGECONTROLLER_H
