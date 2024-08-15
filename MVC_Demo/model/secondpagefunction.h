#ifndef SECONDPAGEFUNCTION_H
#define SECONDPAGEFUNCTION_H

#include <QObject>

class QSecondPageFunction : public QObject
{
    Q_OBJECT
public:
    explicit QSecondPageFunction(QObject *parent = nullptr);

signals:

public slots:
    void count_slot();
};

#endif // SECONDPAGEFUNCTION_H
