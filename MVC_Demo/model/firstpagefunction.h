#ifndef FIRSTPAGEFUNCTION_H
#define FIRSTPAGEFUNCTION_H

#include <QObject>

class QFirstPageFunction : public QObject
{
    Q_OBJECT
public:
    explicit QFirstPageFunction(QObject *parent = nullptr);

signals:

public slots:
    void count_slot();
};

#endif // FIRSTPAGEFUNCTION_H
